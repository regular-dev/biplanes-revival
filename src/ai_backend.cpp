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

#include "tiny_dnn/lossfunctions/loss_function.h"
#include <include/ai_backend.hpp>
#include <include/enums.hpp>

#include <tiny_dnn/tiny_dnn.h>


AI_Backend::AI_Backend()
{
    init_rand();
    initNet();
}

size_t AI_Backend::getTrainedEpochsCount() const
{
    return m_epochs_trained;
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
        } catch(const tiny_dnn::nn_error&) {
            return {};
        }
    }

    return out;
}

void AI_Backend::train(const InputBatch &data, const InputBatch &lbls,
                       size_t batch_size, size_t epochs)
{
    m_epochs_trained += epochs;

    for ( size_t epoch = 0; epoch < epochs; ++epoch )
      m_mdl->fit< tiny_dnn::mse, Optimizer >(*m_opt, data, lbls, batch_size, epochs);
}

float AI_Backend::getLoss(const InputBatch& data, const InputBatch& lbls) const
{
    return m_mdl->get_loss< tiny_dnn::mse >(data, lbls) / data.size();
}

float AI_Backend::predictReward(const EvalInput& in ) const
{
    return m_mdl->predict(in).at(0);
}

bool AI_Backend::saveModel(const std::string &path) const
{
    return m_mdl->save(path);
}

static void fixModelDropout(AI_Backend& backend)
{
    const size_t inputSize = AiDatasetIndices::IndexCount + size_t(AiAction::ActionCount);
    constexpr size_t outputSize = 1;

//    Train 1 epoch on zeroed dataset
    backend.train(
        {tiny_dnn::vec_t(inputSize, 0.f)},
        {tiny_dnn::vec_t(outputSize, 0.f)},
        1, 1);
}

bool AI_Backend::loadModel(const std::string &path)
{
//    --- BUG ---
//    Model needs to be trained at least once
//    for the dropout to load correctly
    fixModelDropout(*this);
//    --- END-BUG ---

    m_epochs_trained = 0;
    return m_mdl->load(path);
}

void AI_Backend::initNet()
{
    using namespace tiny_dnn::activation;
    using namespace tiny_dnn::layers;
    using namespace tiny_dnn;

    const size_t inputSize = AiDatasetIndices::IndexCount + size_t(AiAction::ActionCount);
    const auto outputSize = 1;

    // model
    m_mdl = std::make_shared< network< sequential > >();

    *m_mdl << fc(inputSize, 256) << relu()
           << fc(256, 128) << relu()
           << dropout(128, 0.2)
           << fc(128, 64) << relu()
           << dropout(64, 0.2)
           << fc(64, outputSize) << sigmoid();

    // optimizer
    m_opt = std::make_shared< RMSprop >();

    m_opt->alpha = 3e-4;
    m_opt->mu = 0.9;

    m_epochs_trained = 0;
}

void AI_Backend::init_rand()
{
    m_rand = std::default_random_engine(m_dev_rand());
}
