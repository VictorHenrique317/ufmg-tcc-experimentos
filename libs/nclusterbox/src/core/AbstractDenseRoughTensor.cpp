// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "AbstractDenseRoughTensor.h"

#include "ConstantShift.h"

AbstractDenseRoughTensor::AbstractDenseRoughTensor(): shift(), memberships()
{
}

AbstractDenseRoughTensor::AbstractDenseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double constantShift): shift(new ConstantShift(constantShift)), memberships()
{
}

AbstractDenseRoughTensor::~AbstractDenseRoughTensor()
{
  delete shift;
}

void AbstractDenseRoughTensor::setNoSelection()
{
  memberships.clear();
  memberships.shrink_to_fit();
}

double AbstractDenseRoughTensor::getAverageShift(const vector<vector<unsigned int>>& nSet) const
{
  return shift->getAverageShift(nSet);
}

bool AbstractDenseRoughTensor::wouldBeEmptyAfterProjection()
{
  return false;
}
