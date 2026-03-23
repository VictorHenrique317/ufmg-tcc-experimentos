// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "AbstractSparseRoughTensor.h"
#include "TupleWithPrediction.h"

#include <boost/dynamic_bitset.hpp>

using namespace boost;

AbstractSparseRoughTensor::AbstractSparseRoughTensor(vector<FuzzyTuple>& fuzzyTuplesParam, const double shiftParam): fuzzyTuples(std::move(fuzzyTuplesParam)), shift(shiftParam)
{
}

AbstractSparseRoughTensor::~AbstractSparseRoughTensor()
{
}

void AbstractSparseRoughTensor::setNoSelection()
{
  fuzzyTuples.clear();
  fuzzyTuples.shrink_to_fit();
}

double AbstractSparseRoughTensor::getAverageShift(const vector<vector<unsigned int>>& nSet) const
{
  return shift;
}

void AbstractSparseRoughTensor::setUnitForProjectedTensor(const double totalShiftOnElementInFirstDimension)
{
  // Compute negative/positive memberships of elements in first dimension and the RSS of the null model
  vector<double> elementPositiveMemberships(cardinalities.front());
  vector<double> elementNegativeMemberships(cardinalities.front(), totalShiftOnElementInFirstDimension);
  const vector<FuzzyTuple>::const_iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::const_iterator fuzzyTupleIt = fuzzyTuples.begin();
  double rss = (totalShiftOnElementInFirstDimension * cardinalities.front() - (fuzzyTupleEnd - fuzzyTupleIt) * shift) * shift;
  do
    {
      const double membership = fuzzyTupleIt->getMembership();
      rss += membership * membership;
      const unsigned int elementId = fuzzyTupleIt->getElementId(0);
      if (membership > 0)
	{
	  elementPositiveMemberships[elementId] += membership;
	  elementNegativeMemberships[elementId] -= shift;
	}
      else
	{
	  elementNegativeMemberships[elementId] -= membership + shift;
	}
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
  // Compute unit
  AbstractRoughTensor::setUnitForProjectedTensor(rss, max(*max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end()), *max_element(elementPositiveMemberships.begin(), elementPositiveMemberships.end())));
  TupleWithPrediction::setDefaultMembership(unit * -shift);
}

void AbstractSparseRoughTensor::fillProjectedTensor(AbstractTrieWithPrediction& tensor)
{
  {
    // Fill the tensor
    const vector<FuzzyTuple>::const_iterator fuzzyTupleEnd = fuzzyTuples.end();
    vector<FuzzyTuple>::const_iterator fuzzyTupleIt = fuzzyTuples.begin();
    do
      {
	tensor.setTuple(fuzzyTupleIt->getTuple().begin(), unit * fuzzyTupleIt->getMembership());
      }
    while (++fuzzyTupleIt != fuzzyTupleEnd);
  }
  // Free the space that fuzzyTuples takes
  setNoSelection();
}

vector<unsigned int> AbstractSparseRoughTensor::projectMetadataForDimension(const unsigned int internalDimensionId, const bool isReturningOld2New, vector<string>& ids2LabelsInDimension)
{
  unsigned int& cardinality = cardinalities[internalDimensionId];
  dynamic_bitset<> elementsInDimension(cardinality);
  const vector<vector<vector<unsigned int>>>::const_iterator end = candidateVariables.end();
  vector<vector<vector<unsigned int>>>::const_iterator patternIt = candidateVariables.begin();
  do
    {
      const vector<unsigned int>::const_iterator idEnd = (*patternIt)[internalDimensionId].end();
      vector<unsigned int>::const_iterator idIt = (*patternIt)[internalDimensionId].begin();
      do
	{
	  elementsInDimension.set(*idIt);
	}
      while (++idIt != idEnd);
    }
  while (++patternIt != end);
  vector<unsigned int> oldIds2NewIdsInDimension(cardinality, numeric_limits<unsigned int>::max());
  cardinality = 0;
  if (isReturningOld2New)
    {
      dynamic_bitset<>::size_type id = elementsInDimension.find_first();
      do
	{
	  ids2LabelsInDimension[id].swap(ids2LabelsInDimension[cardinality]);
	  oldIds2NewIdsInDimension[id] = cardinality++;
	  id = elementsInDimension.find_next(id);
	}
      while (id != dynamic_bitset<>::npos);
    }
  else
    {
      vector<unsigned int> newIds2OldIdsInDimension;
      newIds2OldIdsInDimension.reserve(elementsInDimension.count());
      dynamic_bitset<>::size_type id = elementsInDimension.find_first();
      do
	{
	  newIds2OldIdsInDimension.push_back(id);
	  ids2LabelsInDimension[id].swap(ids2LabelsInDimension[cardinality]);
	  oldIds2NewIdsInDimension[id] = cardinality++;
	  id = elementsInDimension.find_next(id);
	}
      while (id != dynamic_bitset<>::npos);
    }
  ids2LabelsInDimension.resize(cardinality);
  ids2LabelsInDimension.shrink_to_fit();
  const vector<vector<vector<unsigned int>>>::iterator candidateVariableEnd = candidateVariables.end();
  vector<vector<vector<unsigned int>>>::iterator candidateVariableIt = candidateVariables.begin();
  do
    {
      const vector<unsigned int>::iterator idEnd = (*candidateVariableIt)[internalDimensionId].end();
      vector<unsigned int>::iterator idIt = (*candidateVariableIt)[internalDimensionId].begin();
      do
	{
	  *idIt = oldIds2NewIdsInDimension[*idIt];
	}
      while (++idIt != idEnd);
    }
  while (++candidateVariableIt != candidateVariableEnd);
  return oldIds2NewIdsInDimension;
}
