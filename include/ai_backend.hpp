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

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <random>

#include "tiny_dnn/util/util.h"

namespace tiny_dnn {
  class sequential;

  template<typename T>
  class network;

  struct RMSprop;
}

class AI_Backend
{
public:
  using EvalInput = tiny_dnn::vec_t;
  using InputBatch = std::vector< tiny_dnn::vec_t >;
  using Label = tiny_dnn::label_t;
  using Labels = std::vector< Label >;
  using SeqNet = tiny_dnn::network< tiny_dnn::sequential >;
  using Optimizer = tiny_dnn::RMSprop;

  explicit AI_Backend();

  void initNet();

  void train(const InputBatch&, const Labels&,
             unsigned int batch_size, unsigned int epochs);

  Label predictLabel(const EvalInput&) const;
  Label predictDistLabel(const EvalInput&, int constraint = 0) const;
  Labels predictBatchLabels(const InputBatch&) const;

  void saveModel(const std::string &path) const;
  void loadModel(const std::string &path);

  int getIndexByProb(const std::vector< std::pair< int, float > > &probs) const;

protected:
  std::shared_ptr< SeqNet > m_mdl;
  std::shared_ptr< Optimizer > m_opt;

  std::unique_ptr<std::random_device> m_dev_rand;
  std::unique_ptr<std::default_random_engine> m_rand;

  void init_rand();
};
