// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_DENSE_ROUGH_TENSOR_H_
#define ABSTRACT_DENSE_ROUGH_TENSOR_H_

#include "AbstractRoughTensor.h"
#include "AbstractShift.h"

class AbstractDenseRoughTensor : virtual public AbstractRoughTensor
{
 public:
  AbstractDenseRoughTensor(const AbstractDenseRoughTensor& otherAbstractDenseRoughTensor) = delete;
  AbstractDenseRoughTensor();
  AbstractDenseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double constantShift);
  virtual ~AbstractDenseRoughTensor();

  AbstractDenseRoughTensor& operator=(const AbstractDenseRoughTensor& otherAbstractDenseRoughTensor) const = delete;

  void setNoSelection();

  double getAverageShift(const vector<vector<unsigned int>>& nSet) const;

  bool wouldBeEmptyAfterProjection();

 protected:
  AbstractShift* shift;
  vector<double> memberships; /* non-empty if only if patterns are to be selected */
  /* PERF: a specific class for a 0/1 tensor where memberships are stored in a dynamic_bitset */
};

#endif /*ABSTRACT_DENSE_ROUGH_TENSOR_H_*/
