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
    init_net();
}

AI_Backend::AI_Backend(const std::string &model_path)
{
    init_net();

    // TODO : impl loading
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
    return m_mdl->predict_label(in);
}

AI_Backend::Labels AI_Backend::predictBatchLabels(const InputBatch &in) const
{
    auto out = Labels( in.size() );

    for (auto i = 0; i < in.size(); ++i) {
        out[i] = m_mdl->predict_label( in[i] );
    }

    return out;
}

void AI_Backend::save_model(const std::string &path) const
{
    // TODO : impl
}

void AI_Backend::init_net()
{
    using namespace tiny_dnn::activation;
    using namespace tiny_dnn::layers;
    using namespace tiny_dnn;

    const size_t inputSize = 52;
    const auto outputSize = static_cast <size_t> (AiAction::EnumSize);

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
