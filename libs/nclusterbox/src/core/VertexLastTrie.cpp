// Copyright 2024-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VertexLastTrie.h"

#include "SparseCrispTube.h"
#include "SparseFuzzyTubeWithSize.h"

VertexLastTrie::VertexLastTrie(): LastTrie()
{
}

VertexLastTrie::VertexLastTrie(const unsigned int cardinality): LastTrie()
{
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  if (is01)
    {
      unsigned int nbOfMissingTubes = cardinality - 1;
      do
	{
	  hyperplanes.push_back(new SparseCrispTube());
	}
      while (--nbOfMissingTubes);
      return;
    }
  unsigned int size = 1;
  do
    {
      hyperplanes.push_back(new SparseFuzzyTubeWithSize(size));
    }
  while (++size != cardinality);
}

VertexLastTrie::VertexLastTrie(vector<double>::const_iterator& membershipIt, const unsigned int cardinality): LastTrie()
{
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  do
    {
      hyperplanes.push_back(new DenseCrispTube(membershipIt, nbOfVerticesBefore));
    }
  while (++nbOfVerticesBefore != cardinality);
}

VertexLastTrie::VertexLastTrie(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int cardinality): LastTrie()
{
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  do
    {
      hyperplanes.push_back(new DenseFuzzyTube(membershipIt, unit, nbOfVerticesBefore));
    }
  while (++nbOfVerticesBefore != cardinality);
}

void VertexLastTrie::deleteHyperplanes()
{
  const vector<Tube*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      delete *hyperplaneIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void VertexLastTrie::setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension)
{
  // is01
  Tube*& hyperplane = hyperplanes[*idIt];
  if (hyperplane->isFullSparseTube(*idIt))
    {
      DenseCrispTube* newHyperplane = static_cast<SparseCrispTube*>(hyperplane)->getDenseRepresentation(*idIt);
      delete hyperplane;
      hyperplane = newHyperplane;
    }
  hyperplane->set(*(idIt + 1));
}

void VertexLastTrie::setTuple(const vector<unsigned int>::const_iterator idIt, const unsigned int cardinalityOfLastDimension, const int membership)
{
  // !is01
  Tube*& hyperplane = hyperplanes[*idIt];
  if (hyperplane->isFullSparseTube(*idIt))
    {
      DenseFuzzyTube* newHyperplane = static_cast<SparseFuzzyTube*>(hyperplane)->getDenseRepresentation(*idIt);
      delete hyperplane;
      hyperplane = newHyperplane;
    }
  hyperplane->set(*(idIt + 1), membership);
}

void VertexLastTrie::sortTubes()
{
  const vector<Tube*>::iterator hyperplaneEnd = hyperplanes.end();
  for (vector<Tube*>::iterator hyperplaneIt = hyperplanes.begin() + 2; hyperplaneIt != hyperplaneEnd; ++hyperplaneIt)
    {
      (*hyperplaneIt)->sortTubes();
    }
}

void VertexLastTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt]->sumOnPattern(idBegin, idIt, sum);
    }
  while (++idIt != idEnd);
}

void VertexLastTrie::minusSumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt]->minusSumOnPattern(idBegin, idIt, sum);
    }
  while (++idIt != idEnd);
}

#ifdef PRUNE
void VertexLastTrie::positiveMembershipsOnHyperplanes(const vector<vector<int>>::iterator sumsIt) const
{
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      *++sumIt += (*hyperplaneIt)->increasePositiveMemberships(*sumsIt);
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

int VertexLastTrie::increasePositiveMemberships(const vector<vector<int>>::iterator sumsIt) const
{
  int overall = 0;
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end();
  vector<Tube*>::const_iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      int sum = (*hyperplaneIt)->increasePositiveMemberships(*sumsIt);
      overall += sum;
      *++sumIt += sum;
    }
  while (++hyperplaneIt != hyperplaneEnd);
  return overall;
}

long long VertexLastTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator dimensionIt) const
{
  long long overall = 0;
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      int sum = 0;
      hyperplanes[*idIt]->sumOnPattern(idBegin, idIt, sum);
      overall += sum;
    }
  while (++idIt != idEnd);
  return overall;
}

void VertexLastTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sumsOnHyperplanes) const
{
  // n = 2
  vector<int>& sums = sumsOnHyperplanes.front();
  fill(sums.begin() + *firstCandidateElementIt, sums.end(), 0);
  const vector<unsigned int>::const_iterator presentElementIdBegin = nSetBegin->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin != *firstCandidateElementIt)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(*firstCandidateElementIt, sums);
    }
  const vector<unsigned int>::const_iterator presentElementIdEnd = nSetBegin->end();
  vector<int>::iterator sumIt = sums.begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *firstCandidateElementIt, sums);
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
}

int VertexLastTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin != *firstCandidateElementIt)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *firstCandidateElementIt, *sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
  return sumOnPattern;
}

void VertexLastTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin != *firstCandidateElementIt)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
    }
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *firstCandidateElementIt, *sumsIt);
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
}

void VertexLastTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin != *firstCandidateElementIt)
    {
      hyperplanes[*presentElementIdBegin]->decreaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
    }
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *firstCandidateElementIt, *sumsIt);
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
}

int VertexLastTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin != *firstCandidateElementIt)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *firstCandidateElementIt, *sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
  return sumOnPattern;
}

int VertexLastTrie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<unsigned int>::const_iterator firstCandidateElementIt, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin != *firstCandidateElementIt)
    {
      hyperplanes[*presentElementIdBegin]->decreaseSumsOnHyperplanes(*firstCandidateElementIt, *sumsIt);
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *firstCandidateElementIt, *sumsIt);
	sumOnPattern += sum;
	*++sumIt -= sum;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
  return sumOnPattern;
}

#else

void VertexLastTrie::increaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // sumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(*sumsIt);
    }
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	*++sumIt += (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *sumsIt);
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
}

void VertexLastTrie::decreaseSumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  // minusSumsOnPatternAndHyperplanes without computing the sumOnPattern
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin)
    {
      hyperplanes[*presentElementIdBegin]->decreaseSumsOnHyperplanes(*sumsIt);
    }
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	*++sumIt -= (*hyperplaneIt)->minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *sumsIt);
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
}

int VertexLastTrie::minusSumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin)
    {
      hyperplanes[*presentElementIdBegin]->decreaseSumsOnHyperplanes(*sumsIt);
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *sumsIt);
	sumOnPattern += sum;
	*++sumIt -= sum;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->minusSumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
  return sumOnPattern;
}
#endif

#if defined ASSERT || !defined PRUNE
int VertexLastTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<vector<int>>::iterator sumsIt) const
{
  const vector<unsigned int>::const_iterator presentElementIdBegin = dimensionIt->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(*sumsIt);
    }
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sumsIt->begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, *sumsIt);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
  return sumOnPattern;
}

long long VertexLastTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sumsOnHyperplanes) const
{
  // n = 2
  vector<int>& sums = sumsOnHyperplanes.front();
  fill(sums.begin(), sums.end(), 0);
  const vector<unsigned int>::const_iterator presentElementIdBegin = nSetBegin->begin();
  // Nothing to sum on the hyperplanes before *(hyperplanesBegin + *presentElementIdBegin), because they all relate to absent elements and so do the hyperplanes before the one that would be followed (the only ones stored)
  if (*presentElementIdBegin)
    {
      hyperplanes[*presentElementIdBegin]->increaseSumsOnHyperplanes(sums);
    }
  const vector<unsigned int>::const_iterator presentElementIdEnd = nSetBegin->end();
  long long sumOnPattern = 0;
  vector<int>::iterator sumIt = sums.begin() + *presentElementIdBegin;
  vector<Tube*>::const_iterator hyperplaneIt = hyperplanes.begin();
  {
    // Hyperplanes until the last present one
    const vector<Tube*>::const_iterator hyperplaneBegin = hyperplaneIt;
    hyperplaneIt += *presentElementIdBegin;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin + 1;
    do
      {
	for (const vector<Tube*>::const_iterator end = hyperplaneBegin + *presentElementIdIt; ++hyperplaneIt != end; )
	  {
	    (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdIt, *++sumIt);
	  }
	const int sum = (*hyperplaneIt)->sumsOnHyperplanes(presentElementIdBegin, presentElementIdIt, sums);
	sumOnPattern += sum;
	*++sumIt += sum;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<Tube*>::const_iterator hyperplaneEnd = hyperplanes.end(); ++hyperplaneIt != hyperplaneEnd; )
    {
      (*hyperplaneIt)->sumOnPattern(presentElementIdBegin, presentElementIdEnd, *++sumIt);
    }
  return sumOnPattern;
}
#endif
