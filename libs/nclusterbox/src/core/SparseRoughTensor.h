// Copyright 2018-2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef SPARSE_ROUGH_TENSOR_H_
#define SPARSE_ROUGH_TENSOR_H_

#include "AbstractSparseRoughTensor.h"

class SparseRoughTensor final : public AbstractSparseRoughTensor
{
 public:
  SparseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double shift);

  AbstractTrie* getTensor() const;
  VisitedPatterns* getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelection) const;
  bool wouldBeEmptyAfterProjection();
  AbstractTrieWithPrediction* projectTensor();
};

#endif /*SPARSE_ROUGH_TENSOR_H_*/
