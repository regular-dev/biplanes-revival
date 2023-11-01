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

int AI_Backend::getIndexByProb(const std::vector< std::pair< int, float > > &probs) const
{
    auto sum = 0.0;

    for (auto &it : probs)
        sum += it.second;

    std::uniform_real_distribution<> dis(0.0, sum);

    float p = dis(*m_rand);

    float cur_pos = 0.0;

    int idx = 0;
    for (int i = 0; i < probs.size(); ++i) {
        if (p >= cur_pos && p < cur_pos + probs[i].second) {
            idx = probs[i].first;
            break;
        }
        cur_pos += probs[i].second;
    }

    return idx;
}

void AI_Backend::train(const InputBatch &data, const Labels &lbls,
                       unsigned int batch_size, unsigned int epochs)
{
    log_message("Begin training...");

    const auto prev_loss = m_mdl->get_loss< tiny_dnn::cross_entropy >(data, lbls);
    log_message("Loss before training : {:.3f}", std::to_string(prev_loss));

    m_mdl->train< tiny_dnn::cross_entropy, Optimizer >(*m_opt, data, lbls,
                                                       batch_size, epochs);

    const auto loss = m_mdl->get_loss< tiny_dnn::cross_entropy >(data, lbls);
    log_message("Loss after training : {:.3f}", std::to_string(loss));
}

AI_Backend::Label AI_Backend::predictLabel(const EvalInput &in) const
{
    const auto out = m_mdl->predict(in);
    return m_mdl->predict_label(in);
}

AI_Backend::Label AI_Backend::predictDistLabel(const EvalInput &in, int constraint) const
{
    const auto mdl_out = m_mdl->predict(in);

    using pair_action_t = std::pair< int, float >;

    std::vector< pair_action_t > out_sorted;
    out_sorted.reserve( mdl_out.size() );

    for (int i = 0; i < mdl_out.size(); ++i)
        out_sorted.push_back( {i, mdl_out[i]} );

    std::sort(out_sorted.begin(), out_sorted.end(),
              [ ] (pair_action_t &a1, pair_action_t &a2)
    {
        return a1.second > a2.second;
    });

    if (constraint > 0 && constraint < in.size())
        out_sorted = { out_sorted.begin(), out_sorted.begin() + constraint };

    return getIndexByProb(out_sorted);
}

AI_Backend::Labels AI_Backend::predictBatchLabels(const InputBatch &in) const
{
    auto out = Labels( in.size() );

    for (auto i = 0; i < in.size(); ++i) {
        out[i] = m_mdl->predict_label( in[i] );
    }

    return out;
}

void AI_Backend::saveModel(const std::string &path) const
{
    m_mdl->save(path);
}

void AI_Backend::loadModel(const std::string &path)
{
    m_mdl->load(path);
}

void AI_Backend::initNet()
{
    using namespace tiny_dnn::activation;
    using namespace tiny_dnn::layers;
    using namespace tiny_dnn;

    const size_t inputSize = 52;
    const auto outputSize = static_cast <size_t> (AiAction::ActionCount);

    // model
    m_mdl = std::make_shared< network< sequential > >();

    *m_mdl  << fc(inputSize, 256) << relu()
            << fc(256, 128) << relu()
            << fc(128, 64) << relu()
            << fc(64, outputSize)
            << softmax_layer(outputSize);

    // optimizer
    m_opt = std::make_shared< RMSprop >();

    m_opt->alpha = 1e-3;
    m_opt->mu = 0.9;
}

void AI_Backend::init_rand()
{
    m_dev_rand = std::make_unique< std::random_device >();
    m_rand = std::make_unique< std::default_random_engine >((*m_dev_rand)());
}
