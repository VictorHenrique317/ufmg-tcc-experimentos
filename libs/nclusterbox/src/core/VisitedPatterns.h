// Copyright 2023,2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VISITED_PATTERNS_H_
#define VISITED_PATTERNS_H_

#include <mutex>
#include <unordered_set>

#include "VisitedPattern.h"

class VisitedPatterns
{
public:
  VisitedPatterns(const float bytesForVisitedPatterns);

  virtual ~VisitedPatterns();

  virtual bool visited(const vector<vector<unsigned int>>& nSet, const double g);
  void clear();

protected:
  bool noMoreCapacity;

  bool tooBad(const double g) const;
  bool concurrentInsert(const VisitedPattern* visitedPattern);

private:
  vector<pair<mutex, unordered_set<const VisitedPattern*, FlatNSetHash, FlatNSetEqual>>> firstTuples;
  vector<const VisitedPattern*> heap;
  long long remainingCapacity;

  static mutex globalLock;

  void eraseWorstPatterns();
};

#endif /*VISITED_PATTERNS_H_*/
