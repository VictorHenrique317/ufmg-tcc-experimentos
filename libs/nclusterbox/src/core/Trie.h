// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TRIE_H_
#define TRIE_H_

#include "AbstractTrie.h"

class Trie : public AbstractTrie
{
 public:
  Trie();
  Trie(Trie&& otherTrie);

  /* Sparse constructors */
  Trie(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);
  Trie(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator cardinalityEnd, const unsigned int distanceToVertexDimension);

  /* Dense constructors */
  Trie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);
  Trie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);
  Trie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension);
  Trie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension);

  ~Trie();

  virtual void deleteHyperplanes();

  void setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension);
  void setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension, const int membership);
  virtual void sortTubes();

  virtual void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  virtual void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const; /* warning: sum cannot exceed numeric_limits<int>::max()! */
  virtual void sumOnAddedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const;
  virtual void minusSumOnRemovedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const;
  void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const; // only follow element; *dimensionIt is the next dimension
  void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const; // only follow element; *dimensionIt is the next dimension

#ifdef PRUNE
  virtual void positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const;
  virtual int increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const;
  virtual long long sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const;
  virtual void sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const;
  virtual int sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const;
  int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const;
  int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;
  virtual int increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual void addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;
  virtual int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual void removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;
  virtual int decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual void removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const;

  // To insert candidates before firstCandidateElement
  virtual void increaseSumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const;
  void sumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::iterator dimensionWithCandidatesIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes) const;
  void increaseSumsOnNewCandidateElementsOfThisDimension(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const;
  void increaseSumsOnNewCandidateVertices(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sums) const;

#else

  virtual void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;
  virtual void removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;
  virtual void addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;
  virtual void removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const;

  virtual int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const;
  virtual void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  virtual int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const;
  virtual void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  virtual int increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual int decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const;
  virtual int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const;
#endif

#if defined ASSERT || !defined PRUNE
  virtual int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const;
  virtual long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sums) const;
  void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const;
#endif

 protected:
  vector<AbstractTrie*> hyperplanes;
};

#endif /*TRIE_H_*/
