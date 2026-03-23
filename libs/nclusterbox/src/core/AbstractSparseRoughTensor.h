// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_SPARSE_ROUGH_TENSOR_H_
#define ABSTRACT_SPARSE_ROUGH_TENSOR_H_

#include "AbstractRoughTensor.h"

class AbstractSparseRoughTensor : virtual public AbstractRoughTensor
{
 public:
  AbstractSparseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double shift);
  virtual ~AbstractSparseRoughTensor();

  void setNoSelection();

  double getAverageShift(const vector<vector<unsigned int>>& nSet) const;
  void setUnitForProjectedTensor(const double totalShiftOnElementInFirstDimension);
  void fillProjectedTensor(AbstractTrieWithPrediction& tensor); // also frees the space that fuzzyTuples takes (the only reason for not being const)

  static vector<unsigned int> projectMetadataForDimension(const unsigned int internalDimensionId, const bool isReturningOld2New, vector<string>& ids2LabelsInDimension);

 protected:
  vector<FuzzyTuple> fuzzyTuples; /* non-empty if only if patterns are to be selected */
  /* PERF: a specific class for a 0/1 tensor where memberships are not stored */
  const double shift;
};

#endif /*ABSTRACT_SPARSE_ROUGH_TENSOR_H_*/
