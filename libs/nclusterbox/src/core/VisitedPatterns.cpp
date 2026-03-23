// Copyright 2023,2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VisitedPatterns.h"

#include <algorithm>

bool greaterG(const VisitedPattern* visitedPattern1, const VisitedPattern* visitedPattern2)
{
  return visitedPattern1->getG() > visitedPattern2->getG();
}

mutex VisitedPatterns::globalLock;

VisitedPatterns::VisitedPatterns(const float bytesForVisitedPatterns): noMoreCapacity(false), firstTuples(VisitedPattern::getNbOfIndices()), heap(), remainingCapacity(bytesForVisitedPatterns)
{
}

VisitedPatterns::~VisitedPatterns()
{
}

bool VisitedPatterns::tooBad(const double g) const
{
  const lock_guard<mutex> lock(globalLock);
  return g < heap.front()->getG();
}

void VisitedPatterns::eraseWorstPatterns()
{
  while (remainingCapacity < 0)
    {
      const VisitedPattern* patternToDelete = heap.front();
      pop_heap(heap.begin(), heap.end(), greaterG);
      heap.pop_back();
      remainingCapacity += patternToDelete->memUsage();
      pair<mutex, unordered_set<const VisitedPattern*, FlatNSetHash, FlatNSetEqual>>& firstTupleForErase = firstTuples[patternToDelete->getIndex()];
      firstTupleForErase.first.lock();
      firstTupleForErase.second.erase(patternToDelete);
      firstTupleForErase.first.unlock();
      globalLock.unlock();	// PERF: does it really help some tooBad calls to run or is releasing/getting the lock here just a waste of time?
      delete patternToDelete;
      globalLock.lock();
    }
}

bool VisitedPatterns::concurrentInsert(const VisitedPattern* visitedPattern)
{
  pair<mutex, unordered_set<const VisitedPattern*, FlatNSetHash, FlatNSetEqual>>& firstTupleForEmplace = firstTuples[visitedPattern->getIndex()];
  firstTupleForEmplace.first.lock();
  if (firstTupleForEmplace.second.emplace(visitedPattern).second)
    {
      firstTupleForEmplace.first.unlock();
      const unsigned int memUsageOfPattern = visitedPattern->memUsage();
      const lock_guard<mutex> lock(globalLock);
      remainingCapacity -= memUsageOfPattern;
      heap.push_back(visitedPattern);
      if (noMoreCapacity)
	{
	  push_heap(heap.begin(), heap.end(), greaterG);
	  eraseWorstPatterns();
	  return false;
	}
      if (remainingCapacity < 0)
	{
	  noMoreCapacity = true;
	  make_heap(heap.begin(), heap.end(), greaterG);
	  eraseWorstPatterns();
	}
      return false;
    }
  firstTupleForEmplace.first.unlock();
  delete visitedPattern;
  return true;
}

bool VisitedPatterns::visited(const vector<vector<unsigned int>>& nSet, const double g)
{
  if (noMoreCapacity && tooBad(g))
    {
      return false;
    }
  const vector<vector<unsigned int>>::const_iterator lastDimensionIt = --nSet.end();
  VisitedPattern* visitedPattern = new VisitedPattern(nSet, lastDimensionIt, g);
  visitedPattern->addLastDimension(*lastDimensionIt);
  return concurrentInsert(visitedPattern);
}

void VisitedPatterns::clear()
{
  firstTuples.clear();
  firstTuples.shrink_to_fit();
  for (const VisitedPattern* visitedPattern : heap)
    {
      delete visitedPattern;
    }
  heap.clear();
  heap.shrink_to_fit();
}
