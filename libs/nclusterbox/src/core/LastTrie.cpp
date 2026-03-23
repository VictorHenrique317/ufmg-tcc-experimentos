// Copyright 2024-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "LastTrie.h"

#include "SparseCrispTube.h"
#include "SparseFuzzyTube.h"

bool LastTrie::is01;

LastTrie::LastTrie(): hyperplanes()
{
}

LastTrie::LastTrie(LastTrie&& otherLastTrie): hyperplanes(std::move(otherLastTrie.hyperplanes))
{
}

LastTrie::LastTrie(const unsigned int cardinality): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingTubes = cardinality;
  if (is01)
    {
      do
	{
	  hyperplanes.push_back(new SparseCrispTube());
	}
      while (--nbOfMissingTubes);
      return;
    }
  do
    {
      hyperplanes.push_back(new SparseFuzzyTube());
    }
  while (--nbOfMissingTubes);
}

LastTrie::LastTrie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality, const unsigned int nextCardinality): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  do
    {
      hyperplanes.push_back(new DenseCrispTube(membershipIt, nextCardinality));
    }
  while (--nbOfMissingHyperplanes);
}

LastTrie::LastTrie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality, const unsigned int nextCardinality): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  do
    {
      hyperplanes.push_back(new DenseFuzzyTube(membershipIt, unit, nextCardinality));
    }
  while (--nbOfMissingHyperplanes);
}

LastTrie::~LastTrie()
{
  deleteHyperplanes();
}

void LastTrie::deleteHyperplanes()
{
  const vector<Tube*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      delete *hyperplaneIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void LastTrie::setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension)
{
  // is01
  Tube*& hyperplane = hyperplanes[*idIt];
  if (hyperplane->isFullSparseTube(cardinalityOfLastDimension))
    {
      DenseCrispTube* newHyperplane = static_cast<SparseCrispTube*>(hyperplane)->getDenseRepresentation(cardinalityOfLastDimension);
      delete hyperplane;
      hyperplane = newHyperplane;
    }
  hyperplane->set(*(idIt + 1));
}

void LastTrie::setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension, const int membership)
{
  // !is01
  Tube*& hyperplane = hyperplanes[*idIt];
  if (hyperplane->isFullSparseTube(cardinalityOfLastDimension))
    {
      DenseFuzzyTube* newHyperplane = static_cast<SparseFuzzyTube*>(hyperplane)->getDenseRepresentation(cardinalityOfLastDimension);
      delete hyperplane;
      hyperplane = newHyperplane;
    }
  hyperplane->set(*(idIt + 1), membership);
}

void LastTrie::sortTubes()
{
  const vector<Tube*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->sortTubes();
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void LastTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->sumOnPattern(nextIdBegin, nextIdEnd, sum);
    }
  while (++idIt != idEnd);
}

void LastTrie::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->minusSumOnPattern(nextIdBegin, nextIdEnd, sum);
    }
  while (++idIt != idEnd);
}

void LastTrie::sumOnAddedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const
{
  // *this is actually a VertexLastTrie
  if (vertexIt != dimensionIt->begin())
    {
      hyperplanes[*vertexIt]->sumOnPattern(dimensionIt->begin(), vertexIt, sum);
    }
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = vertexIt; ++idIt != idEnd; )
    {
      sum += hyperplanes[*idIt]->at(*vertexIt);
    }
}

void LastTrie::minusSumOnRemovedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const
{
  // *this is actually a VertexLastTrie
  if (vertexIt != dimensionIt->begin())
    {
      hyperplanes[*vertexIt]->minusSumOnPattern(dimensionIt->begin(), vertexIt, sum);
    }
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = vertexIt; ++idIt != idEnd; )
    {
      sum -= hyperplanes[*idIt]->at(*vertexIt);
    }
}

void LastTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const
{
  hyperplanes[element]->sumOnPattern(dimensionIt->begin(), dimensionIt->end(), sum);
}

void LastTrie::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const
{
  hyperplanes[element]->minusSumOnPattern(dimensionIt->begin(), dimensionIt->end(), sum);
}

#ifdef PRUNE
void LastTrie::positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const
{
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<int>& nextSums = *(sumsIt + 1);
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      *sumIt++ += (*hyperplaneIt)->increasePositiveMemberships(nextSums);
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int LastTrie::increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const
{
  int overall = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<int>& nextSums = *(sumsIt + 1);
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      const int sum = (*hyperplaneIt)->increasePositiveMemberships(nextSums);
      overall += sum;
      *sumIt++ += sum;
    }
  while (++hyperplaneIt != hyperplaneEnd);
  return overall;
}

long long LastTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  long long overall = 0;
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      int sum = 0;
      hyperplanes[*idIt]->sumOnPattern(nextIdBegin, nextIdEnd, sum);
      overall += sum;
    }
  while (++idIt != idEnd);
  return overall;
}

void LastTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const
{
  reset(firstCandidateElementIt, sumsOnHyperplanes);
  vector<vector<int>>::iterator nextSumsIt = sumsOnHyperplanes.begin();
  vector<int>::iterator sumIt = nextSumsIt->begin() + *firstCandidateElementIt;
  ++nextSumsIt;
  // Exactly sumsOnHyperplanes but without sumOnPattern
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (nSetBegin + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (nSetBegin + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    const unsigned int nextFirstCandidateElement = *(firstCandidateElementIt + 1);
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSetBegin->end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSetBegin->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextFirstCandidateElement, *nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

int LastTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    const unsigned int nextFirstCandidateElement = *(firstCandidateElementIt + 1);
    vector<int>& nextSums = *(sumsIt + 1);
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextFirstCandidateElement, nextSums);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void LastTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not firstCandidateElement; sumIt has already been advanced by firstCandidateElement
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = nextDimensionIt->begin();
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + firstCandidateElement;
  do
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void LastTrie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  if (increasedDimensionId)
    {
      vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
      const vector<Tube*>::const_iterator end = hyperplanes.end();
      vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + firstCandidateElements.front();
      do
	{
	  *sumIt++ += (*hyperplaneIt)->at(element);
	}
      while (++hyperplaneIt != end);
      return;
    }
  hyperplanes[element]->increaseSumsOnHyperplanes(firstCandidateElements.back(), *++sums.begin());
}

void LastTrie::removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // !is01
  if (decreasedDimensionId)
    {
      vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
      const vector<Tube*>::const_iterator end = hyperplanes.end();
      vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + firstCandidateElements.front();;
      do
	{
	  *sumIt++ -= (*hyperplaneIt)->at(element);
	}
      while (++hyperplaneIt != end);
      return;
    }
  hyperplanes[element]->decreaseSumsOnHyperplanes(firstCandidateElements.back(), *++sums.begin());
}

void LastTrie::addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // Added element is a vertex, hence n == 2 and *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before *(hyperplanes.begin() + *vertexIt), because vertex cannot be followed from them
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + *vertexIt;
  if (*vertexIt != firstCandidateElements.front())
    {
      (*hyperplaneIt)->increaseSumsOnHyperplanes(firstCandidateElements.front(), sums.front());
    }
  vector<int>::iterator sumIt = sums.front().begin() + *vertexIt;
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt += (*hyperplaneIt)->at(*vertexIt);
    }
}

void LastTrie::removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // Removed element is a vertex, hence n == 2 and *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before *(hyperplanes.begin() + *vertexIt), because vertex cannot be followed from them
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + *vertexIt;
  if (*vertexIt != firstCandidateElements.front())
    {
      (*hyperplaneIt)->decreaseSumsOnHyperplanes(firstCandidateElements.front(), sums.front());
    }
  vector<int>::iterator sumIt = sums.front().begin() + *vertexIt;
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt -= (*hyperplaneIt)->at(*vertexIt);
    }
}

void LastTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const unsigned int nextFirstCandidateElement = *(firstCandidateElementIt + 1);
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextFirstCandidateElement, nextSums);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

void LastTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const unsigned int nextFirstCandidateElement = *(firstCandidateElementIt + 1);
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ -= (*hyperplaneIt)->minusSumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextFirstCandidateElement, nextSums);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

int LastTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const unsigned int nextFirstCandidateElement = *(firstCandidateElementIt + 1);
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextFirstCandidateElement, nextSums);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

int LastTrie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const unsigned int nextFirstCandidateElement = *(firstCandidateElementIt + 1);
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextFirstCandidateElement, nextSums);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

int LastTrie::increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  if (*vertexIt != *firstCandidateElementIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->sumsOnHyperplanes(dimensionIt->begin(), vertexIt, *firstCandidateElementIt, *sumsIt);
	}
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<int>::iterator sumIt = sumsIt->begin() + *vertexIt;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    *++sumIt += (*hyperplaneIt)->at(*vertexIt);
	  }
	const int sum = (*hyperplaneIt)->at(*vertexIt);
	*++sumIt += sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt += (*hyperplaneIt)->at(*vertexIt);
    }
  return sumOnPattern;
}

// sumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int LastTrie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->sumsOnHyperplanes((dimensionIt + 1)->begin(), (dimensionIt + 1)->end(), *(firstCandidateElementIt + 1), *(sumsIt + 1));
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  const unsigned int addedElement = (dimensionIt + 1)->front();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    *sumIt++ += (*hyperplaneIt)->at(addedElement);
	  }
	const int sum = (*hyperplaneIt)->at(addedElement);
	*sumIt++ += sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      *sumIt++ += (*hyperplaneIt)->at(addedElement);
    }
  return sumOnPattern;
}

// Only element is followed; dimensionIt, firstCandidateElementIt and sumsIt relate to the next dimension
int LastTrie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->sumsOnHyperplanes(dimensionIt->begin(), dimensionIt->end(), *firstCandidateElementIt, *sumsIt);
}

void LastTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not firstCandidateElement; sumIt has already been advanced by firstCandidateElement
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = nextDimensionIt->begin();
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + firstCandidateElement;
  do
    {
      (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int LastTrie::decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  if (*vertexIt != *firstCandidateElementIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->minusSumsOnHyperplanes(dimensionIt->begin(), vertexIt, *firstCandidateElementIt, *sumsIt);
	}
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<int>::iterator sumIt = sumsIt->begin() + *vertexIt;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    *++sumIt -= (*hyperplaneIt)->at(*vertexIt);
	  }
	const int sum = (*hyperplaneIt)->at(*vertexIt);
	*++sumIt -= sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt -= (*hyperplaneIt)->at(*vertexIt);
    }
  return sumOnPattern;
}

// minusSumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int LastTrie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->sumsOnHyperplanes((dimensionIt + 1)->begin(), (dimensionIt + 1)->end(), *(firstCandidateElementIt + 1), *(sumsIt + 1));
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  const unsigned int removedElement = (dimensionIt + 1)->front();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    *sumIt++ -= (*hyperplaneIt)->at(removedElement);
	  }
	const int sum = (*hyperplaneIt)->at(removedElement);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      *sumIt++ -= (*hyperplaneIt)->at(removedElement);
    }
  return sumOnPattern;
}

// Only element is followed; dimensionIt, firstCandidateElementIt and sumsIt relate to the next dimension
int LastTrie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->minusSumsOnHyperplanes(dimensionIt->begin(), dimensionIt->end(), *firstCandidateElementIt, *sumsIt);
}

void LastTrie::increaseSumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  if (dimensionIt->empty())
    {
      increaseSumsOnNewCandidateElementsOfThisDimension(dimensionIt, newFirstCandidateElement, oldFirstCandidateElement, sums);
      return;
    }
  // The new candidates are of some subsequent dimension; follow the present elements of this dimension
  vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateHyperplanes(newFirstCandidateElement, oldFirstCandidateElement, sums);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

void LastTrie::sumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::iterator dimensionWithCandidatesIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes) const
{
  // !is01 and n == 2
  fill_n(sumsOnHyperplanes.begin() + newFirstCandidateElement, oldFirstCandidateElement - newFirstCandidateElement, 0);
  if (dimensionWithCandidatesIt == nSet.begin())
    {
      // The new candidates are of this dimension; increase the related sums
      const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionWithCandidatesIt + 1)->end();
      const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionWithCandidatesIt + 1)->begin();
      vector<int>::iterator sumIt = sumsOnHyperplanes.begin() + newFirstCandidateElement;
      const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.begin() + oldFirstCandidateElement;
      vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + newFirstCandidateElement;
      do
	{
	  (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	  ++sumIt;
	}
      while (++hyperplaneIt != hyperplaneEnd);
      return;
    }
  // The new candidates are of the last dimension; follow the present elements of this dimension
  const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
  vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateHyperplanes(newFirstCandidateElement, oldFirstCandidateElement, sumsOnHyperplanes);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

void LastTrie::increaseSumsOnNewCandidateElementsOfThisDimension(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<int>::iterator sumIt = sums.begin() + newFirstCandidateElement;
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.begin() + oldFirstCandidateElement;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin() + newFirstCandidateElement;
  do
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void LastTrie::increaseSumsOnNewCandidateVertices(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sums) const
{
  // The new candidates are of this dimension; increase the related sums
  vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateHyperplanes(newFirstCandidateVertex, oldFirstCandidateVertex, sums);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

#else

void LastTrie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  if (increasedDimensionId)
    {
      vector<int>::iterator sumIt = sums.front().begin();
      const vector<Tube*>::const_iterator end = hyperplanes.end();
      vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
      do
	{
	  *sumIt++ += (*hyperplaneIt)->at(element);
	}
      while (++hyperplaneIt != end);
      return;
    }
  hyperplanes[element]->increaseSumsOnHyperplanes(*++sums.begin());
}

void LastTrie::removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // !is01
  if (decreasedDimensionId)
    {
      vector<int>::iterator sumIt = sums.front().begin();
      const vector<Tube*>::const_iterator end = hyperplanes.end();
      vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
      do
	{
	  *sumIt++ -= (*hyperplaneIt)->at(element);
	}
      while (++hyperplaneIt != end);
      return;
    }
  hyperplanes[element]->decreaseSumsOnHyperplanes(*++sums.begin());
}

void LastTrie::addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // Added element is a vertex, hence n == 2 and *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before *(hyperplanes.begin() + *vertexIt), because vertex cannot be followed from them
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  if (*vertexIt)
    {
      hyperplaneIt += *vertexIt;
      (*hyperplaneIt)->increaseSumsOnHyperplanes(sums.front());
    }
  vector<int>::iterator sumIt = sums.front().begin() + *vertexIt;
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt += (*hyperplaneIt)->at(*vertexIt);
    }
}

void LastTrie::removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // Removed element is a vertex, hence n == 2 and *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before *(hyperplanes.begin() + *vertexIt), because vertex cannot be followed from them
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  if (*vertexIt)
    {
      hyperplaneIt += *vertexIt;
      (*hyperplaneIt)->decreaseSumsOnHyperplanes(sums.front());
    }
  vector<int>::iterator sumIt = sums.front().begin() + *vertexIt;
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt -= (*hyperplaneIt)->at(*vertexIt);
    }
}

// sumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int LastTrie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->sumsOnHyperplanes((dimensionIt + 1)->begin(), (dimensionIt + 1)->end(), *(sumsIt + 1));
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  const unsigned int addedElement = (dimensionIt + 1)->front();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    *sumIt++ += (*hyperplaneIt)->at(addedElement);
	  }
	const int sum = (*hyperplaneIt)->at(addedElement);
	*sumIt++ += sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      *sumIt++ += (*hyperplaneIt)->at(addedElement);
    }
  return sumOnPattern;
}

// Only element is followed; dimensionIt and sumsIt relate to the next dimension
int LastTrie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->sumsOnHyperplanes(dimensionIt->begin(), dimensionIt->end(), *sumsIt);
}

void LastTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin();
  {
    // Hyperplanes until the last present one
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextSums);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

// minusSumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int LastTrie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->sumsOnHyperplanes((dimensionIt + 1)->begin(), (dimensionIt + 1)->end(), *(sumsIt + 1));
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  const unsigned int removedElement = (dimensionIt + 1)->front();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    *sumIt++ -= (*hyperplaneIt)->at(removedElement);
	  }
	const int sum = (*hyperplaneIt)->at(removedElement);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      *sumIt++ -= (*hyperplaneIt)->at(removedElement);
    }
  return sumOnPattern;
}

// Only element is followed; dimensionIt and sumsIt relate to the next dimension
int LastTrie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->minusSumsOnHyperplanes(dimensionIt->begin(), dimensionIt->end(), *sumsIt);
}

void LastTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin();
  {
    // Hyperplanes until the last present one
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ -= (*hyperplaneIt)->minusSumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextSums);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

int LastTrie::increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  vector<int>::iterator sumIt = sumsIt->begin();
  if (*vertexIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(*sumsIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->sumsOnHyperplanes(dimensionIt->begin(), vertexIt, *sumsIt);
	}
      sumIt +=*vertexIt;
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    *++sumIt += (*hyperplaneIt)->at(*vertexIt);
	  }
	const int sum = (*hyperplaneIt)->at(*vertexIt);
	*++sumIt += sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt += (*hyperplaneIt)->at(*vertexIt);
    }
  return sumOnPattern;
}

int LastTrie::decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const
{
  // *this is actually a VertexLastTrie
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  vector<int>::iterator sumIt = sumsIt->begin();
  if (*vertexIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(*sumsIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->minusSumsOnHyperplanes(dimensionIt->begin(), vertexIt, *sumsIt);
	}
      sumIt +=*vertexIt;
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    *++sumIt -= (*hyperplaneIt)->at(*vertexIt);
	  }
	const int sum = (*hyperplaneIt)->at(*vertexIt);
	*++sumIt -= sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      *++sumIt -= (*hyperplaneIt)->at(*vertexIt);
    }
  return sumOnPattern;
}

int LastTrie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin();
  {
    // Hyperplanes until the last present one
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextSums);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void LastTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not 0
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = nextDimensionIt->begin();
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->minusSumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}
#endif

#if defined ASSERT || !defined PRUNE
int LastTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (dimensionIt + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin();
  {
    // Hyperplanes until the last present one
    vector<int>& nextSums = *(sumsIt + 1);
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, nextSums);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

long long LastTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const
{
  reset(sumsOnHyperplanes);
  vector<vector<int>>::iterator nextSumsIt = sumsOnHyperplanes.begin();
  vector<int>::iterator sumIt = nextSumsIt->begin();
  ++nextSumsIt;
  // Exactly sumsOnPatternAndHyperplanes but storing sumOnPattern in a long long, because an int may be insufficient
  long long sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = (nSetBegin + 1)->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = (nSetBegin + 1)->begin();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSetBegin->end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSetBegin->begin();
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(nextPresentElementIdBegin, nextPresentElementIdEnd, *nextSumsIt);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void LastTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not 0
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<unsigned int>::const_iterator nextPresentElementIdBegin = nextDimensionIt->begin();
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->sumOnPattern(nextPresentElementIdBegin, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}
#endif
