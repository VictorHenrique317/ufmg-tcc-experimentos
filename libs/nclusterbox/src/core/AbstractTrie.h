// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_TRIE_H_
#define ABSTRACT_TRIE_H_

#include <vector>

#include "../../Parameters.h"

using namespace std;

class AbstractTrie
{
 public:
  virtual ~AbstractTrie();

  virtual void deleteHyperplanes() = 0;

#ifdef PRUNE
  void positiveMembershipsOnHyperplanes(vector<vector<int>>& elementPositiveMemberships, const int unit) const;
  long long sumOnPattern(const vector<vector<unsigned int>>::const_iterator nSetBegin, const unsigned long long area, const int unit) const;
  void sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit) const;
  void sumsOnGraphHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit, const unsigned int vertexDimension) const;

  virtual void positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual long long sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const = 0;
  virtual void sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sums) const = 0;
  virtual int sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const = 0;
  virtual int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const = 0;
  virtual int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const = 0;
  virtual int increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const = 0;
  virtual int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const = 0;
  virtual int decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const = 0;

  void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums, const long long area, const int unit) const;
  void addNonVertexElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums, const long long area, const int unit, const unsigned int vertexDimensionId) const;
  void addVertex(const unsigned int increasedDimensionId, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums, const long long area, const int unit) const;

  // To insert candidates before firstCandidateElement
  virtual void increaseSumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const = 0;
  virtual void sumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::iterator dimensionWithCandidatesIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes) const = 0;
  virtual void increaseSumsOnNewCandidateVertices(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sums) const = 0;
  void sumsOnNewCandidateVertices(const vector<vector<unsigned int>>& nSet, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sumsOnHyperplanes) const;
  virtual void increaseSumsOnNewCandidateElementsOfThisDimension(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const = 0;
  void sumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::iterator dimensionWithCandidatesIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes, const unsigned long long area, const int unit) const;
  void sumsOnNewCandidateVertices(const vector<vector<unsigned int>>& nSet, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes, const unsigned long long areaOfACandidate, const int unit) const;

#else

  // Functions for !is01
  virtual void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const = 0;
  virtual void removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const = 0;
  virtual void addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const = 0;
  virtual void removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const = 0;

  // Functions for is01
  void addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums, const long long area, const int unit) const;
  void addNonVertexElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums, const long long area, const int unit, const unsigned int vertexDimension) const;
  void addVertex(const unsigned int increasedDimensionId, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums, const long long area, const int unit) const;

  virtual int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual int minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual void decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const = 0;
#endif

  virtual void setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension) = 0;
  virtual void setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension, const int membership) = 0;
  virtual void sortTubes() = 0;

  virtual void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const = 0;
  virtual void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const = 0;
  virtual void sumOnAddedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const = 0;
  virtual void minusSumOnRemovedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const = 0;
  virtual void sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const = 0; // only follow element; *dimensionIt is the next dimension
  virtual void minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const = 0; // only follow element; *dimensionIt is the next dimension

#if defined ASSERT || !defined PRUNE
  virtual int sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const = 0;
  virtual long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sums) const = 0;
  virtual void increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const = 0;

  long long sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit) const;
  long long sumsOnGraphPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit, const unsigned int vertexDimension) const;
#endif

 protected:
#ifdef PRUNE
  static void reset(const vector<unsigned int>::const_iterator firstCandidateElementBegin, vector<vector<int>>& sums);
#endif

#if defined ASSERT || !defined PRUNE
  static void reset(vector<vector<int>>& sums);
#endif

 private:
#ifdef PRUNE
  static vector<int> backupAndResetSums(vector<int>::iterator sumIt, vector<int>::iterator sumEnd);
  static vector<vector<int>> backupAndResetSumsAfterFirstCandidateElements(vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sums);
  static vector<vector<int>> backupAndResetSumsAfterFirstCandidateElements(const unsigned int increasedDimensionId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sums);
  static void scaleShiftAndAddBackup(const int unit, const int shift, vector<int>::const_iterator backupIt, vector<int>::iterator sumIt, const vector<int>::const_iterator sumEnd);
  static void scaleShiftAndAddBackupAfterFirstCandidateElements(const unsigned int increasedDimensionId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>::const_iterator backupIt, const int unit, const long long defaultNSetMembership, vector<vector<unsigned int>>::const_iterator dimensionIt, vector<vector<int>>& sums);
  static void scaleShiftAndAddBackupAfterFirstCandidateElements(const unsigned int increasedDimensionId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>::const_iterator backupIt, const int unit, const unsigned int vertexDimensionId, const long long defaultNSetMembership, vector<vector<unsigned int>>::const_iterator dimensionIt, vector<vector<int>>& sums);
  static void scaleShiftAndAddBackupAfterFirstCandidateElements(const unsigned int vertexDimensionId, const unsigned int addedVertexId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>::const_iterator backupIt, const int unit, const long long defaultNSetMembership, vector<vector<unsigned int>>::const_iterator dimensionIt, vector<vector<int>>& sums);
  static void scaleAndShift(const int unit, const int shift, vector<int>::iterator nbOfPresentTuplesIt, const vector<int>::iterator nbOfPresentTuplesEnd);
  static void increaseSumsOnVerticesWithShift(const vector<int>& increases, const unsigned int firstCandidateVertex, const int unit, const long long area, vector<int>& sums, const vector<unsigned int>& presentVertices);
  static void scaleShiftForVerticesAndAddBackup(const int unit, const long long defaultNSetMembership, const unsigned int firstCandidateElement, vector<int>::const_iterator backupIt, const vector<int>::iterator sumBegin, const vector<int>::const_iterator sumEnd, const vector<unsigned int>& presentVertices);

#else

  static vector<vector<int>> increasesForAddingElement(const unsigned int increasedDimensionId, const vector<vector<int>>& sums);
  static void increaseSumsOnVerticesWithShift(const vector<int>& increases, const int unit, const long long area, vector<int>& sums, const vector<unsigned int>& presentVertices);
  static void increaseWithShift(const vector<int>& increases, const int unit, const int shift, vector<int>& sums);
#endif

#if defined ASSERT || !defined PRUNE
  static void scaleAndShift(const int unit, const int shift, vector<int>& nbOfPresentTuples);
#endif
};

#endif /*ABSTRACT_TRIE_H_*/
