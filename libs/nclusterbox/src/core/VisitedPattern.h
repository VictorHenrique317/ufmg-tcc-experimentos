// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VISITED_PATTERN_H_
#define VISITED_PATTERN_H_

#include <vector>
#include <deque>

using namespace std;

class VisitedPattern
{
 public:
  VisitedPattern(const VisitedPattern& otherVisitedPattern) = delete;
  VisitedPattern(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::const_iterator indexedNSetEnd, const double g);

  unsigned int getIndex() const;
  const deque<unsigned int>& getFlatNSet() const; // a deque rather than a vector to avoid memory framentation once the memory cap reached (the space freed removing usually-small visited patterns does not allow to continuously store the new and usually-large visited patterns)
  double getG() const;
  unsigned int memUsage() const;

  void addLastDimension(const vector<unsigned int>& lastDimension);

  static unsigned int init(const vector<unsigned int>& cardinalities, const vector<unsigned int>::const_iterator cardinalityEnd);
  static unsigned int getNbOfIndices();

 private:
  unsigned int index;
  deque<unsigned int> flatNSet;
  double g;

  static vector<unsigned int> elementOffsets;
  static vector<unsigned int> tupleOffsets;
};

#include <boost/functional/hash.hpp>

struct FlatNSetHash
{
  size_t operator()(const VisitedPattern* visitedPattern) const
  {
    return boost::hash_range(visitedPattern->getFlatNSet().begin(), visitedPattern->getFlatNSet().end());
  }
};

struct FlatNSetEqual
{
  bool operator()(const VisitedPattern* visitedPattern, const VisitedPattern* otherVisitedPattern) const
  {
    return visitedPattern->getFlatNSet() == otherVisitedPattern->getFlatNSet();
  }
};

#endif /*VISITED_PATTERN_H_*/
