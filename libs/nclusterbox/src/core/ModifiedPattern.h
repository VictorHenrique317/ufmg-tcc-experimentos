// Copyright 2022-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MODIFIED_PATTERN_H_
#define MODIFIED_PATTERN_H_

#include <unordered_set>
#include <mutex>
#include <boost/container_hash/hash.hpp>

#include "AbstractRoughTensor.h"

enum NextStep { insert, erase, stop };

class ModifiedPattern
{
public:
  ModifiedPattern();
  virtual ~ModifiedPattern();

  static void modify();
  static void grow();

  static void setContext(const AbstractRoughTensor* roughTensor, const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelection);
#ifdef NB_OF_PATTERNS
  static void setNoOutputPattern();
#endif
  static unsigned int getNbOfOutputPatterns();
  static void insertCandidateVariables();

protected:
  vector<vector<unsigned int>> nSet;
  unsigned long long area;
  long long membershipSum;
  vector<vector<int>> sumsOnHyperplanes;
  vector<unsigned int> firstCandidateElements; // ids of the first absent element that, added, could increase g

  NextStep nextStep;
  double bestG;
  vector<vector<unsigned int>>::iterator bestDimensionIt;
  vector<int>::const_iterator bestSumIt;

  static const AbstractRoughTensor* roughTensor;
  static bool isSomeVisitedPatternStored;
  static VisitedPatterns* visitedPatterns;
  static AbstractTrie* tensor;

  virtual void computeAllSums();
  virtual void init();
  virtual bool insertOrErase();

#ifdef PRUNE
  static vector<vector<int>> elementPositiveMemberships;

  void considerDimensionForNextModificationStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const unsigned int cardinalityMinusOneOrTwo, const unsigned int firstCandidateElement); // cardinalityMinusOneOrTwo is dimensionIt->size() - 2 if *dimensionIt is a vertex dimension; dimensionIt->size() - 1 otherwise
  void considerDimensionForNextGrowingStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const vector<unsigned int>& firstNonInitialAndSubsequentInitial, const unsigned int cardinalityMinusOneOrTwo, const unsigned int firstCandidateElement); // cardinalityMinusOneOrTwo is dimensionIt->size() - 2 if *dimensionIt is a vertex dimension; dimensionIt->size() - 1 otherwise
#ifdef UPDATE_SUMS
  void increaseFirstCandidates();
  void considerInsertingElementsBeforeFirstCandidate(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>::const_iterator elementPositiveMembershipsInDimensionBegin, const vector<int>::const_iterator sumsInDimensionBegin, unsigned int& firstCandidateElement);
  void considerInsertingElementsBeforeFirstCandidates();
#endif
#else
  void considerDimensionForNextModificationStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const unsigned int cardinalityMinusOneOrTwo); // cardinalityMinusOneOrTwo is dimensionIt->size() - 2 if *dimensionIt is a vertex dimension; dimensionIt->size() - 1 otherwise
  void considerDimensionForNextGrowingStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const vector<unsigned int>& firstNonInitialAndSubsequentInitial, const unsigned int cardinalityMinusOneOrTwo); // cardinalityMinusOneOrTwo is dimensionIt->size() - 2 if *dimensionIt is a vertex dimension; dimensionIt->size() - 1 otherwise
#endif
  bool doModifyingStep(); // returns whether to go on
  bool insertAndReturnIfVisited(const unsigned int element); // element in *bestDimensionIt
  bool eraseAndReturnIfVisited(const unsigned int element); // element in *bestDimensionIt
  void localMaximumFound();

  static vector<unsigned int> firstNonInitialAndSubsequentInitialIn(const vector<unsigned int>& dimension);
  static int minInitialSum(const vector<int>& sumsInDimension, const vector<unsigned int>& firstNonInitialAndSubsequentInitial);

#ifdef ASSERT
  void assertAreaAndSums();
#endif

private:
  static unordered_set<vector<vector<unsigned int>>, boost::hash<vector<vector<unsigned int>>>> candidateVariables;
  static unsigned int nbOfOutputPatterns;
  static mutex candidateVariablesLock;

  void doModify();
  void doGrow();
  bool doGrowingStep(const vector<vector<unsigned int>>& firstNonInitialAndSubsequentInitial); // returns whether to go on

#ifdef PRUNE
  void computeFirstCandidateElements();
#endif
};

#endif	/* MODIFIED_PATTERN_H_ */
