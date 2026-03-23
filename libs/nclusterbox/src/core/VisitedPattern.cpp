// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VisitedPattern.h"

vector<unsigned int> VisitedPattern::elementOffsets;
vector<unsigned int> VisitedPattern::tupleOffsets;

VisitedPattern::VisitedPattern(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::const_iterator indexedNSetEnd, const double gParam): index(), flatNSet(), g(gParam)
{
  vector<vector<unsigned int>>::const_iterator nSetIt = nSet.begin();
  vector<unsigned int>::const_iterator elementIt = nSetIt->begin();
  index = *elementIt;
  flatNSet.insert(flatNSet.end(), ++elementIt, nSetIt->end());
  if (++nSetIt != indexedNSetEnd)
    {
      vector<unsigned int>::const_iterator elementOffsetIt = elementOffsets.begin();
      vector<unsigned int>::const_iterator tupleOffsetIt = tupleOffsets.begin();
      for (; ; )
	{
	  elementIt = nSetIt->begin();
	  index += *elementIt * *tupleOffsetIt;
	  for (const vector<unsigned int>::const_iterator elementEnd = nSetIt->end(); ++elementIt != elementEnd; )
	    {
	      flatNSet.push_back(*elementIt + *elementOffsetIt);
	    }
	  if (++nSetIt == indexedNSetEnd)
	    {
	      return;
	    }
	  ++elementOffsetIt;
	  ++tupleOffsetIt;
	}
    }
}

unsigned int VisitedPattern::getIndex() const
{
  return index;
}

const deque<unsigned int>& VisitedPattern::getFlatNSet() const
{
  return flatNSet;
}

double VisitedPattern::getG() const
{
  return g;
}

unsigned int VisitedPattern::memUsage() const
{
  return sizeof(VisitedPattern) + 5 * sizeof(VisitedPattern*) + (sizeof(unsigned int) * flatNSet.size() / 4096 + 1) * (4096 + sizeof(VisitedPattern*)); // 5 * sizeof(VisitedPattern*) because one pointer in the heap, one in the unordered_set, one for the block (assuming as many blocks as VisitedPattern objects) and two for the list inside the block (the usual way to handle collisions); sizeof(unsigned int) * flatNSet.size() / 4096 + 1 is the number of internal 4096-byte arrays to store flatNSet (a deque, assuming its last internal array is not full) and to access those arrays through a VisitedPattern*
}

void VisitedPattern::addLastDimension(const vector<unsigned int>& lastDimension)
{
  // Not a graph pattern: every element of lastDimension must be added with offset to flatNSet
  const vector<unsigned int>::const_iterator elementEnd = lastDimension.end();
  vector<unsigned int>::const_iterator elementIt = lastDimension.begin();
  const unsigned int lastOffset = elementOffsets.back();
  do
    {
      flatNSet.push_back(*elementIt + lastOffset);
    }
  while (++elementIt != elementEnd);
}

unsigned int VisitedPattern::init(const vector<unsigned int>& cardinalities, const vector<unsigned int>::const_iterator cardinalityEnd)
{
  vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
  tupleOffsets.reserve(cardinalityEnd - cardinalityIt);
  elementOffsets.reserve(cardinalityEnd - cardinalityIt);
  tupleOffsets.push_back(*cardinalityIt);
  elementOffsets.push_back(*cardinalityIt);
  while (++cardinalityIt != cardinalityEnd)
    {
      tupleOffsets.push_back(*cardinalityIt * tupleOffsets.back());
      elementOffsets.push_back(*cardinalityIt + elementOffsets.back());
    }
  return tupleOffsets.back();
}

unsigned int VisitedPattern::getNbOfIndices()
{
  return tupleOffsets.back();
}
