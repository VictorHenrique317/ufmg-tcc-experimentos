// Copyright 2024-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "ModifiedGraphPattern.h"

#include "AbstractRoughGraphTensor.h"
#include "ConcurrentPatternPool.h"
#include "LastTrie.h"

#ifdef PRUNE
#include <algorithm>
#endif

#ifdef DEBUG_MODIFY
#include <iostream>
#endif

ModifiedGraphPattern::ModifiedGraphPattern(): ModifiedPattern()
{
}

ModifiedGraphPattern::~ModifiedGraphPattern()
{
}

void ModifiedGraphPattern::modify()
{
  ModifiedGraphPattern().doModifyGraphPattern();
}

void ModifiedGraphPattern::grow()
{
  ModifiedGraphPattern().doGrowGraphPattern();
}

void ModifiedGraphPattern::doModifyGraphPattern()
{
  for (nSet = ConcurrentPatternPool::next(); !nSet.empty(); nSet = ConcurrentPatternPool::next())
    {
      init();
      if (!(isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG)))
	{
	  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
	  const vector<vector<unsigned int>>::iterator vertexDimensionIt = nSet.begin() + ConcurrentPatternPool::vertexDimension;
	  do
	    {
	      // Decide modification step
	      nextStep = stop;
	      vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
#ifdef PRUNE
	      vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElements.begin();
#endif
	      vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin();
	      for (; dimensionIt != vertexDimensionIt; ++dimensionIt)
		{
#ifdef PRUNE
		  considerDimensionForNextModificationStep(dimensionIt, *sumsInDimensionIt, dimensionIt->size() - 1, *firstCandidateElementIt);
		  ++firstCandidateElementIt;
#else
		  considerDimensionForNextModificationStep(dimensionIt, *sumsInDimensionIt, dimensionIt->size() - 1);
#endif
		  ++sumsInDimensionIt;
		}
#ifdef PRUNE
	      considerDimensionForNextModificationStep(vertexDimensionIt, *sumsInDimensionIt, vertexDimensionIt->size() - 2, *firstCandidateElementIt);
#else
	      considerDimensionForNextModificationStep(vertexDimensionIt, *sumsInDimensionIt, vertexDimensionIt->size() - 2);
#endif
	      while (++dimensionIt != dimensionEnd)
		{
#ifdef PRUNE
		  considerDimensionForNextModificationStep(dimensionIt, *++sumsInDimensionIt, dimensionIt->size() - 1, *++firstCandidateElementIt);
#else
		  considerDimensionForNextModificationStep(dimensionIt, *++sumsInDimensionIt, dimensionIt->size() - 1);
#endif
		}
#if defined UPDATE_SUMS && defined PRUNE
	      considerInsertingElementsBeforeFirstCandidatesInGraphPattern();
#endif
	    }
	  while (doModifyingStep());
	}
    }
}

void ModifiedGraphPattern::doGrowGraphPattern()
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
      do
	{
	  firstNonInitialAndSubsequentInitial.emplace_back(firstNonInitialAndSubsequentInitialIn(*dimensionIt));
	}
      while (++dimensionIt != dimensionEnd);
      const vector<vector<unsigned int>>::iterator vertexDimensionIt = nSet.begin() + ConcurrentPatternPool::vertexDimension;
      do
	{
	  // Decide growing step
	  nextStep = stop;
	  dimensionIt = nSet.begin();
	  vector<vector<unsigned int>>::const_iterator firstNonInitialAndSubsequentInitialIt = firstNonInitialAndSubsequentInitial.begin();
	  vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
#ifdef PRUNE
	  vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElements.begin();
#endif
	  for (; dimensionIt != vertexDimensionIt; ++dimensionIt)
	    {
#ifdef PRUNE
	      considerDimensionForNextGrowingStep(dimensionIt, *sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1, *firstCandidateElementIt);
	      ++firstCandidateElementIt;
#else
	      considerDimensionForNextGrowingStep(dimensionIt, *sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1);
#endif
	      ++sumsInDimensionIt;
	      ++firstNonInitialAndSubsequentInitialIt;
	    }
#ifdef PRUNE
	  considerDimensionForNextGrowingStep(dimensionIt, *sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 2, *firstCandidateElementIt);
#else
	  considerDimensionForNextGrowingStep(dimensionIt, *sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 2);
#endif
	  while (++dimensionIt != dimensionEnd)
	    {
#ifdef PRUNE
	      considerDimensionForNextGrowingStep(dimensionIt, *++sumsInDimensionIt, *++firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1, *++firstCandidateElementIt);
#else
	      considerDimensionForNextGrowingStep(dimensionIt, *++sumsInDimensionIt, *++firstNonInitialAndSubsequentInitialIt, dimensionIt->size() - 1);
#endif
	    }
#if defined UPDATE_SUMS && defined PRUNE
	  considerInsertingElementsBeforeFirstCandidatesInGraphPattern();
#endif
	}
      while (doGrowingStepForGraphPattern(firstNonInitialAndSubsequentInitial));
    }
}

#ifdef PRUNE
void ModifiedGraphPattern::computeFirstCandidateElements()
{
  firstCandidateElements.clear();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  vector<vector<int>>::const_iterator elementPositiveMembershipsIt = elementPositiveMemberships.begin();
  for (const vector<vector<unsigned int>>::const_iterator vertexDimensionIt = dimensionIt + ConcurrentPatternPool::vertexDimension; dimensionIt != vertexDimensionIt; ++dimensionIt)
    {
      firstCandidateElements.push_back(upper_bound(elementPositiveMembershipsIt->begin(), elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin());
      ++elementPositiveMembershipsIt;
    }
  firstCandidateElements.push_back(upper_bound(elementPositiveMembershipsIt->begin(), elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(static_cast<double>(dimensionIt->size() + 1) / (dimensionIt->size() - 1)) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin());
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  while (++dimensionIt != dimensionEnd)
    {
      ++elementPositiveMembershipsIt;
      firstCandidateElements.push_back(upper_bound(elementPositiveMembershipsIt->begin(), elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin());
    }
}

#ifdef UPDATE_SUMS
void ModifiedGraphPattern::increaseFirstCandidatesInGraphPattern()
{
  const unsigned int bestDimensionId = bestDimensionIt - nSet.begin();
  if (bestDimensionId == ConcurrentPatternPool::vertexDimension)
    {
      increaseFirstCandidates();
      return;
    }
  vector<unsigned int>::iterator firstCandidateElementIt = firstCandidateElements.begin();
  vector<vector<int>>::const_iterator elementPositiveMembershipsIt = elementPositiveMemberships.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  const vector<vector<unsigned int>>::const_iterator vertexDimensionIt = dimensionIt + ConcurrentPatternPool::vertexDimension;
  if (bestDimensionId < ConcurrentPatternPool::vertexDimension)
    {
      for (; dimensionIt != bestDimensionIt; ++dimensionIt)
	{
	  *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
	  ++firstCandidateElementIt;
	  ++elementPositiveMembershipsIt;
	}
      while (++dimensionIt != vertexDimensionIt)
	{
	  ++firstCandidateElementIt;
	  ++elementPositiveMembershipsIt;
	  *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
	}
      ++firstCandidateElementIt;
      ++elementPositiveMembershipsIt;
      *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(static_cast<double>(vertexDimensionIt->size() + 1) / (vertexDimensionIt->size() - 1)) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
    }
  else
    {
      for (; dimensionIt != vertexDimensionIt; ++dimensionIt)
	{
	  *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
	  ++firstCandidateElementIt;
	  ++elementPositiveMembershipsIt;
	}
      *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(static_cast<double>(vertexDimensionIt->size() + 1) / (vertexDimensionIt->size() - 1)) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
      while (++dimensionIt != bestDimensionIt)
	{
	  ++firstCandidateElementIt;
	  ++elementPositiveMembershipsIt;
	  *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
	}
    }
  for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
    {
      ++firstCandidateElementIt;
      ++elementPositiveMembershipsIt;
      *firstCandidateElementIt = upper_bound(elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, elementPositiveMembershipsIt->begin() + dimensionIt->front(), (sqrt(1. + 1. / static_cast<double>(dimensionIt->size())) - 1.) * membershipSum) - elementPositiveMembershipsIt->begin();
    }
}

void ModifiedGraphPattern::considerInsertingElementsBeforeFirstCandidatesInGraphPattern()
{
  vector<unsigned int>::iterator firstCandidateElementIt = firstCandidateElements.begin();
  vector<vector<int>>::const_iterator sumsInDimensionIt = sumsOnHyperplanes.begin();
  vector<vector<int>>::const_iterator elementPositiveMembershipsIt = elementPositiveMemberships.begin();
  vector<vector<unsigned int>>::iterator dimensionIt = nSet.begin();
  for (const vector<vector<unsigned int>>::iterator vertexDimensionIt = dimensionIt + ConcurrentPatternPool::vertexDimension; dimensionIt != vertexDimensionIt; ++dimensionIt)
    {
      considerInsertingElementsBeforeFirstCandidate(dimensionIt, elementPositiveMembershipsIt->begin(), sumsInDimensionIt->begin(), *firstCandidateElementIt);
      ++firstCandidateElementIt;
      ++sumsInDimensionIt;
      ++elementPositiveMembershipsIt;
    }
  {
    // Consider inserting vertices before the first candidate vertex
    const unsigned int decreasedFirstCandidateElement = upper_bound(elementPositiveMembershipsIt->begin(), elementPositiveMembershipsIt->begin() + *firstCandidateElementIt, sqrt(bestG * area / (dimensionIt->size() - 1) * (dimensionIt->size() + 1)) - membershipSum) - elementPositiveMembershipsIt->begin();
    if (decreasedFirstCandidateElement != *firstCandidateElementIt)
      {
	if (LastTrie::is01)
	  {
	    tensor->sumsOnNewCandidateVertices(nSet, decreasedFirstCandidateElement, *firstCandidateElementIt, sumsOnHyperplanes[ConcurrentPatternPool::vertexDimension], 2 * area / (dimensionIt->size() - 1), AbstractRoughTensor::getUnit());
	  }
	else
	  {
	    tensor->sumsOnNewCandidateVertices(nSet, decreasedFirstCandidateElement, *firstCandidateElementIt, sumsOnHyperplanes[ConcurrentPatternPool::vertexDimension]);
	  }
	vector<int>::const_iterator bestIncreasingSumInDimensionIt = max_element(sumsInDimensionIt->begin() + decreasedFirstCandidateElement, sumsInDimensionIt->begin() + *firstCandidateElementIt, [](const int sum1, const int sum2) {return sum1 <= sum2;}); // in case of equality, prefer adding the globally densest slice
	*firstCandidateElementIt = decreasedFirstCandidateElement;
	double g = membershipSum + *bestIncreasingSumInDimensionIt;
	g *= abs(g) / area * (dimensionIt->size() - 1) / (dimensionIt->size() + 1);
	if (g > bestG)
	  {
	    bestG = g;
	    bestDimensionIt = dimensionIt;
	    bestSumIt = bestIncreasingSumInDimensionIt;
	    nextStep = insert;
	  }
      }
  }
  const vector<vector<unsigned int>>::iterator dimensionEnd = nSet.end();
  while (++dimensionIt != dimensionEnd)
    {
      considerInsertingElementsBeforeFirstCandidate(dimensionIt, (++elementPositiveMembershipsIt)->begin(), (++sumsInDimensionIt)->begin(), *++firstCandidateElementIt);
    }
}
#endif
#endif

void ModifiedGraphPattern::computeAllSums()
{
#ifdef PRUNE
  if (LastTrie::is01)
    {
      membershipSum = tensor->sumOnPattern(nSet.begin(), area, AbstractRoughTensor::getUnit());
      computeFirstCandidateElements();
      tensor->sumsOnGraphHyperplanes(nSet.begin(), firstCandidateElements.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension);
      return;
    }
  membershipSum = tensor->sumOnPattern(nSet.begin());
  computeFirstCandidateElements();
  tensor->sumsOnHyperplanes(nSet.begin(), firstCandidateElements.begin(), sumsOnHyperplanes);
#else
  if (LastTrie::is01)
    {
      membershipSum = tensor->sumsOnGraphPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes, area, AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension);
      return;
    }
  membershipSum = tensor->sumsOnPatternAndHyperplanes(nSet.begin(), sumsOnHyperplanes);
#endif
}

void ModifiedGraphPattern::init()
{
  area = AbstractRoughGraphTensor::graphPatternArea(nSet);
  computeAllSums();
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

bool ModifiedGraphPattern::insertOrErase()
{
  const unsigned int element = bestSumIt - sumsOnHyperplanes[bestDimensionIt - nSet.begin()].begin();
  if (bestDimensionIt - nSet.begin() != ConcurrentPatternPool::vertexDimension)
    {
      area /= bestDimensionIt->size();
      if (nextStep == insert)
	{
	  if (insertAndReturnIfVisited(element))
	    {
	      return false;
	    }
#ifdef UPDATE_SUMS
	  membershipSum += *bestSumIt;
#ifdef PRUNE
	  increaseFirstCandidatesInGraphPattern();
#endif
	  if (LastTrie::is01)
	    {
#ifdef PRUNE
	      tensor->addNonVertexElement(bestDimensionIt - nSet.begin(), element, nSet, firstCandidateElements, sumsOnHyperplanes, area, AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension);
#else
	      tensor->addNonVertexElement(bestDimensionIt - nSet.begin(), element, nSet, sumsOnHyperplanes, area, AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension);
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
	  increaseFirstCandidatesInGraphPattern();
#endif
	  if (LastTrie::is01)
	    {
#ifdef PRUNE
	      tensor->addNonVertexElement(bestDimensionIt - nSet.begin(), element, nSet, firstCandidateElements, sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension); // negating the last two arguments for a decrease
#else
	      tensor->addNonVertexElement(bestDimensionIt - nSet.begin(), element, nSet, sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit(), ConcurrentPatternPool::vertexDimension); // negating the last two arguments for a decrease
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
    }
  else
    {
      area *= 2;
      if (nextStep == insert)
	{
	  area /= bestDimensionIt->size() - 1;
#ifdef DEBUG_MODIFY
	  cout << "    Adding slice for ";
	  roughTensor->printElement(bestDimensionIt - nSet.begin(), element, cout);
	  cout << " gives ";
#endif
	  const vector<unsigned int>::const_iterator vertexIt = bestDimensionIt->insert(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), element), element);
	  if (isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG))
	    {
#ifdef DEBUG_MODIFY
	      roughTensor->printPattern(nSet, static_cast<float>(membershipSum + *bestSumIt) / (area * bestDimensionIt->size() / 2), cout);
	      cout << ", which has already been reached: abort\n";
#endif
	      return false;
	    }
#ifdef UPDATE_SUMS
	  membershipSum += *bestSumIt;
#ifdef PRUNE
	  increaseFirstCandidatesInGraphPattern();
#endif
	  if (LastTrie::is01)
	    {
#ifdef PRUNE
	      tensor->addVertex(bestDimensionIt - nSet.begin(), vertexIt, nSet, firstCandidateElements, sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
#else
	      tensor->addVertex(bestDimensionIt - nSet.begin(), vertexIt, nSet, sumsOnHyperplanes, area, AbstractRoughTensor::getUnit());
#endif
	    }
	  else
	    {
#ifdef PRUNE
	      tensor->addVertex(vertexIt, nSet, firstCandidateElements, sumsOnHyperplanes);
#else
	      tensor->addVertex(vertexIt, nSet, sumsOnHyperplanes);
#endif
	    }
#endif
	  area *= bestDimensionIt->size();
	}
      else
	{
	  area /= bestDimensionIt->size();
#ifdef DEBUG_MODIFY
	  cout << "    Removing slice for ";
	  roughTensor->printElement(bestDimensionIt - nSet.begin(), element, cout);
	  cout << " gives ";
#endif
	  vector<unsigned int>::const_iterator vertexIt = bestDimensionIt->erase(lower_bound(bestDimensionIt->begin(), bestDimensionIt->end(), element));
	  if (isSomeVisitedPatternStored && visitedPatterns->visited(nSet, bestG))
	    {
#ifdef DEBUG_MODIFY
	      roughTensor->printPattern(nSet, static_cast<float>(membershipSum - *bestSumIt) / (area * bestDimensionIt->size() / 2), cout);
	      cout << ", which has already been reached: abort\n";
#endif
	      return false;
	    }
#ifdef UPDATE_SUMS
	  membershipSum -= *bestSumIt;
#ifdef PRUNE
	  increaseFirstCandidatesInGraphPattern();
#endif
	  vertexIt = bestDimensionIt->insert(vertexIt, element);
	  if (LastTrie::is01)
	    {
#ifdef PRUNE
	      tensor->addVertex(bestDimensionIt - nSet.begin(), vertexIt, nSet, firstCandidateElements, sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit()); // negating the last two arguments for a decrease
#else
	      tensor->addVertex(bestDimensionIt - nSet.begin(), vertexIt, nSet, sumsOnHyperplanes, -area, -AbstractRoughTensor::getUnit()); // negating the last two arguments for a decrease
#endif
	    }
	  else
	    {
#ifdef PRUNE
	      tensor->removeVertex(vertexIt, nSet, firstCandidateElements, sumsOnHyperplanes);
#else
	      tensor->removeVertex(vertexIt, nSet, sumsOnHyperplanes);
#endif
	    }
	  bestDimensionIt->erase(vertexIt);
#endif
	  area *= bestDimensionIt->size() - 1;
	}
      area /= 2;
    }
#ifndef UPDATE_SUMS
  computeAllSums();
#endif
#ifdef DEBUG_MODIFY
  roughTensor->printPattern(nSet, static_cast<float>(membershipSum) / area, cout);
  cout << " and g = " << bestG / AbstractRoughTensor::getUnit() / AbstractRoughTensor::getUnit() << '\n';
#endif
  return true;
}

bool ModifiedGraphPattern::doGrowingStepForGraphPattern(const vector<vector<unsigned int>>& firstNonInitialAndSubsequentInitial)
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
  do
    {
      double g = membershipSum - minInitialSum(*sumsInDimensionIt, *firstNonInitialAndSubsequentInitialIt);
      if (g * abs(g) / area * (dimensionIt->size() - 1) / (dimensionIt->size() + 1) > bestG)
	{
#ifdef DEBUG_MODIFY
	  cout << " does not locally maximize g, because of initial element: discard\n";
#endif
	  return false;
	}
      ++sumsInDimensionIt;
      ++firstNonInitialAndSubsequentInitialIt;
    }
  while (++dimensionIt != dimensionEnd);
  localMaximumFound();
  return false;
}
