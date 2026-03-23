// Copyright 2023-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VERTEX_TRIE_H_
#define VERTEX_TRIE_H_

#include "Trie.h"

class VertexTrie final : public Trie
{
 public:
  VertexTrie();

  /* Sparse constructor */
  VertexTrie(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);

  /* Dense constructors */
  VertexTrie(vector<double>::const_iterator& membershipIt, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);
  VertexTrie(vector<double>::const_iterator& membershipIt, const int unit, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);

  void deleteHyperplanes();

  void sortTubes();

  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  void sumOnAddedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const;
  void minusSumOnRemovedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const;

#ifdef PRUNE
  void positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const;
  int increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const;
  long long sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const;
  void sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const;
  int sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;
  int increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;
  int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;
  int decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;

  // To insert candidates before firstCandidateElement
  void increaseSumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const;
  void increaseSumsOnNewCandidateVertices(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sums) const;

#else

  void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;
  void removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;
  void addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;
  void removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;

  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const;
  int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const;
  int decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const;
#endif

#if defined ASSERT || !defined PRUNE
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const;
  long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const;
#endif
};

#endif /*VERTEX_TRIE_H_*/
