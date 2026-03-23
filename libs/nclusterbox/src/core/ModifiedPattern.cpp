// Copyright 2022-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "ModifiedPattern.h"

#include "ConcurrentPatternPool.h"
#include "LastTrie.h"

#include <algorithm>

#if defined DEBUG_MODIFY || defined ASSERT
#include <iostream>
#endif

const AbstractRoughTensor* ModifiedPattern::roughTensor;
bool ModifiedPattern::isSomeVisitedPatternStored;
VisitedPatterns* ModifiedPattern::visitedPatterns;
AbstractTrie* ModifiedPattern::tensor;

#ifdef PRUNE
vector<vector<int>> ModifiedPattern::elementPositiveMemberships;
#endif
unordered_set<vector<vector<unsigned int>>, boost::hash<vector<vector<unsigned int>>>> ModifiedPattern::candidateVariables;
unsigned int ModifiedPattern::nbOfOutputPatterns;
mutex ModifiedPattern::candidateVariablesLock;

ModifiedPattern::ModifiedPattern(): nSet(), area(), membershipSum(), sumsOnHyperplanes(AbstractRoughTensor::nullSumsOnHyperplanes()), firstCandidateElements(), nextStep(), bestG(), bestDimensionIt(), bestSumIt()
{
  firstCandidateElements.reserve(sumsOnHyperplanes.size());
#ifndef PRUNE
  firstCandidateElements.resize(sumsOnHyperplanes.size());
#endif
}

ModifiedPattern::~ModifiedPattern()
{
}

void ModifiedPattern::modify()
{
  ModifiedPattern().doModify();
}

void ModifiedPattern::grow()
{
  ModifiedPattern().doGrow();
}

void ModifiedPattern::doModify()
{
  for (nSet = ConcurrentPatternPool::next(); !nSet.empty(); nSet = ConcurrentPatternPool::next())
    {
      init();
      if (!(isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG)))
	{
	  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
	  do
	    {
	      // Decide modification step
	      nextStep = stop;
	      vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
	      vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin();
#ifdef PRUNE
	      vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElements.begin();
	      considerDimensionForNextModificationStep(dimensionIt, *sumsInDimensionIt, dimensionIt->size() - 1, *firstCandidateElementIt);
#else
	      considerDimensionForNextModificationStep(dimensionIt, *sumsInDimensionIt, dimensionIt->size() - 1);
#endif
	      ++dimensionIt;
	      do
		{
#ifdef PRUNE
		  considerDimensionForNextModificationStep(dimensionIt, *++sumsInDimensionIt, dimensionIt->size() - 1, *++firstCandidateElementIt);
#else
		  considerDimensionForNextModificationStep(dimensionIt, *++sumsInDimensionIt, dimensionIt->size() - 1);
#endif
		}
	      while (++dimensionIt != dimensionEnd);
#if defined UPDATE_SUMS && defined PRUNE
	      considerInsertingElementsBeforeFirstCandidates();
#endif
	    }
	  while (doModifyingStep());
	}
    }
}

void ModifiedPattern::doGrow()
{
  for (nSet = ConcurrentPatternPool::next(); !nSet.empty(); nSet = ConcurrentPatternPool::next())
    {
      init();
      // assuming no input pattern is a subpattern of another input pattern; if not, the commented code below is useful to never reconsider several times the superpattern (and have it output several times, if AbstractRoughTensor::isDirectOutput())
//       if (isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG))
// 	{
// 	  continue;
// 	}
      const vector<vector<unsigned int>>::iterator dimensionEnd = nSet.end();
      vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin();
      vector<vector<unsigned int>> firstNonInitialAndSubsequentInitial;
      firstNonInitialAndSubsequentInitial.reserve(dimensionEnd - dimensionIt);
      firstNonInitialAndSubsequentInitial.emplace_back(firstNonInitialAndSubsequentInitialIn(*dimensionIt));
      ++dimensionIt;
      do
	{
	  firstNonInitialAndSubsequentInitial.emplace_back(firstNonInitialAndSubsequentInitialIn(*dimensionIt));
	}
      while (++dimensionIt != dimensionEnd);
      do
	{
	  // Decide growing step
	  nextStep = stop;
	  dimensionIt = nSet.begin();
	  vector<vector<unsigned int>>::const_iterator firstNonInitialAndSubsequentInitialIt = firstNonInitialAndSubsequentInitial.begin();
	  vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
#ifdef PRUNE
	  vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElements.begin();
	  considerDimensionForNextGrowingStep(dimensionIt, *sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1, *firstCandidateElementIt);
#else
	  considerDimensionForNextGrowingStep(dimensionIt, *sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1);
#endif
	  ++dimensionIt;
	  do
	    {
#ifdef PRUNE
	      considerDimensionForNextGrowingStep(dimensionIt, *++sumsInDimensionIt, *++firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1, *++firstCandidateElementIt);
#else
	      considerDimensionForNextGrowingStep(dimensionIt, *++sumsInDimensionIt, *++firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1);
#endif
	    }
	  while (++dimensionIt != dimensionEnd);
#if defined UPDATE_SUMS && defined PRUNE
	  considerInsertingElementsBeforeFirstCandidates();
#endif
	}
      while (doGrowingStep(firstNonInitialAndSubsequentInitial));
    }
}

unsigned int ModifiedPattern::getNbOfOutputPatterns()
{
  return nbOfOutputPatterns;
}

void ModifiedPattern::insertCandidateVariables()
{
  delete tensor;
  if (isSomeVisitedPatternStored)
    {
      visitedPatterns->clear();
    }
#ifdef PRUNE
  elementPositiveMemberships.clear();
  elementPositiveMemberships.shrink_to_fit();
#endif
  if (!AbstractRoughTensor::isDirectOutput())
    {
      move(candidateVariables.begin(), candidateVariables.end(), back_inserter<vector<vector<vector<unsigned int>>>>(AbstractRoughTensor::candidateVariables));
    }
  candidateVariables.clear();
}

#ifdef PRUNE
void ModifiedPattern::computeFirstCandidateElements()
{
  firstCandidateElements.clear();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  vector<vector<int>>::const_iterator elementPositiveMembershipsIt = elementPositiveMemberships.begin();
  // In case of growing, element dimensionIt->front() may be non-removable, the search could go beyond that index to compute/check fewer sums... and essentially all the data class functions would have to be rewritten to add in arguments a vector of iterators to the first present elements greater or equal to the first candidate in each dimension of nSet
  firstCandidateElements.push_back(upper_bound(elementPositiveMembershipsIt->begin(), elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin());
  ++dimensionIt;
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  do
    {
      ++elementPositiveMembershipsIt;
      firstCandidateElements.push_back(upper_bound(elementPositiveMembershipsIt->begin(), elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin());
    }
  while (++dimensionIt != dimensionEnd);
}

#ifdef UPDATE_SUMS
void ModifiedPattern::increaseFirstCandidates()
{
  vector<unsigned int>::iterator firstCandidateElementIt = firstCandidateElements.begin();
  vector<vector<int>>::const_iterator elementPositiveMembershipsIt = elementPositiveMemberships.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  // No increase of the first candidate to enter *bestDimensionIt since the related sums will not be updated
  for (; dimensionIt != bestDimensionIt; ++dimensionIt)
    {
      *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
      ++firstCandidateElementIt;
      ++elementPositiveMembershipsIt;
    }
  for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
    {
      ++firstCandidateElementIt;
      ++elementPositiveMembershipsIt;
      *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
    }
}

void ModifiedPattern::considerInsertingElementsBeforeFirstCandidate(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>::const_iterator elementPositiveMembershipsInDimensionBegin, const vector<int>::const_iterator sumsInDimensionBegin, unsigned int& firstCandidateElement)
{
  const unsigned int decreasedFirstCandidateElement = upper_bound(elementPositiveMembershipsInDimensionBegin, elementPositiveMembershipsInDimensionBegin + firstCandidateElement, sqrt(bestG * (area / dimensionIt->size()) * (dimensionIt->size() + 1)) - membershipSum) - elementPositiveMembershipsInDimensionBegin;
  if (decreasedFirstCandidateElement != firstCandidateElement)
    {
      if (LastTrie::is01)
	{
	  tensor->sumsOnNewCandidateHyperplanes(nSet, dimensionIt, decreasedFirstCandidateElement, firstCandidateElement, sumsOnHyperplanes[dimensionIt - nSet.begin()], area, AbstractRoughTensor::getUnit());
	}
      else
	{
	  tensor->sumsOnNewCandidateHyperplanes(nSet, dimensionIt, decreasedFirstCandidateElement, firstCandidateElement, sumsOnHyperplanes[dimensionIt - nSet.begin()]);
	}
      vector<int>::const_iterator bestIncreasingSumInDimensionIt = max_element(sumsInDimensionBegin + decreasedFirstCandidateElement, sumsInDimensionBegin + firstCandidateElement, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
      firstCandidateElement = decreasedFirstCandidateElement;
      double g = membershipSum + *bestIncreasingSumInDimensionIt;
      g *= abs(g) / area * dimensionIt->size() / (dimensionIt->size() + 1);
      if (g > bestG)
	{
	  bestG = g;
	  bestDimensionIt = dimensionIt;
	  bestSumIt = bestIncreasingSumInDimensionIt;
	  nextStep = insert;
	}
    }
}

void ModifiedPattern::considerInsertingElementsBeforeFirstCandidates()
{
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin();
  vector<unsigned int>::iterator firstCandidateElementIt = firstCandidateElements.begin();
  vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
  vector<vector<int>>::const_iterator elementPositiveMembershipsIt = elementPositiveMemberships.begin();
  considerInsertingElementsBeforeFirstCandidate(dimensionIt, elementPositiveMembershipsIt->begin(), sumsInDimensionIt->begin(), *firstCandidateElementIt);
  ++dimensionIt;
  do
    {
      considerInsertingElementsBeforeFirstCandidate(dimensionIt, (++elementPositiveMembershipsIt)->begin(), (++sumsInDimensionIt)->begin(), *++firstCandidateElementIt);
    }
  while (++dimensionIt != dimensionEnd);
}
#endif
#endif

void ModifiedPattern::computeAllSums()
{
#ifdef PRUNE
  if (LastTrie::is01)
    {
      membershipSum = tensor->sumOnPattern(nSet.begin(), area, AbstractRoughTensor::getUnit());
      computeFirstCandidateElements();
      tensor->sumsOnHyperplanes(nSet.begin(), firstCandidateElements.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
      return;
    }
  membershipSum = tensor->sumOnPattern(nSet.begin());
  computeFirstCandidateElements();
  tensor->sumsOnHyperplanes(nSet.begin(), firstCandidateElements.begin(), sumsOnHyperplanes);
#else
  if (LastTrie::is01)
    {
      membershipSum = tensor->sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
      return;
    }
  membershipSum = tensor->sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes);
#endif
}

void ModifiedPattern::init()
{
  area = AbstractRoughTensor::patternArea(nSet);
  ModifiedPattern::computeAllSums();
  bestG = abs(static_cast<double>(membershipSum)) * membershipSum / area;
#ifdef DEBUG_MODIFY
  cout << "* ";
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
  cout << " gives g = " << bestG / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << '\n';
#endif
#ifdef ASSERT
  assertAreaAndSums();
#endif
}

#ifdef PRUNE
void ModifiedPattern::considerDimensionForNextModificationStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const unsigned int cardinalityMinusOneOrTwo, const unsigned int firstCandidateElement)
#else
void ModifiedPattern::considerDimensionForNextModificationStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const unsigned int cardinalityMinusOneOrTwo)
#endif
{
  if (sumsInDimension.size() == dimensionIt->size())
    {
      // Every element in the dimension of the tensor is present
      if (cardinalityMinusOneOrTwo)
	{
	  // Any element can be erased from *dimensionIt
	  const vector<int>::const_iterator bestDecreasingSumInDimensionIt = min_element(sumsInDimension.begin(), sumsInDimension.end()); // in case of equality, prefer removing the globally sparsest slice
	  double g = membershipSum - *bestDecreasingSumInDimensionIt;
	  g *= abs(g) / area * dimensionIt->size() / cardinalityMinusOneOrTwo;
	  if (g > bestG)
	    {
	      bestG = g;
	      bestDimensionIt = dimensionIt;
	      bestSumIt = bestDecreasingSumInDimensionIt;
	      nextStep = erase;
	    }
	}
      return;
    }
  // Some element absent from *dimensionIt can be added
  vector<int>::const_iterator bestIncreasingSumInDimensionIt;
  vector<int>::const_iterator sumIt;
  if (cardinalityMinusOneOrTwo)
    {
      // Some element can be erased from *dimensionIt
      vector<int>::const_iterator bestDecreasingSumInDimensionIt;
      const vector<int>::const_iterator sumBegin = sumsInDimension.begin();
      vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
#ifdef PRUNE
      if (*presentElementIdIt != firstCandidateElement)
	{
	  // Initializing bestDecreasingSumInDimensionIt with the sum relating to the first present element; bestIncreasingSumInDimensionIt with the greatest sum before
	  sumIt = sumBegin + *presentElementIdIt++;
	  bestIncreasingSumInDimensionIt = max_element(sumBegin + firstCandidateElement, sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	  bestDecreasingSumInDimensionIt = sumIt++;
	}
#else
      if (*presentElementIdIt)
	{
	  // Initializing bestDecreasingSumInDimensionIt with the sum relating to the first present element; bestIncreasingSumInDimensionIt with the greatest sum before
	  sumIt = sumBegin + *presentElementIdIt++;
	  bestIncreasingSumInDimensionIt = max_element(sumBegin, sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	  bestDecreasingSumInDimensionIt = sumIt++;
	}
#endif
      else
	{
	  // Initializing bestIncreasingSumInDimensionIt with the sum relating to the first absent candidate element; bestDecreasingSumInDimensionIt with the lowest sum before
	  bestDecreasingSumInDimensionIt = sumBegin;
#ifdef PRUNE
	  bestDecreasingSumInDimensionIt += firstCandidateElement;
	  unsigned int elementId = firstCandidateElement + 1;
#else
	  unsigned int elementId = 1;
#endif
	  sumIt = bestDecreasingSumInDimensionIt;
	  for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); ++presentElementIdIt != presentElementIdEnd && *presentElementIdIt == elementId; ++elementId)
	    {
	      if (*++sumIt < *bestDecreasingSumInDimensionIt) // in case of equality, prefer removing the globally sparsest slice
		{
		  bestDecreasingSumInDimensionIt = sumIt;
		}
	    }
	  bestIncreasingSumInDimensionIt = ++sumIt++;
	}
      // Compute bestDecreasingSumInDimensionIt and bestIncreasingSumInDimensionIt considering the sums until the one relating to the last present element
      for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); presentElementIdIt != presentElementIdEnd; ++presentElementIdIt)
	{
	  for (const vector<int>::const_iterator end = sumBegin + *presentElementIdIt; sumIt != end; ++sumIt)
	    {
	      if (*sumIt >= *bestIncreasingSumInDimensionIt) // in case of equality, prefer adding the globally densest slice
		{
		  bestIncreasingSumInDimensionIt = sumIt;
		}
	    }
	  if (*sumIt < *bestDecreasingSumInDimensionIt) // in case of equality, prefer removing the globally sparsest slice
	    {
	      bestDecreasingSumInDimensionIt = sumIt;
	    }
	  ++sumIt;
	}
      double g = membershipSum - *bestDecreasingSumInDimensionIt;
      g *= abs(g) / area * dimensionIt->size() / cardinalityMinusOneOrTwo;
      if (g > bestG)
	{
	  bestG = g;
	  bestDimensionIt = dimensionIt;
	  bestSumIt = bestDecreasingSumInDimensionIt;
	  nextStep = erase;
	}
    }
  else
    {
      // No element can be erased from *dimensionIt
      if (dimensionIt->size() == 1)
	{
#ifdef PRUNE
	  if (dimensionIt->front() != firstCandidateElement)
	    {
	      sumIt = sumsInDimension.begin() + dimensionIt->front();
	      bestIncreasingSumInDimensionIt = max_element(sumsInDimension.begin() + firstCandidateElement, sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	      ++sumIt;
	    }
	  else
	    {
	      sumIt = ++sumsInDimension.begin() + firstCandidateElement;
	      bestIncreasingSumInDimensionIt = sumIt++;
	    }
#else
	  if (dimensionIt->front())
	    {
	      sumIt = sumsInDimension.begin() + dimensionIt->front();
	      bestIncreasingSumInDimensionIt = max_element(sumsInDimension.begin(), sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	      ++sumIt;
	    }
	  else
	    {
	      sumIt = ++sumsInDimension.begin();
	      bestIncreasingSumInDimensionIt = sumIt++;
	    }
#endif
	}
      else
	{
	   // *dimensionIt contains two vertices
#ifdef PRUNE
	  if (dimensionIt->front() != firstCandidateElement)
	    {
	      sumIt = sumsInDimension.begin() + dimensionIt->front();
	      bestIncreasingSumInDimensionIt = max_element(sumsInDimension.begin() + firstCandidateElement, sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	      ++sumIt;
	    }
	  else
	    {
	      sumIt = ++sumsInDimension.begin() + firstCandidateElement;
	      if ((*dimensionIt)[1] == firstCandidateElement + 1)
		{
		  // the two vertices are firstCandidateElement and firstCandidateElement + 1 and sumIt points to the sum on the second vertex
		  bestIncreasingSumInDimensionIt = sumIt + 1;
		}
	      else
		{
		  bestIncreasingSumInDimensionIt = sumIt++;
		}
	    }
#else
	  if (dimensionIt->front())
	    {
	      sumIt = sumsInDimension.begin() + dimensionIt->front();
	      bestIncreasingSumInDimensionIt = max_element(sumsInDimension.begin(), sumIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	      ++sumIt;
	    }
	  else
	    {
	      sumIt = ++sumsInDimension.begin();
	      if ((*dimensionIt)[1] == 1)
		{
		  // the two vertices are 0 and 1 and sumIt points to the sum on the second vertex
		  bestIncreasingSumInDimensionIt = sumIt + 1;
		}
	      else
		{
		  bestIncreasingSumInDimensionIt = sumIt++;
		}
	    }
#endif
	  // Elements between the two present vertices
	  for (const vector<int>::const_iterator secondVertexSumIt = sumsInDimension.begin() + (*dimensionIt)[1]; sumIt != secondVertexSumIt; ++sumIt)
	    {
	      if (*sumIt >= *bestIncreasingSumInDimensionIt) // in case of equality, prefer adding the globally densest slice
		{
		  bestIncreasingSumInDimensionIt = sumIt;
		}
	    }
	  ++sumIt;
	}
    }
  // Elements after the last present one
  for (const vector<int>::const_iterator sumEnd = sumsInDimension.end(); sumIt != sumEnd; ++sumIt)
    {
      if (*sumIt >= *bestIncreasingSumInDimensionIt) // in case of equality, prefer adding the globally densest slice
	{
	  bestIncreasingSumInDimensionIt = sumIt;
	}
    }
  double g = membershipSum + *bestIncreasingSumInDimensionIt;
  g *= abs(g) / area * (cardinalityMinusOneOrTwo + 1) / (dimensionIt->size() + 1);
  if (g > bestG)
    {
      bestG = g;
      bestDimensionIt = dimensionIt;
      bestSumIt = bestIncreasingSumInDimensionIt;
      nextStep = insert;
    }
}

#ifdef PRUNE
void ModifiedPattern::considerDimensionForNextGrowingStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const vector<unsigned int>& firstNonInitialAndSubsequentInitial, const unsigned int cardinalityMinusOneOrTwo, const unsigned int firstCandidateElement)
#else
void ModifiedPattern::considerDimensionForNextGrowingStep(const vector<vector<unsigned int>>::iterator dimensionIt, const vector<int>& sumsInDimension, const vector<unsigned int>& firstNonInitialAndSubsequentInitial, const unsigned int cardinalityMinusOneOrTwo)
#endif
{
  vector<unsigned int>::const_iterator presentElementIdIt;
  vector<unsigned int>::const_iterator initialElementIt = firstNonInitialAndSubsequentInitial.begin(); // despite the name, *initialElementIt is the first *non*-initial element
#ifdef PRUNE
  unsigned int elementId;
  if (*initialElementIt)
    {
      elementId = *initialElementIt++;
      presentElementIdIt = dimensionIt->begin() + elementId;
    }
  else
    {
      elementId = firstCandidateElement;
      for (const vector<unsigned int>::const_iterator end = firstNonInitialAndSubsequentInitial.end(); ++initialElementIt != end && *initialElementIt == elementId; ++elementId)
	{
	}
      presentElementIdIt = lower_bound(dimensionIt->begin(), dimensionIt->end(), elementId);
    }
#else
  unsigned int elementId = *initialElementIt++;
  presentElementIdIt = dimensionIt->begin() + elementId;
#endif
  vector<int>::const_iterator sumIt = sumsInDimension.begin() + elementId;
  if (sumIt != sumsInDimension.end())
    {
      // Some element can be added or removed
      int bestIncreasingSum;
      vector<int>::const_iterator bestIncreasingSumInDimensionIt;
      if (presentElementIdIt == dimensionIt->end())
	{
	  // No element can be removed
	  bestIncreasingSumInDimensionIt = sumIt;
	  bestIncreasingSum = *sumIt;
	}
      else
	{
	  int bestDecreasingSum;
	  vector<int>::const_iterator bestDecreasingSumInDimensionIt;
	  if (*presentElementIdIt == elementId)
	    {
	      // First non initial element is present
	      ++presentElementIdIt;
	      bestDecreasingSumInDimensionIt = sumIt;
	      bestDecreasingSum = *sumIt;
	      bestIncreasingSumInDimensionIt = sumsInDimension.end();
	      bestIncreasingSum = numeric_limits<int>::min();
	    }
	  else
	    {
	      // First non initial element is absent
	      bestDecreasingSumInDimensionIt = sumsInDimension.end();
	      bestDecreasingSum = numeric_limits<int>::max();
	      bestIncreasingSumInDimensionIt = sumIt;
	      bestIncreasingSum = *sumIt;
	    }
	  // Elements until the last last initial one
	  vector<unsigned int>::const_iterator end = firstNonInitialAndSubsequentInitial.end();
	  for (; initialElementIt != end; ++initialElementIt)
	    {
	      while (++elementId != *initialElementIt)
		{
		  if (elementId == *presentElementIdIt)
		    {
		      ++presentElementIdIt;
		      if (*++sumIt < bestDecreasingSum) // in case of equality, prefer removing the globally sparsest slice
			{
			  bestDecreasingSum = *sumIt;
			  bestDecreasingSumInDimensionIt = sumIt;
			}
		      continue;
		    }
		  if (*++sumIt >= bestIncreasingSum) // in case of equality, prefer adding the globally densest slice
		    {
		      bestIncreasingSum = *sumIt;
		      bestIncreasingSumInDimensionIt = sumIt;
		    }
		}
	      ++sumIt;
	      ++presentElementIdIt;
	    }
	  // Elements after the last initial one
	  for (end = dimensionIt->end(); presentElementIdIt != end; ++presentElementIdIt)
	    {
	      while (++elementId != *presentElementIdIt)
		{
		  if (*++sumIt >= bestIncreasingSum) // in case of equality, prefer adding the globally densest slice
		    {
		      bestIncreasingSum = *sumIt;
		      bestIncreasingSumInDimensionIt = sumIt;
		    }
		}
	      if (*++sumIt < bestDecreasingSum) // in case of equality, prefer removing the globally sparsest slice
		{
		  bestDecreasingSum = *sumIt;
		  bestDecreasingSumInDimensionIt = sumIt;
		}
	    }
	  if (bestDecreasingSum != numeric_limits<int>::max())
	    {
	      double g = membershipSum - bestDecreasingSum;
	      g *= abs(g) / area * dimensionIt->size() / cardinalityMinusOneOrTwo;
	      if (g > bestG)
		{
		  bestG = g;
		  bestDimensionIt = dimensionIt;
		  bestSumIt = bestDecreasingSumInDimensionIt;
		  nextStep = erase;
		}
	    }
	}
      // Elements after the last present one
      for (const vector<int>::const_iterator sumEnd = sumsInDimension.end(); ++sumIt != sumEnd; )
	{
	  if (*sumIt >= bestIncreasingSum) // in case of equality, prefer adding the globally densest slice
	    {
	      bestIncreasingSum = *sumIt;
	      bestIncreasingSumInDimensionIt = sumIt;
	    }
	}
      if (bestIncreasingSum != numeric_limits<int>::min())
	{
	  double g = membershipSum + bestIncreasingSum;
	  g *= abs(g) / area * (cardinalityMinusOneOrTwo + 1) / (dimensionIt->size() + 1);
	  if (g > bestG)
	    {
	      bestG = g;
	      bestDimensionIt = dimensionIt;
	      bestSumIt = bestIncreasingSumInDimensionIt;
	      nextStep = insert;
	    }
	}
    }
}

bool ModifiedPattern::doModifyingStep()
{
#ifdef ASSERT
  assertAreaAndSums();
#endif
  if (nextStep != stop)
    {
      return insertOrErase();
    }
#ifdef DEBUG_MODIFY
  cout << "    ";
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
#endif
  localMaximumFound();
  return false;
}

bool ModifiedPattern::doGrowingStep(const vector<vector<unsigned int>>& firstNonInitialAndSubsequentInitial)
{
#ifdef ASSERT
  assertAreaAndSums();
#endif
  if (nextStep != stop)
    {
      return insertOrErase();
    }
#ifdef DEBUG_MODIFY
  cout << "    ";
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
#endif
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  vector<vector<unsigned int>>::const_iterator firstNonInitialAndSubsequentInitialIt = firstNonInitialAndSubsequentInitial.begin();
  vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
  double g = membershipSum - minInitialSum(*sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt);
  if (g * abs(g) / (area / dimensionIt->size() * (dimensionIt->size() - 1)) > bestG)
    {
#ifdef DEBUG_MODIFY
      cout << " does not locally maximize g, because of initial element: discard\n";
#endif
      return false;
    }
  ++dimensionIt;
  do
    {
      g = membershipSum - minInitialSum(*++sumsInDimensionIt, *++firstNonInitialAndSubsequentInitialIt);
      if (g * abs(g) / (area / dimensionIt->size() * (dimensionIt->size() - 1)) > bestG)
	{
#ifdef DEBUG_MODIFY
	  cout << " does not locally maximize g, because of initial element: discard\n";
#endif
	  return false;
	}
    }
  while (++dimensionIt != dimensionEnd);
  localMaximumFound();
  return false;
}

bool ModifiedPattern::insertAndReturnIfVisited(const unsigned int element)
{
#ifdef DEBUG_MODIFY
  cout << "    Adding slice for ";
  roughTensor->printElement(bestDimensionIt - nSet.begin(), element, cout);
  cout << " gives ";
#endif
  bestDimensionIt->insert(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), element), element);
  if (isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG))
    {
#ifdef DEBUG_MODIFY
      roughTensor->printPattern(nSet, static_cast<float>(membershipSum + *bestSumIt) / roughTensor->area(nSet), cout);
      cout << ", which has already been reached: abort\n";
#endif
      return true;
    }
  return false;
}

bool ModifiedPattern::eraseAndReturnIfVisited(const unsigned int element)
{
#ifdef DEBUG_MODIFY
  cout << "    Removing slice for ";
  roughTensor->printElement(bestDimensionIt - nSet.begin(), element, cout);
  cout << " gives ";
#endif
  bestDimensionIt->erase(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), element));
  if (isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG))
    {
#ifdef DEBUG_MODIFY
      roughTensor->printPattern(nSet, static_cast<float>(membershipSum - *bestSumIt) / roughTensor->area(nSet), cout);
      cout << ", which has already been reached: abort\n";
#endif
      return true;
    }
  return false;
}

bool ModifiedPattern::insertOrErase()
{
  area /= bestDimensionIt->size();
  const unsigned int element = static_cast<unsigned int>(bestSumIt - sumsOnHyperplanes[bestDimensionIt - nSet.begin()].begin());
  if (nextStep == insert)
    {
      if (insertAndReturnIfVisited(element))
	{
	  return false;
	}
#ifdef UPDATE_SUMS
      membershipSum += *bestSumIt;
#ifdef PRUNE
      increaseFirstCandidates();
#endif
      if (LastTrie::is01)
	{
#ifdef PRUNE
	  tensor->addElement(bestDimensionIt - nSet.begin(), element, nSet, firstCandidateElements, sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
#else
	  tensor->addElement(bestDimensionIt - nSet.begin(), element, nSet, sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
#endif
	}
      else
	{
#ifdef PRUNE
	  tensor->addElement(bestDimensionIt - nSet.begin(), element, nSet, firstCandidateElements, sumsOnHyperplanes);
#else
	  tensor->addElement(bestDimensionIt - nSet.begin(), element, nSet, sumsOnHyperplanes);
#endif
	}
#endif
    }
  else
    {
      if (eraseAndReturnIfVisited(element))
	{
	  return false;
	}
#ifdef UPDATE_SUMS
      membershipSum -= *bestSumIt;
#ifdef PRUNE
      increaseFirstCandidates();
#endif
      if (LastTrie::is01)
	{
#ifdef PRUNE
	  tensor->addElement(bestDimensionIt - nSet.begin(), element, nSet, firstCandidateElements, sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit()); // negating the last two arguments for a decrease
#else
	  tensor->addElement(bestDimensionIt - nSet.begin(), element, nSet, sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit()); // negating the last two arguments for a decrease
#endif
	}
      else
	{
#ifdef PRUNE
	  tensor->removeElement(bestDimensionIt - nSet.begin(), element, nSet, firstCandidateElements, sumsOnHyperplanes);
#else
	  tensor->removeElement(bestDimensionIt - nSet.begin(), element, nSet, sumsOnHyperplanes);
#endif
	}
#endif
    }
  area *= bestDimensionIt->size();
#ifndef UPDATE_SUMS
  computeAllSums();
#endif
#ifdef DEBUG_MODIFY
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
  cout << " and g = " << bestG / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << '\n';
#endif
  return true;
}

void ModifiedPattern::localMaximumFound()
{
#ifdef DEBUG_MODIFY
  cout << " locally maximizes g\n";
#endif
  if (AbstractRoughTensor::isDirectOutput())
    {
      candidateVariablesLock.lock();
      if (candidateVariables.insert(nSet).second)
	{
	  // Unlock candidateVariablesLock and lock another mutex, so that the discovery of (probably already-found) other local maxima can be tested while this local maximum is output
	  candidateVariablesLock.unlock();
	  static mutex outputLock; // using a local mutex takes a little more time, but there are only a few distinct candidate variables and having a static member variable just for that case (!isSomeVisitedPatternStored && AbstractRoughTensor::isDirectOutput()) looks exagerated
	  const lock_guard<mutex> lock(outputLock);
	  roughTensor->output(std::move(nSet), static_cast<float>(membershipSum) / area);
	  ++nbOfOutputPatterns;
	  return;
	}
      candidateVariablesLock.unlock();
      return;
    }
  const lock_guard<mutex> lock(candidateVariablesLock);
  candidateVariables.insert(std::move(nSet));
}

void ModifiedPattern::setContext(const AbstractRoughTensor* roughTensorParam, const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelection)
{
  nbOfOutputPatterns = 0;
  roughTensor = roughTensorParam;
  tensor = roughTensor->getTensor();
#ifdef PRUNE
  elementPositiveMemberships = AbstractRoughTensor::nullSumsOnHyperplanes();
  if (LastTrie::is01)
    {
      tensor->positiveMembershipsOnHyperplanes(elementPositiveMemberships, AbstractRoughTensor::getUnit());
    }
  else
    {
      tensor->positiveMembershipsOnHyperplanes(elementPositiveMemberships.begin());
    }
#endif
  visitedPatterns = roughTensor->getEmptyVisitedPatterns(availableBytes - nbOfJobs * sizeof(ModifiedPattern), nbOfJobs, isNoSelection);
  isSomeVisitedPatternStored = visitedPatterns != nullptr;
}

vector<unsigned int> ModifiedPattern::firstNonInitialAndSubsequentInitialIn(const vector<unsigned int>& dimension)
{
  const unsigned int sizeOfDimension = dimension.size();
  unsigned int elementId = 0;
  while (dimension[elementId] == elementId && ++elementId != sizeOfDimension)
    {
    }
  vector<unsigned int> firstNonInitialAndSubsequentInitial;
  firstNonInitialAndSubsequentInitial.reserve(sizeOfDimension - elementId + 1);
  firstNonInitialAndSubsequentInitial.push_back(elementId);
  firstNonInitialAndSubsequentInitial.insert(firstNonInitialAndSubsequentInitial.end(), dimension.begin() + elementId, dimension.end());
  return firstNonInitialAndSubsequentInitial;
}

int ModifiedPattern::minInitialSum(const vector<int>& sumsInDimension, const vector<unsigned int>& firstNonInitialAndSubsequentInitial)
{
  int bestDecreasingSum;
  vector<unsigned int>::const_iterator firstNonInitialAndSubsequentInitialIt = firstNonInitialAndSubsequentInitial.begin();
  if (*firstNonInitialAndSubsequentInitialIt)
    {
      bestDecreasingSum = *min_element(sumsInDimension.begin(), sumsInDimension.begin() + *firstNonInitialAndSubsequentInitialIt);
    }
  else
    {
      bestDecreasingSum = sumsInDimension[*++firstNonInitialAndSubsequentInitialIt];
    }
  for (const vector<unsigned int>::const_iterator initialEnd = firstNonInitialAndSubsequentInitial.end(); ++firstNonInitialAndSubsequentInitialIt != initialEnd; )
    {
      if (sumsInDimension[*firstNonInitialAndSubsequentInitialIt] < bestDecreasingSum)
	{
	  bestDecreasingSum = sumsInDimension[*firstNonInitialAndSubsequentInitialIt];
	}
    }
  return bestDecreasingSum;
}

#ifdef NB_OF_PATTERNS
void ModifiedPattern::setNoOutputPattern()
{
  nbOfOutputPatterns = 0;
}
#endif

#ifdef ASSERT
void ModifiedPattern::assertAreaAndSums()
{
  // Asserting area
  if (roughTensor->area(nSet) != area)
    {
      cerr << "area is " << roughTensor->area(nSet) << " and not " << area << ", as computed!\n";
    }
  vector<vector<int>> actualSumsOnHyperplanes = AbstractRoughTensor::nullSumsOnHyperplanes();
  {
    // Asserting membershipSum
    long long actualMembershipSum;
    if (LastTrie::is01)
      {
	if (ConcurrentPatternPool::vertexDimension == numeric_limits<unsigned int>::max())
	  {
	    actualMembershipSum = tensor->sumsOnPatternAndHyperplanes(nSet.begin(), actualSumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
	  }
	else
	  {
	    actualMembershipSum = tensor->sumsOnGraphPatternAndHyperplanes(nSet.begin(), actualSumsOnHyperplanes, area, AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension);
	  }
      }
    else
      {
	actualMembershipSum = tensor->sumsOnPatternAndHyperplanes(nSet.begin(), actualSumsOnHyperplanes);
      }
    if (actualMembershipSum != membershipSum)
      {
	cerr << "membership sum is " << static_cast<double>(actualMembershipSum) / AbstractRoughTensor::getUnit() << " and not " << static_cast<double>(membershipSum) / AbstractRoughTensor::getUnit() << ", as computed!\n";
      }
  }
  vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElements.begin();
  vector<vector<int>>::const_iterator actualSumsInDimensionIt = actualSumsOnHyperplanes.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  for (const vector<int>& sumsInDimension : sumsOnHyperplanes)
    {
      unsigned long long hypotheticalArea = area;
      if (dimensionIt - nSet.begin() == ConcurrentPatternPool::vertexDimension)
	{
	  hypotheticalArea *= 2;
	  hypotheticalArea /= dimensionIt->size() - 1;
	  hypotheticalArea *= dimensionIt->size() + 1;
	  hypotheticalArea /= 2;
	}
      else
	{
	  hypotheticalArea /= dimensionIt->size();
	  hypotheticalArea *= dimensionIt->size() + 1;
	}
      const double threshold = bestG * hypotheticalArea;
      // Asserting firstCandidateElements
      vector<int>::const_iterator actualSumIt = find_if(actualSumsInDimensionIt->begin(), actualSumsInDimensionIt->begin() + *firstCandidateElementIt, [this, threshold](const int sum) { const double hypotheticalMembershipSum = membershipSum + sum; return abs(hypotheticalMembershipSum) * hypotheticalMembershipSum > threshold; });
      for (; actualSumIt != actualSumsInDimensionIt->begin() + *firstCandidateElementIt; actualSumIt = find_if(++actualSumIt, actualSumsInDimensionIt->begin() + *firstCandidateElementIt, [this, threshold](const int sum) { const double hypotheticalMembershipSum = membershipSum + sum; return abs(hypotheticalMembershipSum) * hypotheticalMembershipSum > threshold; }))
	{
	  roughTensor->printElement(actualSumsInDimensionIt - actualSumsOnHyperplanes.begin(), actualSumIt - actualSumsInDimensionIt->begin(), cerr);
	  cerr << " considered irrelevant but adding its slice would lead to a pattern of area " << hypotheticalArea << " and shifted density " <<  static_cast<double>(membershipSum + *actualSumIt) / hypotheticalArea / AbstractRoughTensor::getUnit() << ", hence an increase of g to " << static_cast<double>(membershipSum + *actualSumIt) * (membershipSum + *actualSumIt) / hypotheticalArea / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << "!\n";
	}
      // Asserting sumsOnHyperplanes
      for (pair<vector<int>::const_iterator, vector<int>::const_iterator> mismatchingSumIts = mismatch(sumsInDimension.begin() + *firstCandidateElementIt, sumsInDimension.end(), actualSumIt); mismatchingSumIts.first != sumsInDimension.end(); mismatchingSumIts = mismatch(++mismatchingSumIts.first, sumsInDimension.end(), ++mismatchingSumIts.second))
	{
	  cerr << "sum on ";
	  roughTensor->printElement(actualSumsInDimensionIt - actualSumsOnHyperplanes.begin(), mismatchingSumIts.first - sumsInDimension.begin(), cerr);
	  cerr << " is " << static_cast<double>(*mismatchingSumIts.second) / AbstractRoughTensor::getUnit() << " and not " << static_cast<double>(*mismatchingSumIts.first) / AbstractRoughTensor::getUnit() << ", as computed!\n";
	}
      ++actualSumsInDimensionIt;
      ++firstCandidateElementIt;
      ++dimensionIt;
    }
}
#endif
