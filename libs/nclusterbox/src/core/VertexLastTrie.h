// Copyright 2024,2025 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VERTEX_LAST_TRIE_H_
#define VERTEX_LAST_TRIE_H_

#include "LastTrie.h"

class VertexLastTrie final : public LastTrie
{
 public:
  VertexLastTrie();
  VertexLastTrie(VertexLastTrie&& otherVertexLastTrie);

  /* Sparse constructor */
  VertexLastTrie(const unsigned int cardinality);

  /* Dense constructors */
  VertexLastTrie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality);
  VertexLastTrie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality);

  void deleteHyperplanes();

  void setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension);
  void setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension, const int membership);
  void sortTubes();

  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */

#ifdef PRUNE
  void positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const;
  int increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const;
  long long sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const;
  void sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const;
  int sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;

#else

  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
#endif

#if defined ASSERT || !defined PRUNE
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const;
#endif
};

#endif /*VERTEX_LAST_TRIE_H_*/
