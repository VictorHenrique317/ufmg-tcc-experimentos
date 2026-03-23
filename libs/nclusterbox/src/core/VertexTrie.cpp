// Copyright 2023-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VertexTrie.h"

#include "LastTrie.h"

VertexTrie::VertexTrie(): Trie()
{
}

VertexTrie::VertexTrie(const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): Trie()
{
  const unsigned int cardinality = *cardinalityIt;
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  const vector<unsigned int>::const_iterator nextCardinalityIt = cardinalityIt + 1;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      do
	{
	  hyperplanes.push_back(new LastTrie(nbOfVerticesBefore));
	}
      while (++nbOfVerticesBefore != cardinality);
      return;
    }
  do
    {
      hyperplanes.push_back(new Trie(nbOfVerticesBefore, nextCardinalityIt, lastCardinalityIt));
    }
  while (++nbOfVerticesBefore != cardinality);
}

VertexTrie::VertexTrie(vector<double>::const_iterator& membershipIt, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): Trie()
{
  const unsigned int cardinality = *cardinalityIt;
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  const vector<unsigned int>::const_iterator nextCardinalityIt = cardinalityIt + 1;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrie(membershipIt, nbOfVerticesBefore, lastCardinality));
	}
      while (++nbOfVerticesBefore != cardinality);
      return;
    }
  do
    {
      hyperplanes.push_back(new Trie(membershipIt, nbOfVerticesBefore, nextCardinalityIt, lastCardinalityIt));
    }
  while (++nbOfVerticesBefore != cardinality);
}

VertexTrie::VertexTrie(vector<double>::const_iterator& membershipIt, const int unit, const vector<unsigned int>::const_iterator cardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): Trie()
{
  const unsigned int cardinality = *cardinalityIt;
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  const vector<unsigned int>::const_iterator nextCardinalityIt = cardinalityIt + 1;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrie(membershipIt, unit, nbOfVerticesBefore, lastCardinality));
	}
      while (++nbOfVerticesBefore != cardinality);
      return;
    }
  do
    {
      hyperplanes.push_back(new Trie(membershipIt, unit, nbOfVerticesBefore, nextCardinalityIt, lastCardinalityIt));
    }
  while (++nbOfVerticesBefore != cardinality);
}

void VertexTrie::deleteHyperplanes()
{
  const vector<AbstractTrie*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      delete *hyperplaneIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void VertexTrie::sortTubes()
{
  const vector<AbstractTrie*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      (*hyperplaneIt)->sortTubes();
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void VertexTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  vector<unsigned int>::const_iterator idIt = ++dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->sumOnPattern(dimensionIt, idIt, sum);
    }
  while (++idIt != idEnd);
}

void VertexTrie::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  vector<unsigned int>::const_iterator idIt = ++dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->minusSumOnPattern(dimensionIt, idIt, sum);
    }
  while (++idIt != idEnd);
}

void VertexTrie::sumOnAddedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const
{
  if (vertexIt != dimensionIt->begin())
    {
      hyperplanes[*vertexIt]->sumOnPattern(dimensionIt, vertexIt, sum);
    }
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = vertexIt; ++idIt != idEnd; )
    {
      hyperplanes[*idIt]->sumOnPattern(nextDimensionIt, *vertexIt, sum);
    }
}

void VertexTrie::minusSumOnRemovedVertex(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, int& sum) const
{
  if (vertexIt != dimensionIt->begin())
    {
      hyperplanes[*vertexIt]->minusSumOnPattern(dimensionIt, vertexIt, sum);
    }
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  for (vector<unsigned int>::const_iterator idIt = vertexIt; ++idIt != idEnd; )
    {
      hyperplanes[*idIt]->minusSumOnPattern(nextDimensionIt, *vertexIt, sum);
    }
}

#ifdef PRUNE
void VertexTrie::positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const
{
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      *++sumIt += (*hyperplaneIt)->increasePositiveMemberships(sumsIt);
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int VertexTrie::increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const
{
  int overall = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      int sum = (*hyperplaneIt)->increasePositiveMemberships(sumsIt);
      overall += sum;
      *++sumIt += sum;
    }
  while (++hyperplaneIt != hyperplaneEnd);
  return overall;
}

long long VertexTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  long long overall = 0;
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = ++dimensionIt->begin();
  do
    {
      int sum = 0;
      hyperplanes[*idIt]->sumOnPattern(dimensionIt, idIt, sum);
      overall += sum;
    }
  while (++idIt != idEnd);
  return overall;
}

void VertexTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const
{
  reset(firstCandidateElementIt, sumsOnHyperplanes);
  // Nothing to sum on the hyperplanes before hyperplanes[nSetBegin->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsOnHyperplanes.front().begin();
  if (nSetBegin->front() != *firstCandidateElementIt)
    {
      hyperplanes[nSetBegin->front()]->increaseSumsOnHyperplanes(nSetBegin + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += nSetBegin->front();
  const vector<unsigned int>::const_iterator presentVertexEnd = nSetBegin->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = nSetBegin->begin();
    hyperplaneIt += *presentVertexIt++;
    const vector<vector<int>>::iterator sumsIt = sumsOnHyperplanes.begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(nSetBegin, presentVertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->sumsOnHyperplanes(nSetBegin, firstCandidateElementIt, presentVertexIt, sumsIt);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(nSetBegin, presentVertexEnd, *++sumIt);
    }
}

int VertexTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(dimensionIt, firstCandidateElementIt, presentVertexIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

int VertexTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, firstCandidateElementIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

int VertexTrie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->decreaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, firstCandidateElementIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt -= sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

// sumsOnPatternAndHyperplanes but calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int VertexTrie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, firstCandidateElementIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

void VertexTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->sumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, firstCandidateElementIt, sumsIt);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
}

void VertexTrie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // Added element is not a vertex, hence increasedDimensionId != 0 and n > 2
  vector<unsigned int>& increasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[increasedDimensionId];
  vector<unsigned int> elementDimension {element};
  elementDimension.swap(increasedDimension);
  vector<int>& unchangedSums = sums[increasedDimensionId];
  vector<int> empty;
  empty.swap(unchangedSums);
  const vector<unsigned int>::const_iterator firstCandidateElementBegin = firstCandidateElements.begin();
  vector<int>::iterator sumIt = sums.front().begin() + *firstCandidateElementBegin;
  const vector<vector<unsigned int>>::const_iterator dimensionBegin = nSet.begin();
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionBegin->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (dimensionBegin->front() != *firstCandidateElementBegin)
    {
      hyperplanes[dimensionBegin->front()]->increaseSumsOnHyperplanes(dimensionBegin + 1, *firstCandidateElementBegin, sumIt);
    }
  sumIt += dimensionBegin->front();
  const vector<unsigned int>::const_iterator presentVertexEnd = dimensionBegin->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionBegin->begin();
    hyperplaneIt += *presentVertexIt++;
    const vector<vector<int>>::iterator sumsBegin = sums.begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionBegin, presentVertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(dimensionBegin, presentVertexIt, firstCandidateElementBegin, sumsBegin);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionBegin, presentVertexEnd, *++sumIt);
    }
  unchangedSums = std::move(empty);
  increasedDimension = std::move(elementDimension);
}

int VertexTrie::increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (*vertexIt != *firstCandidateElementIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(nextDimensionIt, *firstCandidateElementIt, sumIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->sumsOnPatternAndHyperplanes(dimensionIt, vertexIt, firstCandidateElementIt, sumsIt);
	}
      sumIt +=*vertexIt;
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextDimensionIt, *vertexIt, nextFirstCandidateElementIt, nextSumsIt);
	*++sumIt += sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

void VertexTrie::addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // Added element is a vertex, hence increasedDimensionId == 0
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because *vertexIt cannot be followed from them
  vector<int>::iterator sumIt = sums.front().begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  if (*vertexIt != firstCandidateElements.front())
    {
      if (vertexIt == dimensionIt->begin())
	{
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(dimensionIt + 1, firstCandidateElements.front(), sumIt);
	}
      else
	{
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(dimensionIt, vertexIt, firstCandidateElements.begin(), sums.begin());
	}
    }
  sumIt += *vertexIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    ++dimensionIt;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = ++firstCandidateElements.begin();
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, *vertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(dimensionIt, *vertexIt, nextFirstCandidateElementIt, nextSumsIt);
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, *vertexIt, *++sumIt);
    }
}

// minusSumsOnPatternAndHyperplanes but calling recursivey decreaseSumsOnPatternAndHyperplanes instead of minusSumsOnPatternAndHyperplanes
int VertexTrie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->decreaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, firstCandidateElementIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt -= sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

void VertexTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front() != *firstCandidateElementIt)
    {
      hyperplanes[dimensionIt->front()]->decreaseSumsOnHyperplanes(dimensionIt + 1, *firstCandidateElementIt, sumIt);
    }
  sumIt += dimensionIt->front();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, firstCandidateElementIt, sumsIt);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
}

void VertexTrie::removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // !is01
  // Added element is not a vertex, hence increasedDimensionId != 0 and n > 2
  vector<unsigned int>& decreasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[decreasedDimensionId];
  vector<unsigned int> elementDimension {element};
  elementDimension.swap(decreasedDimension);
  vector<int>& unchangedSums = sums[decreasedDimensionId];
  vector<int> empty;
  empty.swap(unchangedSums);
  const vector<unsigned int>::const_iterator firstCandidateElementBegin = firstCandidateElements.begin();
  vector<int>::iterator sumIt = sums.front().begin() + *firstCandidateElementBegin;
  const vector<vector<unsigned int>>::const_iterator dimensionBegin = nSet.begin();
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionBegin->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (dimensionBegin->front() != *firstCandidateElementBegin)
    {
      hyperplanes[dimensionBegin->front()]->decreaseSumsOnHyperplanes(dimensionBegin + 1, *firstCandidateElementBegin, sumIt);
    }
  sumIt += dimensionBegin->front();
  const vector<unsigned int>::const_iterator presentVertexEnd = dimensionBegin->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionBegin->begin();
    hyperplaneIt += *presentVertexIt++;
    const vector<vector<int>>::iterator sumsBegin = sums.begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionBegin, presentVertexIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(dimensionBegin, presentVertexIt, firstCandidateElementBegin, sumsBegin);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionBegin, presentVertexEnd, *++sumIt);
    }
  unchangedSums = std::move(empty);
  decreasedDimension = std::move(elementDimension);
}

int VertexTrie::decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (*vertexIt != *firstCandidateElementIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(nextDimensionIt, *firstCandidateElementIt, sumIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->minusSumsOnPatternAndHyperplanes(dimensionIt, vertexIt, firstCandidateElementIt, sumsIt);
	}
      sumIt += *vertexIt;
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = firstCandidateElementIt + 1;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextDimensionIt, *vertexIt, nextFirstCandidateElementIt, nextSumsIt);
	*++sumIt -= sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

void VertexTrie::removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums) const
{
  // Added element is a vertex, hence decreasedDimensionId == 0
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because *vertexIt cannot be followed from them
  vector<int>::iterator sumIt = sums.front().begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  if (*vertexIt != firstCandidateElements.front())
    {
      if (vertexIt == dimensionIt->begin())
	{
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(dimensionIt + 1, firstCandidateElements.front(), sumIt);
	}
      else
	{
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(dimensionIt, vertexIt, firstCandidateElements.begin(), sums.begin());
	}
    }
  sumIt += *vertexIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    ++dimensionIt;
    const vector<unsigned int>::const_iterator nextFirstCandidateElementIt = ++firstCandidateElements.begin();
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, *vertexIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(dimensionIt, *vertexIt, nextFirstCandidateElementIt, nextSumsIt);
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, *vertexIt, *++sumIt);
    }
}

void VertexTrie::increaseSumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  // The new candidates not being vertices, they are of some subsequent dimension; follow the present vertices of this dimension
  vector<unsigned int>::const_iterator presentElementIdIt = ++dimensionIt->begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateHyperplanes(dimensionIt, presentElementIdIt, newFirstCandidateElement, oldFirstCandidateElement, sums);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

void VertexTrie::increaseSumsOnNewCandidateVertices(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sums) const
{
  // The new candidates are of this dimension; increase the related sums
  vector<unsigned int>::const_iterator presentElementIdIt = dimensionIt->begin();
  do
    {
      hyperplanes[*presentElementIdIt]->increaseSumsOnNewCandidateElementsOfThisDimension(dimensionIt, newFirstCandidateVertex, oldFirstCandidateVertex, sums);
    }
  while (++presentElementIdIt != presentElementIdEnd);
}

#else

void VertexTrie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // Added element is not a vertex, hence increasedDimensionId != 0 and n > 2
  vector<unsigned int>& increasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[increasedDimensionId];
  vector<unsigned int> elementDimension {element};
  elementDimension.swap(increasedDimension);
  vector<int>& unchangedSums = sums[increasedDimensionId];
  vector<int> empty;
  empty.swap(unchangedSums);
  vector<int>::iterator sumIt = sums.front().begin();
  const vector<vector<unsigned int>>::const_iterator dimensionBegin = nSet.begin();
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionBegin->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (dimensionBegin->front())
    {
      hyperplanes[dimensionBegin->front()]->increaseSumsOnHyperplanes(dimensionBegin + 1, sumIt);
    }
  sumIt += dimensionBegin->front();
  const vector<unsigned int>::const_iterator presentVertexEnd = dimensionBegin->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionBegin->begin();
    hyperplaneIt += *presentVertexIt++;
    const vector<vector<int>>::iterator sumsBegin = sums.begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionBegin, presentVertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(dimensionBegin, presentVertexIt, sumsBegin);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionBegin, presentVertexEnd, *++sumIt);
    }
  unchangedSums = std::move(empty);
  increasedDimension = std::move(elementDimension);
}

void VertexTrie::removeElement(const unsigned int decreasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // !is01
  // Added element is not a vertex, hence increasedDimensionId != 0 and n > 2
  vector<unsigned int>& decreasedDimension = const_cast<vector<vector<unsigned int>>&>(nSet)[decreasedDimensionId];
  vector<unsigned int> elementDimension {element};
  elementDimension.swap(decreasedDimension);
  vector<int>& unchangedSums = sums[decreasedDimensionId];
  vector<int> empty;
  empty.swap(unchangedSums);
  vector<int>::iterator sumIt = sums.front().begin();
  const vector<vector<unsigned int>>::const_iterator dimensionBegin = nSet.begin();
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionBegin->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (dimensionBegin->front())
    {
      hyperplanes[dimensionBegin->front()]->decreaseSumsOnHyperplanes(dimensionBegin + 1, sumIt);
    }
  sumIt += dimensionBegin->front();
  const vector<unsigned int>::const_iterator presentVertexEnd = dimensionBegin->end();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionBegin->begin();
    hyperplaneIt += *presentVertexIt++;
    const vector<vector<int>>::iterator sumsBegin = sums.begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionBegin, presentVertexIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(dimensionBegin, presentVertexIt, sumsBegin);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionBegin, presentVertexEnd, *++sumIt);
    }
  unchangedSums = std::move(empty);
  decreasedDimension = std::move(elementDimension);
}

void VertexTrie::addVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // Added element is a vertex, hence increasedDimensionId == 0
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because *vertexIt cannot be followed from them
  vector<int>::iterator sumIt = sums.front().begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  if (*vertexIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
	}
      else
	{
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(dimensionIt, vertexIt, sums.begin());
	}
    }
  sumIt += *vertexIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    ++dimensionIt;
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, *vertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(dimensionIt, *vertexIt, nextSumsIt);
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, *vertexIt, *++sumIt);
    }
}

void VertexTrie::removeVertex(const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums) const
{
  // Added element is a vertex, hence decreasedDimensionId == 0
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because *vertexIt cannot be followed from them
  vector<int>::iterator sumIt = sums.front().begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  if (*vertexIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
	}
      else
	{
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(dimensionIt, vertexIt, sums.begin());
	}
    }
  sumIt += *vertexIt;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    ++dimensionIt;
    const vector<vector<int>>::iterator nextSumsIt = ++sums.begin();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, *vertexIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(dimensionIt, *vertexIt, nextSumsIt);
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, *vertexIt, *++sumIt);
    }
}

// sumsOnPatternAndHyperplanes but calling recursivey increaseSumsOnPatternAndHyperplanes instead of sumsOnPatternAndHyperplanes
int VertexTrie::increaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front())
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

void VertexTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front())
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
    }
  sumIt += dimensionIt->front();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->sumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, sumsIt);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
}

// minusSumsOnPatternAndHyperplanes but calling recursivey decreaseSumsOnPatternAndHyperplanes instead of minusSumsOnPatternAndHyperplanes
int VertexTrie::decreaseSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front())
    {
      hyperplanes[dimensionIt->front()]->decreaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt -= sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

void VertexTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front())
    {
      hyperplanes[dimensionIt->front()]->decreaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
    }
  sumIt += dimensionIt->front();
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, sumsIt);
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
}

int VertexTrie::increaseSumsOnAddedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (*vertexIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->increaseSumsOnHyperplanes(nextDimensionIt, sumIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->sumsOnPatternAndHyperplanes(dimensionIt, vertexIt, sumsIt);
	}
      sumIt +=*vertexIt;
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->increaseSumsOnPatternAndHyperplanes(nextDimensionIt, *vertexIt, nextSumsIt);
	*++sumIt += sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->sumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

int VertexTrie::decreaseSumsOnRemovedVertexAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[*vertexIt], because vertex cannot be followed from them
  int sumOnPattern;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  if (*vertexIt)
    {
      if (vertexIt == dimensionIt->begin())
	{
	  sumOnPattern = 0;
	  hyperplanes[*vertexIt]->decreaseSumsOnHyperplanes(nextDimensionIt, sumIt);
	}
      else
	{
	  sumOnPattern = hyperplanes[*vertexIt]->minusSumsOnPatternAndHyperplanes(dimensionIt, vertexIt, sumsIt);
	}
      sumIt += *vertexIt;
    }
  else
    {
      sumOnPattern = 0;
    }
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *vertexIt;
    const vector<vector<int>>::iterator nextSumsIt = sumsIt + 1;
    const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
    for (vector<unsigned int>::const_iterator presentVertexIt = vertexIt; ++presentVertexIt != presentVertexEnd; )
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->decreaseSumsOnPatternAndHyperplanes(nextDimensionIt, *vertexIt, nextSumsIt);
	*++sumIt -= sum;
	sumOnPattern += sum;
      }
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator end = hyperplanes.end(); ++hyperplaneIt != end; )
    {
      (*hyperplaneIt)->minusSumOnPattern(nextDimensionIt, *vertexIt, *++sumIt);
    }
  return sumOnPattern;
}

int VertexTrie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front())
    {
      hyperplanes[dimensionIt->front()]->decreaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt -= sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}
#endif

#if defined ASSERT || !defined PRUNE
int VertexTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentVertexEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // Nothing to sum on the hyperplanes before hyperplanes[dimensionIt->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsIt->begin();
  if (dimensionIt->front())
    {
      hyperplanes[dimensionIt->front()]->increaseSumsOnHyperplanes(dimensionIt + 1, sumIt);
    }
  sumIt += dimensionIt->front();
  int sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
    hyperplaneIt += *presentVertexIt++;
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnPatternAndHyperplanes(dimensionIt, presentVertexIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(dimensionIt, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}

long long VertexTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const
{
  reset(sumsOnHyperplanes);
  // Nothing to sum on the hyperplanes before hyperplanes[nSetBegin->front()], because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  vector<int>::iterator sumIt = sumsOnHyperplanes.front().begin();
  if (nSetBegin->front())
    {
      hyperplanes[nSetBegin->front()]->increaseSumsOnHyperplanes(nSetBegin + 1, sumIt);
    }
  sumIt += nSetBegin->front();
  const vector<unsigned int>::const_iterator presentVertexEnd = nSetBegin->end();
  long long sumOnPattern = 0;
  vector<AbstractTrie*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    const vector<AbstractTrie*>::const_iterator hyperplaneBegin = hyperplaneIt;
    vector<unsigned int>::const_iterator presentVertexIt = nSetBegin->begin();
    hyperplaneIt += *presentVertexIt++;
    const vector<vector<int>>::iterator sumsIt = sumsOnHyperplanes.begin();
    do
      {
	for (const vector<AbstractTrie*>::const_iterator end = hyperplaneBegin + *presentVertexIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(nSetBegin, presentVertexIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnPatternAndHyperplanes(nSetBegin, presentVertexIt, sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentVertexIt != presentVertexEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<AbstractTrie*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(nSetBegin, presentVertexEnd, *++sumIt);
    }
  return sumOnPattern;
}
#endif
