// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "Trie.h"

#include "VertexTrie.h"
#include "VertexLastTrie.h"

Trie::Trie(): hyperplanes()
{
}

Trie::Trie(Trie&& otherTrie): hyperplanes(std::move(otherTrie.hyperplanes))
{
}

Trie::Trie(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  if (nextNextCardinalityIt == lastCardinalityIt)
    {
      do
	{
	  hyperplanes.push_back(new LastTrie(nextCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new Trie(nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

Trie::Trie(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new VertexLastTrie(lastCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  const unsigned int nextDistanceToVertexDimension = distanceToVertexDimension - 1;
  if (nextDistanceToVertexDimension)
    {
      const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
      do
	{
	  hyperplanes.push_back(new Trie(*nextCardinalityIt, nextNextCardinalityIt, lastCardinalityIt, nextDistanceToVertexDimension));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new VertexTrie(nextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

Trie::Trie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  if (nextNextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrie(membershipIt, nextCardinality, lastCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new Trie(membershipIt, nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

Trie::Trie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  if (nextNextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrie(membershipIt, unit, nextCardinality, lastCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new Trie(membershipIt, unit, nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

Trie::Trie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new VertexLastTrie(membershipIt, unit, lastCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  const unsigned int nextDistanceToVertexDimension = distanceToVertexDimension - 1;
  if (nextDistanceToVertexDimension)
    {
      const unsigned int nextCardinality = *nextCardinalityIt;
      const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
      do
	{
	  hyperplanes.push_back(new Trie(membershipIt, unit, nextCardinality, nextNextCardinalityIt, lastCardinalityIt, nextDistanceToVertexDimension));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new VertexTrie(membershipIt, unit, nextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

Trie::Trie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new VertexLastTrie(membershipIt, lastCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  const unsigned int nextDistanceToVertexDimension = distanceToVertexDimension - 1;
  if (nextDistanceToVertexDimension)
    {
      const unsigned int nextCardinality = *nextCardinalityIt;
      const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
      do
	{
	  hyperplanes.push_back(new Trie(membershipIt, nextCardinality, nextNextCardinalityIt, lastCardinalityIt, nextDistanceToVertexDimension));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new VertexTrie(membershipIt, nextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

Trie::~Trie()
{
  deleteHyperplanes();
}

void Trie::deleteHyperplanes()
{
  const vector<AbstractTrie*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      delete *hyperplaneIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void Trie::setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension)
{
  // is01
  hyperplanes[*idIt]->setTuple(idIt + 1, cardinalityOfLastDimension);
}

void Trie::setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension, const int membership)
{
  // !is01
  hyperplanes[*idIt]->setTuple(idIt + 1, cardinalityOfLastDimension, membership);
}

void Trie::sortTubes()
{
  const vector<AbstractTrie*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->sortTubes();
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void Trie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->sumOnPattern(nextDimensionIt, nextIdEnd, sum);
    }
  while (++idIt != idEnd);
}

void Trie::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->minusSumOnPattern(nextDimensionIt, nextIdEnd, sum);
    }
  while (++idIt != idEnd);
}

void Trie::sumOnAddedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->sumOnAddedVertex(nextDimensionIt, vertexIt, sum);
    }
  while (++idIt != idEnd);
}

void Trie::minusSumOnRemovedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, sum);
    }
  while (++idIt != idEnd);
}

void Trie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const
{
  hyperplanes[element]->sumOnPattern(dimensionIt, dimensionIt->end(), sum);
}

void Trie::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, int& sum) const
{
  hyperplanes[element]->minusSumOnPattern(dimensionIt, dimensionIt->end(), sum);
}

#ifdef PRUNE
void Trie::positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const
{
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      *sumIt++ += (*hyperplaneIt)->increasePositiveMemberships(nextSumsIt);
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int Trie::increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const
{
  int overall = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      const int sum = (*hyperplaneIt)->increasePositiveMemberships(nextSumsIt);
      overall += sum;
      *sumIt++ += sum;
    }
  while (++hyperplaneIt != hyperplaneEnd);
  return overall;
}

long long Trie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  long long overall = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      int sum = 0;
      hyperplanes[*idIt]->sumOnPattern(nextDimensionIt, nextIdEnd, sum);
      overall += sum;
    }
  while (++idIt != idEnd);
  return overall;
}

void Trie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const
{
  reset(firstCandidateElementIt, sumsOnHyperplanes);
  vector<vector<int>>::iterator nextSumsIt = sumsOnHyperplanes.begin();
  vector<int>::iterator sumIt = nextSumsIt->begin() + *firstCandidateElementIt;
  ++nextSumsIt;
  // Exactly sumsOnHyperplanes but without sumOnPattern
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = nSetBegin + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSetBegin->end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSetBegin->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->sumsOnHyperplanes(nextDimensionIt, nextFirstCandidateElementIt, nextPresentElementIdEnd, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

int Trie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(nextDimensionIt, nextFirstCandidateElementIt, nextPresentElementIdEnd, nextSumsIt);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void Trie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not firstCandidateElement
  sumIt += firstCandidateElement;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin() + firstCandidateElement;
  do
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int Trie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void Trie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, const unsigned int firstCandidateElement, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not firstCandidateElement
  sumIt += firstCandidateElement;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin() + firstCandidateElement;
  do
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int Trie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

// sumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int Trie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->sumsOnPatternAndHyperplanes(nextDimensionIt, nextDimensionIt->end(), firstCandidateElementIt + 1, sumsIt + 1);
    }
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	*sumIt++ += sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void Trie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->sumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

// Only element is followed; dimensionIt, firstCandidateElementIt and sumsIt relate to the next dimension
int Trie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->sumsOnPatternAndHyperplanes(dimensionIt, dimensionIt->end(), firstCandidateElementIt, sumsIt);
}

void Trie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  if (increasedDimensionId > 1)
    {
      vector<unsigned int>& increasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[increasedDimensionId];
      vector<unsigned int> elementDimension {element};
      elementDimension.swap(increasedDimension);
      vector<int>& unchangedSums = sums[increasedDimensionId];
      vector<int> empty;
      empty.swap(unchangedSums);
      vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
      const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
      const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	hyperplaneIt += firstCandidateElements.front();
	const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = ++firstCandidateElements.begin();
	const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
		++sumIt;
	      }
	    *sumIt++ += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	  ++sumIt;
	}
      unchangedSums = std::move(empty);
      increasedDimension = std::move(elementDimension);
      return;
    }
  if (increasedDimensionId)	// could be the previous test (here, it tests increasedDimensionId == 1) and the block below removed; it nonetheless directly forwards the added element, to follow next, using calls of sumOnPattern and increaseSumsOnPatternAndHyperplanes that are necessary for the addition of a vertex
    {
      vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
      const vector<vector<unsigned int>>::const_iterator nextNextDimensionIt = nSet.begin() + 2;
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	hyperplaneIt += firstCandidateElements.front();
	const vector<unsigned int>::const_iterator nextNextFirstCandidateElementIt = firstCandidateElements.begin() + 2;
	const vector<vector<int>>::iterator nextNextSumsIt = sums.begin() + 2;
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->sumOnPattern(nextNextDimensionIt, element, *sumIt);
		++sumIt;
	      }
	    *sumIt++ += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextNextDimensionIt, element, nextNextFirstCandidateElementIt, nextNextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->sumOnPattern(nextNextDimensionIt, element, *sumIt);
	  ++sumIt;
	}
      return;
    }
  hyperplanes[element]->increaseSumsOnHyperplanes(++nSet.begin(), (++nSet.begin())->end(), ++firstCandidateElements.begin(), ++sums.begin());
}

// All present elements before dimensionIt->end() are considered
int Trie::increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
      {
	(*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *sumIt);
	++sumIt;
      }
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    sumOnPattern = (*hyperplaneIt)->increaseSumsOnAddedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextFirstCandidateElementIt, nextSumsIt);
    *sumIt += sumOnPattern;
    for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); ++presentElementIdIt != presentElementIdEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnAddedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextFirstCandidateElementIt, nextSumsIt);
	*++sumIt += sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

void Trie::addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // *this is the root of the trie and added element is a vertex: consequently, increasedDimensionId != 0
  vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += firstCandidateElements.front();
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = ++firstCandidateElements.begin();
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->increaseSumsOnAddedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextFirstCandidateElementIt, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *sumIt);
      ++sumIt;
    }
}

// minusSumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey decreaseSumsOnPatternAndHyperplanes instead of minusSumsOnPatternAndHyperplanes
int Trie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->minusSumsOnPatternAndHyperplanes(nextDimensionIt, nextDimensionIt->end(), firstCandidateElementIt + 1, sumsIt + 1);
    }
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void Trie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ -= (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

// Only element is followed; dimensionIt, firstCandidateElementIt and sumsIt relate to the next dimension
int Trie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->minusSumsOnPatternAndHyperplanes(dimensionIt, dimensionIt->end(), firstCandidateElementIt, sumsIt);
}

void Trie::removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // !is01
  if (decreasedDimensionId > 1)
    {
      vector<unsigned int>& decreasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[decreasedDimensionId];
      vector<unsigned int> elementDimension {element};
      elementDimension.swap(decreasedDimension);
      vector<int>& unchangedSums = sums[decreasedDimensionId];
      vector<int> empty;
      empty.swap(unchangedSums);
      vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
      const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
      const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	hyperplaneIt += firstCandidateElements.front();
	const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = ++firstCandidateElements.begin();
	const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
		++sumIt;
	      }
	    *sumIt++ -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextFirstCandidateElementIt, nextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	  ++sumIt;
	}
      unchangedSums = std::move(empty);
      decreasedDimension = std::move(elementDimension);
      return;
    }
  if (decreasedDimensionId)	// could be the previous test (here, it tests decreasedDimensionId == 1) and the block below removed; it nonetheless directly forwards the removed element, to follow next, using calls of minusSumOnPattern and decreaseSumsOnPatternAndHyperplanes that are necessary for the removal of a vertex
    {
      vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();
      const vector<vector<unsigned int>>::const_iterator nextNextDimensionIt = nSet.begin() + 2;
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	hyperplaneIt += firstCandidateElements.front();
	const vector<unsigned int>::const_iterator nextNextFirstCandidateElementIt = firstCandidateElements.begin() + 2;
	const vector<vector<int>>::iterator nextNextSumsIt = sums.begin() + 2;
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->minusSumOnPattern(nextNextDimensionIt, element, *sumIt);
		++sumIt;
	      }
	    *sumIt++ -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextNextDimensionIt, element, nextNextFirstCandidateElementIt, nextNextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->minusSumOnPattern(nextNextDimensionIt, element, *sumIt);
	  ++sumIt;
	}
      return;
    }
  hyperplanes[element]->decreaseSumsOnHyperplanes(++nSet.begin(), (++nSet.begin())->end(), ++firstCandidateElements.begin(), ++sums.begin());
}

// All present elements before dimensionIt->end() are considered
int Trie::decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *firstCandidateElementIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
      {
	(*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *sumIt);
	++sumIt;
      }
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    sumOnPattern = (*hyperplaneIt)->decreaseSumsOnRemovedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextFirstCandidateElementIt, nextSumsIt);
    *sumIt -= sumOnPattern;
    for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); ++presentElementIdIt != presentElementIdEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnRemovedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextFirstCandidateElementIt, nextSumsIt);
	*++sumIt -= sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

void Trie::removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // *this is the root of the trie and removed element is a vertex: consequently, increasedDimensionId != 0
  vector<int>::iterator sumIt = sums.front().begin() + firstCandidateElements.front();;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += firstCandidateElements.front();
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = ++firstCandidateElements.begin();
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ -= (*hyperplaneIt)->decreaseSumsOnRemovedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextFirstCandidateElementIt, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *sumIt);
      ++sumIt;
    }
}

void Trie::increaseSumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  if (dimensionIt->empty())
    {
      increaseSumsOnNewCandidateElementsOfThisDimension(dimensionIt, newFirstCandidateElement, oldFirstCandidateElement, sums);
      return;
    }
  // The new candidates are of some subsequent dimension; follow the present elements of this dimension
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateHyperplanes(nextDimensionIt, nextPresentElementIdEnd, newFirstCandidateElement, oldFirstCandidateElement, sums);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

void Trie::sumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::iterator dimensionWithCandidatesIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes) const
{
  // !is01
  fill_n(sumsOnHyperplanes.begin() + newFirstCandidateElement, oldFirstCandidateElement - newFirstCandidateElement, 0);
  vector<unsigned int> empty;
  empty.swap(*dimensionWithCandidatesIt);
  increaseSumsOnNewCandidateHyperplanes(nSet.begin(), nSet.front().end(), newFirstCandidateElement, oldFirstCandidateElement, sumsOnHyperplanes);
  *dimensionWithCandidatesIt = std::move(empty);
}

void Trie::increaseSumsOnNewCandidateElementsOfThisDimension(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sums.begin() + newFirstCandidateElement;
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.begin() + oldFirstCandidateElement;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin() + newFirstCandidateElement;
  do
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void Trie::increaseSumsOnNewCandidateVertices(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sums) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  // The new candidates being vertices, they are of some subsequent dimension; follow the present elements of this dimension
  vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateVertices(nextDimensionIt, nextPresentElementIdEnd, newFirstCandidateVertex, oldFirstCandidateVertex, sums);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

#else

void Trie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  if (increasedDimensionId > 1)
    {
      vector<unsigned int>& increasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[increasedDimensionId];
      vector<unsigned int> elementDimension {element};
      elementDimension.swap(increasedDimension);
      vector<int>& unchangedSums = sums[increasedDimensionId];
      vector<int> empty;
      empty.swap(unchangedSums);
      vector<int>::iterator sumIt = sums.front().begin();
      const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
      const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
		++sumIt;
	      }
	    *sumIt++ += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	  ++sumIt;
	}
      unchangedSums = std::move(empty);
      increasedDimension = std::move(elementDimension);
      return;
    }
  if (increasedDimensionId)	// could be the previous test (here, it tests increasedDimensionId == 1) and the block below removed; it nonetheless directly forwards the added element, to follow next, using calls of sumOnPattern and increaseSumsOnPatternAndHyperplanes that are necessary for the addition of a vertex
    {
      vector<int>::iterator sumIt = sums.front().begin();
      const vector<vector<unsigned int>>::const_iterator nextNextDimensionIt = nSet.begin() + 2;
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	const vector<vector<int>>::iterator nextNextSumsIt = sums.begin() + 2;
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->sumOnPattern(nextNextDimensionIt, element, *sumIt);
		++sumIt;
	      }
	    *sumIt++ += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextNextDimensionIt, element, nextNextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->sumOnPattern(nextNextDimensionIt, element, *sumIt);
	  ++sumIt;
	}
      return;
    }
  hyperplanes[element]->increaseSumsOnHyperplanes(++nSet.begin(), (++nSet.begin())->end(), ++sums.begin());
}

void Trie::removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // !is01
  if (decreasedDimensionId > 1)
    {
      vector<unsigned int>& decreasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[decreasedDimensionId];
      vector<unsigned int> elementDimension {element};
      elementDimension.swap(decreasedDimension);
      vector<int>& unchangedSums = sums[decreasedDimensionId];
      vector<int> empty;
      empty.swap(unchangedSums);
      vector<int>::iterator sumIt = sums.front().begin();
      const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
      const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
		++sumIt;
	      }
	    *sumIt++ -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	  ++sumIt;
	}
      unchangedSums = std::move(empty);
      decreasedDimension = std::move(elementDimension);
      return;
    }
  if (decreasedDimensionId)	// could be the previous test (here, it tests decreasedDimensionId == 1) and the block below removed; it nonetheless directly forwards the removed element, to follow next, using calls of minusSumOnPattern and decreaseSumsOnPatternAndHyperplanes that are necessary for the removal of a vertex
    {
      vector<int>::iterator sumIt = sums.front().begin();
      const vector<vector<unsigned int>>::const_iterator nextNextDimensionIt = nSet.begin() + 2;
      vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
      {
	// Hyperplanes until the last present one
	const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
	const vector<vector<int>>::iterator nextNextSumsIt = sums.begin() + 2;
	const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
	vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
	do
	  {
	    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	      {
		(*hyperplaneIt)->minusSumOnPattern(nextNextDimensionIt, element, *sumIt);
		++sumIt;
	      }
	    *sumIt++ -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextNextDimensionIt, element, nextNextSumsIt);
	    ++hyperplaneIt;
	  }
	while (++presentElementIdIt != presentElementIdEnd);
      }
      // Hyperplanes after the last present one
      for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
	{
	  (*hyperplaneIt)->minusSumOnPattern(nextNextDimensionIt, element, *sumIt);
	  ++sumIt;
	}
      return;
    }
  hyperplanes[element]->decreaseSumsOnHyperplanes(++nSet.begin(), (++nSet.begin())->end(), ++sums.begin());
}

void Trie::addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // *this is the root of the trie and added element is a vertex: consequently, increasedDimensionId != 0
  vector<int>::iterator sumIt = sums.front().begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->increaseSumsOnAddedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *sumIt);
      ++sumIt;
    }
}

void Trie::removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // *this is the root of the trie and removed element is a vertex: consequently, increasedDimensionId != 0
  vector<int>::iterator sumIt = sums.front().begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = ++nSet.begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSet.front().end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSet.front().begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ -= (*hyperplaneIt)->decreaseSumsOnRemovedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *sumIt);
      ++sumIt;
    }
}

// sumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int Trie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->sumsOnPatternAndHyperplanes(nextDimensionIt, nextDimensionIt->end(), sumsIt + 1);
    }
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	*sumIt++ += sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

// Only element is followed; dimensionIt and sumsIt relate to the next dimension
int Trie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->sumsOnPatternAndHyperplanes(dimensionIt, dimensionIt->end(), sumsIt);
}

void Trie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ += (*hyperplaneIt)->sumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

// minusSumsOnPatternAndHyperplanes plus the test sumsIt->empty() to detect the added element and calling recursivey decreaseSumsOnPatternAndHyperplanes instead of minusSumsOnPatternAndHyperplanes
int Trie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (sumsIt->empty())
    {
      return hyperplanes[dimensionIt->front()]->minusSumsOnPatternAndHyperplanes(nextDimensionIt, nextDimensionIt->end(), sumsIt + 1);
    }
  int sumOnPattern = 0;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

// Only element is followed; dimensionIt and sumsIt relate to the next dimension
int Trie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const unsigned int element, const vector<vector<int>>::iterator sumsIt) const
{
  return hyperplanes[element]->minusSumsOnPatternAndHyperplanes(dimensionIt, dimensionIt->end(), sumsIt);
}

void Trie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	*sumIt++ -= (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
}

// All present elements before dimensionIt->end() are considered
int Trie::increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
      {
	(*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *sumIt);
	++sumIt;
      }
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    sumOnPattern = (*hyperplaneIt)->increaseSumsOnAddedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextSumsIt);
    *sumIt += sumOnPattern;
    for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); ++presentElementIdIt != presentElementIdEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnAddedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextSumsIt);
	*++sumIt += sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnAddedVertex(nextDimensionIt, vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

// All present elements before dimensionIt->end() are considered
int Trie::decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
      {
	(*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *sumIt);
	++sumIt;
      }
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    sumOnPattern = (*hyperplaneIt)->decreaseSumsOnRemovedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextSumsIt);
    *sumIt -= sumOnPattern;
    for (const vector<unsigned int>::const_iterator presentElementIdEnd = dimensionIt->end(); ++presentElementIdIt != presentElementIdEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnRemovedVertexAndHyperplanes(nextDimensionIt, vertexIt, nextSumsIt);
	*++sumIt -= sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnRemovedVertex(nextDimensionIt, vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

int Trie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	*sumIt++ -= sum;
	sumOnPattern += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void Trie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not 0
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}
#endif

#if defined ASSERT || !defined PRUNE
int Trie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  int sumOnPattern = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<int>::iterator sumIt = sumsIt->begin();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

long long Trie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const
{
  reset(sumsOnHyperplanes);
  vector<vector<int>>::iterator nextSumsIt = sumsOnHyperplanes.begin();
  vector<int>::iterator sumIt = nextSumsIt->begin();
  ++nextSumsIt;
  // Exactly sumsOnPatternAndHyperplanes but storing sumOnPattern in a long long, because an int may be insufficient
  long long sumOnPattern = 0;
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = nSetBegin + 1;
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<unsigned int>::const_iterator presentElementIdEnd = nSetBegin->end();
    vector<unsigned int>::const_iterator presentElementIdIt = nSetBegin->begin();
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; hyperplaneIt != end; ++hyperplaneIt)
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
	    ++sumIt;
	  }
	const int sum = (*hyperplaneIt)->sumsOnPatternAndHyperplanes(nextDimensionIt, nextPresentElementIdEnd, nextSumsIt);
	sumOnPattern += sum;
	*sumIt++ += sum;
	++hyperplaneIt;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  return sumOnPattern;
}

void Trie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nextDimensionIt, vector<int>::iterator sumIt) const
{
  // *this relates to the first vertex and its id is not 0
  const vector<unsigned int>::const_iterator nextPresentElementIdEnd = nextDimensionIt->end();
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, nextPresentElementIdEnd, *sumIt);
      ++sumIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}
#endif
