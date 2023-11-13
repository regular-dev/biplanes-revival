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
             size_t batch_size, size_t epochs);

  void train(const InputBatch&, const InputBatch& lbls, size_t batch_size, size_t epochs);

  float getLoss(const InputBatch&, const InputBatch& lbls) const;

  Label predictLabel(const EvalInput&) const;
  Label predictDistLabel(const EvalInput&, size_t constraint = 0); // not const cause of rand_engine
  std::vector <size_t> predictDistLabels(const EvalInput&); // not const cause of rand_engine
  std::vector <std::pair <size_t, float>> predictDistLabelsProb(const EvalInput&); // not const cause of rand_engine
  Labels predictBatchLabels(const InputBatch&) const;

  bool saveModel(const std::string &path) const;
  bool loadModel(const std::string &path);

  size_t getIndexByProb(const std::vector< std::pair< size_t, float > > &probs);
  size_t getRandomIndex(const std::vector< size_t> &, const size_t constraint = 0);

  std::vector< float > getWeights();

protected:
  std::shared_ptr< SeqNet > m_mdl;
  std::shared_ptr< Optimizer > m_opt;

  std::random_device m_dev_rand;
  std::default_random_engine m_rand;

  void init_rand();
};
