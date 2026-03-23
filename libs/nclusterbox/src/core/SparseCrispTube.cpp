// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseCrispTube.h"

#include <algorithm>

long long SparseCrispTube::defaultMembership;
double SparseCrispTube::densityThreshold;

SparseCrispTube::SparseCrispTube(): tube()
{
}

bool SparseCrispTube::isFullSparseTube(const unsigned int size) const
{
  return tube.size() == static_cast<unsigned int>(size * densityThreshold);
}

int SparseCrispTube::at(const unsigned int element) const
{
  return binary_search(tube.begin(), tube.end(), element);
}

void SparseCrispTube::set(const unsigned int element)
{
  tube.push_back(element);
}

DenseCrispTube* SparseCrispTube::getDenseRepresentation(const unsigned int size) const
{
  return new DenseCrispTube(tube, size);
}

void SparseCrispTube::sortTubes()
{
  tube.shrink_to_fit();
  sort(tube.begin(), tube.end());
}

void SparseCrispTube::sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& nbOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt);
      if (tubeBegin == tubeEnd)
	{
	  return;
	}
      if (*tubeBegin == *idIt)
	{
	  ++nbOfPresentTuples;
	}
    }
  while (++idIt != presentElementIdEnd);
}

void SparseCrispTube::minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& nbOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator tubeEnd = tube.end();
  vector<unsigned int>::const_iterator tubeBegin = tube.begin();
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt);
      if (tubeBegin == tubeEnd)
	{
	  return;
	}
      if (*tubeBegin == *idIt)
	{
	  --nbOfPresentTuples;
	}
    }
  while (++idIt != presentElementIdEnd);
}

#ifdef PRUNE
int SparseCrispTube::increasePositiveMemberships(vector<int>& numbersOfPresentTuples) const
{
  for (const unsigned int elementId : tube)
    {
      ++numbersOfPresentTuples[elementId];
    }
  return tube.size();
}

int SparseCrispTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator idEnd = tube.end();
  vector<unsigned int>::const_iterator idIt = lower_bound(tube.begin(), idEnd, firstCandidateElement);
  if (idIt == idEnd)
    {
      return 0;
    }
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back()); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; *idIt < *presentElementIdIt; ++idIt)
	  {
	    ++numbersOfPresentTuples[*idIt];
	  }
	if (*idIt == *presentElementIdIt)
	  {
	    ++idIt;
	    ++numbersOfPresentTuples[*presentElementIdIt];
	    ++sumOnPattern;
	  }
      }
  }
  for (; idIt != idEnd; ++idIt)
    {
      ++numbersOfPresentTuples[*idIt];
    }
  return sumOnPattern;
}

int SparseCrispTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator idEnd = tube.end();
  vector<unsigned int>::const_iterator idIt = lower_bound(tube.begin(), idEnd, firstCandidateElement);
  if (idIt == idEnd)
    {
      return 0;
    }
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back()); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; *idIt < *presentElementIdIt; ++idIt)
	  {
	    --numbersOfPresentTuples[*idIt];
	  }
	if (*idIt == *presentElementIdIt)
	  {
	    ++idIt;
	    --numbersOfPresentTuples[*presentElementIdIt];
	    ++sumOnPattern;
	  }
      }
  }
  for (; idIt != idEnd; ++idIt)
    {
      --numbersOfPresentTuples[*idIt];
    }
  return sumOnPattern;
}

void SparseCrispTube::increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator idEnd = tube.end();
  for (vector<unsigned int>::const_iterator idIt = lower_bound(tube.begin(), idEnd, firstCandidateElement); idIt != idEnd; ++idIt)
    {
      ++numbersOfPresentTuples[*idIt];
    }
}

void SparseCrispTube::decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator idEnd = tube.end();
  for (vector<unsigned int>::const_iterator idIt = lower_bound(tube.begin(), idEnd, firstCandidateElement); idIt != idEnd; ++idIt)
    {
      --numbersOfPresentTuples[*idIt];
    }
}

void SparseCrispTube::increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  const vector<unsigned int>::const_iterator idEnd = lower_bound(tube.begin(), tube.end(), oldFirstCandidateElement);
  for (vector<unsigned int>::const_iterator idIt = lower_bound(tube.begin(), idEnd, newFirstCandidateElement); idIt != idEnd; ++idIt)
    {
      ++numbersOfPresentTuples[*idIt];
    }
}

#else

void SparseCrispTube::decreaseSumsOnHyperplanes(vector<int>& numbersOfPresentTuples) const
{
  for (const unsigned int elementId : tube)
    {
      --numbersOfPresentTuples[elementId];
    }
}

int SparseCrispTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& numbersOfPresentTuples) const
{
  if (tube.empty())
    {
      return 0;
    }
  vector<unsigned int>::const_iterator idIt = tube.begin();
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back()); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; *idIt < *presentElementIdIt; ++idIt)
	  {
	    --numbersOfPresentTuples[*idIt];
	  }
	if (*idIt == *presentElementIdIt)
	  {
	    ++idIt;
	    --numbersOfPresentTuples[*presentElementIdIt];
	    ++sumOnPattern;
	  }
      }
  }
  for (const vector<unsigned int>::const_iterator idEnd = tube.end(); idIt != idEnd; ++idIt)
    {
      --numbersOfPresentTuples[*idIt];
    }
  return sumOnPattern;
}
#endif

#if defined ASSERT || !defined PRUNE
int SparseCrispTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& numbersOfPresentTuples) const
{
  if (tube.empty())
    {
      return 0;
    }
  vector<unsigned int>::const_iterator idIt = tube.begin();
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back()); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; *idIt < *presentElementIdIt; ++idIt)
	  {
	    ++numbersOfPresentTuples[*idIt];
	  }
	if (*idIt == *presentElementIdIt)
	  {
	    ++idIt;
	    ++numbersOfPresentTuples[*presentElementIdIt];
	    ++sumOnPattern;
	  }
      }
  }
  for (const vector<unsigned int>::const_iterator idEnd = tube.end(); idIt != idEnd; ++idIt)
    {
      ++numbersOfPresentTuples[*idIt];
    }
  return sumOnPattern;
}

void SparseCrispTube::increaseSumsOnHyperplanes(vector<int>& numbersOfPresentTuples) const
{
  for (const unsigned int elementId : tube)
    {
      ++numbersOfPresentTuples[elementId];
    }
}
#endif

long long SparseCrispTube::getDefaultMembership()
{
  return defaultMembership;
}

void SparseCrispTube::setDefaultMembership(const int defaultMembershipParam)
{
  defaultMembership = defaultMembershipParam;
}

void SparseCrispTube::setDefaultMembershipAndDensityLimit(const int defaultMembershipParam, const double densityThresholdParam)
{
  defaultMembership = defaultMembershipParam;
  densityThreshold = densityThresholdParam;
}
