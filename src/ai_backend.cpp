/*
  Biplanes Revival
  Copyright (C) 2019-2023 Regular-dev community
  https://regular-dev.org
  regular.dev.org@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <include/ai_backend.hpp>
#include <include/enums.hpp>
#include <include/utility.hpp>

#include <tiny_dnn/tiny_dnn.h>


AI_Backend::AI_Backend()
{
    init_rand();
    initNet();
}

size_t AI_Backend::getIndexByProb(const std::vector< std::pair< size_t, float > > &probs)
{
    auto sum = 0.0;

    for (auto &it : probs)
        sum += it.second;

    std::uniform_real_distribution<> dis(0.0, sum);

    float p = dis(m_rand);

    float cur_pos = 0.0;

    int idx = 0;
    for (size_t i = 0; i < probs.size(); ++i) {
        if (p >= cur_pos && p < cur_pos + probs[i].second) {
            idx = probs[i].first;
            break;
        }
        cur_pos += probs[i].second;
    }

    return idx;
}

size_t AI_Backend::getRandomIndex(const std::vector< size_t >& labels, const size_t constraint)
{
  std::uniform_int_distribution<> dis(0,
    constraint < labels.size()
    ? constraint
    : labels.size() - 1);

    return labels.at(dis(m_rand));
}

std::vector<float> AI_Backend::getWeights()
{
    std::vector< float > out;
    out.reserve(256 * 128);

    for (auto i = 0; i < m_mdl->layer_size(); ++i) {
        try {
            auto &l = m_mdl->at<tiny_dnn::layers::fc>(i);

            for (auto &iw : l.weights()) {
                for (auto it = iw->begin(); it != iw->end(); ++it) {
                    out.push_back(*it);
            }
        }
        } catch(tiny_dnn::nn_error) {
            continue;
        }
    }

    return out;
}

void AI_Backend::train(const InputBatch &data, const Labels &lbls,
                       size_t batch_size, size_t epochs)
{
    log_message("Begin labeled training...\n");
    m_mdl->train< tiny_dnn::cross_entropy, Optimizer >(*m_opt, data, lbls,
                                                       batch_size, epochs);
}

void AI_Backend::train(const InputBatch &data, const InputBatch &lbls, size_t batch_size, size_t epochs)
{
    log_message("Begin training...\n");

    for ( size_t epoch = 0; epoch < epochs; ++epoch )
    {
      const auto prev_loss = getLoss(data, lbls);
      log_message("Loss before training epoch " + std::to_string(epoch) + ": " + std::to_string(prev_loss), "\n");

      m_mdl->fit< tiny_dnn::cross_entropy, Optimizer >(*m_opt, data, lbls, batch_size, epochs);

      const auto loss = getLoss(data, lbls);
      log_message("Loss after training epoch " + std::to_string(epoch) + ": " + std::to_string(loss), "\n");
    }
}

float AI_Backend::getLoss(const InputBatch& data, const InputBatch& lbls) const
{
    return m_mdl->get_loss< tiny_dnn::cross_entropy >(data, lbls) / data.size();
}

AI_Backend::Label AI_Backend::predictDistLabel(const EvalInput &in, size_t constraint)
{
    const auto mdl_out = m_mdl->predict(in);

    using pair_action_t = std::pair< size_t, float >;

    std::vector< pair_action_t > out_sorted;
    out_sorted.reserve( mdl_out.size() );

    for (size_t i = 0; i < mdl_out.size(); ++i)
        out_sorted.push_back( {i, mdl_out[i]} );

    std::sort(out_sorted.begin(), out_sorted.end(),
              [ ] (pair_action_t &a1, pair_action_t &a2)
    {
        return a1.second > a2.second;
    });

    if (constraint > 0 && constraint < out_sorted.size())
        out_sorted = { out_sorted.begin(), out_sorted.begin() + constraint };

    return getIndexByProb(out_sorted);
}

std::vector <size_t> AI_Backend::predictDistLabels(const EvalInput &in)
{
  const auto mdl_out = m_mdl->predict(in);

  using pair_action_t = std::pair< size_t, float >;

  std::vector< pair_action_t > out_sorted;
  out_sorted.reserve( mdl_out.size() );

  for (size_t i = 0; i < mdl_out.size(); ++i)
      out_sorted.push_back( {i, mdl_out[i]} );

  std::sort(out_sorted.begin(), out_sorted.end(),
            [ ] (pair_action_t &a1, pair_action_t &a2)
  {
      return a1.second > a2.second;
  });

  std::vector <size_t> result {};
  result.reserve(out_sorted.size());

  for ( const auto& [action, prob] : out_sorted )
    result.push_back(action);

  return result;
}

std::vector <std::pair< size_t, float >> AI_Backend::predictDistLabelsProb(const EvalInput &in)
{
  const auto mdl_out = m_mdl->predict(in);

  using pair_action_t = std::pair< size_t, float >;

  std::vector< pair_action_t > out_sorted;
  out_sorted.reserve( mdl_out.size() );

  for (size_t i = 0; i < mdl_out.size(); ++i)
      out_sorted.push_back( {i, mdl_out[i]} );

  std::sort(out_sorted.begin(), out_sorted.end(),
            [ ] (pair_action_t &a1, pair_action_t &a2)
  {
      return a1.second > a2.second;
  });

  return out_sorted;
}

AI_Backend::Labels AI_Backend::predictBatchLabels(const InputBatch &in) const
{
    auto out = Labels( in.size() );

    for (auto i = 0; i < in.size(); ++i) {
        out[i] = m_mdl->predict_label( in[i] );
    }

    return out;
}

bool AI_Backend::saveModel(const std::string &path) const
{
    return m_mdl->save(path);
}

bool AI_Backend::loadModel(const std::string &path)
{
    return m_mdl->load(path);
}

void AI_Backend::initNet()
{
    using namespace tiny_dnn::activation;
    using namespace tiny_dnn::layers;
    using namespace tiny_dnn;

    const size_t inputSize = AiDatasetIndices::IndexCount;
    const auto outputSize = static_cast <size_t> (AiAction::ActionCount);

    // model
    m_mdl = std::make_shared< network< sequential > >();

    *m_mdl << fc(inputSize, 256) << relu()
           << fc(256, 128) << relu() << dropout(128, 0.2)
           << fc(128, 64) << relu() << dropout(64, 0.2)
           << fc(64, outputSize)
           << softmax_layer(outputSize);

    // optimizer
    m_opt = std::make_shared< RMSprop >();

    m_opt->alpha = 3e-4;
    m_opt->mu = 0.9;
}

void AI_Backend::init_rand()
{
    m_rand = std::default_random_engine(m_dev_rand());
}
