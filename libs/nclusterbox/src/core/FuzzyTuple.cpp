// Copyright 2018-2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "FuzzyTuple.h"

#include <limits>

FuzzyTuple::FuzzyTuple(vector<unsigned int>& tupleParam, const double membershipParam): tuple(std::move(tupleParam)), membership(membershipParam)
{
}

FuzzyTuple::FuzzyTuple(const vector<vector<unsigned int>::const_iterator>& tupleIts, const double membershipParam): tuple(), membership(membershipParam)
{
  vector<vector<unsigned int>::const_iterator>::const_iterator tupleItsIt = tupleIts.begin();
  const vector<vector<unsigned int>::const_iterator>::const_iterator tupleItsEnd = tupleIts.end();
  tuple.reserve(tupleItsEnd - tupleItsIt);
  tuple.push_back(**tupleItsIt);
  ++tupleItsIt;
  do
    {
      tuple.push_back(**tupleItsIt);
    }
  while (++tupleItsIt != tupleItsEnd);
}

FuzzyTuple& FuzzyTuple::operator=(FuzzyTuple&& otherFuzzyTuple)
{
  tuple = std::move(otherFuzzyTuple.tuple);
  membership = otherFuzzyTuple.membership;
  return *this;
}

bool FuzzyTuple::operator==(const FuzzyTuple& otherFuzzyTuple) const
{
  return tuple == otherFuzzyTuple.tuple;
}

bool FuzzyTuple::operator<(const FuzzyTuple& otherFuzzyTuple) const
{
  return tuple > otherFuzzyTuple.tuple;
}

const vector<unsigned int>& FuzzyTuple::getTuple() const
{
  return tuple;
}

double FuzzyTuple::getMembership() const
{
  return membership;
}

double FuzzyTuple::getMembershipSquared() const
{
  return membership * membership;
}

unsigned int FuzzyTuple::getElementId(const unsigned int dimensionId) const
{
  return tuple[dimensionId];
}

void FuzzyTuple::reorder(const vector<unsigned int>& oldOrder2NewOrder)
{
  vector<unsigned int>::const_iterator idIt = tuple.begin();
  const vector<unsigned int>::const_iterator idEnd = tuple.end();
  vector<unsigned int> newTuple(idEnd - idIt);
  vector<unsigned int>::const_iterator newDimensionIdIt = oldOrder2NewOrder.begin();
  newTuple[*newDimensionIdIt] = *idIt++;
  do
    {
      newTuple[*++newDimensionIdIt] = *idIt;
    }
  while (++idIt != idEnd);
  tuple = std::move(newTuple);
}

void FuzzyTuple::ifSmallerIdSwapWithNext(const unsigned int dimensionId)
{
  const vector<unsigned int>::iterator id1It = tuple.begin() + dimensionId;
  const vector<unsigned int>::iterator id2It = id1It + 1;
  if (*id1It < *id2It)
    {
      iter_swap(id1It, id2It);
    }
}

void FuzzyTuple::shiftMembership(const double shift)
{
  membership -= shift;
}

void FuzzyTuple::remapElements(const unsigned int dimensionId, const vector<unsigned int>& mapping, vector<FuzzyTuple>& fuzzyTuples)
{
  const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  do
    {
      unsigned int& elementId = fuzzyTupleIt->tuple[dimensionId];
      elementId = mapping[elementId];
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
}

void FuzzyTuple::remapElements(const unsigned int dimension1Id, const unsigned int dimension2Id, const vector<unsigned int>& mapping, vector<FuzzyTuple>& fuzzyTuples)
{
  const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  do
    {
      unsigned int& element1Id = fuzzyTupleIt->tuple[dimension1Id];
      element1Id = mapping[element1Id];
      unsigned int& element2Id = fuzzyTupleIt->tuple[dimension2Id];
      element2Id = mapping[element2Id];
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
}

void FuzzyTuple::remapElementsToProject(const unsigned int dimensionId, const vector<unsigned int>& mapping, vector<FuzzyTuple>& fuzzyTuples)
{
  vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  do
    {
      unsigned int& elementId = fuzzyTupleIt->tuple[dimensionId];
      elementId = mapping[elementId];
      if (elementId == numeric_limits<unsigned int>::max())
	{
	  *fuzzyTupleIt = std::move(*--fuzzyTupleEnd);
	}
      else
	{
	  ++fuzzyTupleIt;
	}
    }
  while (fuzzyTupleIt != fuzzyTupleEnd);
  fuzzyTuples.erase(fuzzyTupleEnd, fuzzyTuples.end());
}

void FuzzyTuple::remapElementsToProject(const unsigned int dimension1Id, const unsigned int dimension2Id, const vector<unsigned int>& mapping, vector<FuzzyTuple>& fuzzyTuples)
{
  vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  do
    {
      unsigned int& element1Id = fuzzyTupleIt->tuple[dimension1Id];
      element1Id = mapping[element1Id];
      if (element1Id == numeric_limits<unsigned int>::max())
	{
	  *fuzzyTupleIt = std::move(*--fuzzyTupleEnd);
	}
      else
	{
	  unsigned int& element2Id = fuzzyTupleIt->tuple[dimension2Id];
	  element2Id = mapping[element2Id];
	  if (element2Id == numeric_limits<unsigned int>::max())
	    {
	      *fuzzyTupleIt = std::move(*--fuzzyTupleEnd);
	    }
	  else
	    {
	      ++fuzzyTupleIt;
	    }
	}
    }
  while (fuzzyTupleIt != fuzzyTupleEnd);
  fuzzyTuples.erase(fuzzyTupleEnd, fuzzyTuples.end());
}
