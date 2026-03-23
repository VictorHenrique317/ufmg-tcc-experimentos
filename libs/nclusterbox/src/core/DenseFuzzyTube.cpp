// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "DenseFuzzyTube.h"

DenseFuzzyTube::DenseFuzzyTube(vector<double>::const_iterator& membershipIt, const int unit, const unsigned int size): tube()
{
  tube.reserve(size);
  const vector<double>::const_iterator tubeEnd = membershipIt + size;
  do
    {
      tube.push_back(unit * *membershipIt);
    }
  while (++membershipIt != tubeEnd);
}

DenseFuzzyTube::DenseFuzzyTube(const vector<pair<unsigned int, int>>& sparseTube, const int defaultMembership, const unsigned int size): tube(size, defaultMembership)
{
  for (const pair<unsigned int, int>& entry : sparseTube)
    {
      tube[entry.first] = entry.second;
    }
}

int DenseFuzzyTube::at(const unsigned int element) const
{
  return tube[element];
}

void DenseFuzzyTube::set(const unsigned int element, const int membership)
{
  tube[element] = membership;
}

void DenseFuzzyTube::sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const
{
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      sum += tube[*idIt];
    }
  while (++idIt != presentElementIdEnd);
}

void DenseFuzzyTube::minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const
{
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      sum -= tube[*idIt];
    }
  while (++idIt != presentElementIdEnd);
}

#ifdef PRUNE
int DenseFuzzyTube::increasePositiveMemberships(vector<int>& sums) const
{
  int overall = 0;
  vector<int>::iterator sumIt = sums.begin();
  for (const int membership : tube)
    {
      if (membership > 0)
	{
	  overall += membership;
	  *sumIt += membership;
	}
      ++sumIt;
    }
  return overall;
}

int DenseFuzzyTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const
{
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sums.begin() + firstCandidateElement;
  vector<int>::const_iterator tubeIt = tube.begin();
  {
    // Hyperplanes until the last present one
    const vector<int>::const_iterator tubeBegin = tubeIt;
    tubeIt += firstCandidateElement;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    do
      {
	for (const vector<int>::const_iterator end = tubeBegin + *presentElementIdIt; tubeIt != end; ++tubeIt)
	  {
	    *sumIt++ += *tubeIt;
	  }
	*sumIt++ += *tubeIt;
	sumOnPattern += *tubeIt++;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<int>::const_iterator tubeEnd = tube.end(); tubeIt != tubeEnd; ++tubeIt)
    {
      *sumIt++ += *tubeIt;
    }
  return sumOnPattern;
}

int DenseFuzzyTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const
{
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sums.begin() + firstCandidateElement;
  vector<int>::const_iterator tubeIt = tube.begin();
  {
    // Hyperplanes until the last present one
    const vector<int>::const_iterator tubeBegin = tubeIt;
    tubeIt += firstCandidateElement;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    do
      {
	for (const vector<int>::const_iterator end = tubeBegin + *presentElementIdIt; tubeIt != end; ++tubeIt)
	  {
	    *sumIt++ -= *tubeIt;
	  }
	*sumIt++ -= *tubeIt;
	sumOnPattern += *tubeIt++;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<int>::const_iterator tubeEnd = tube.end(); tubeIt != tubeEnd; ++tubeIt)
    {
      *sumIt++ -= *tubeIt;
    }
  return sumOnPattern;
}

void DenseFuzzyTube::increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const
{
  vector<int>::iterator sumIt = sums.begin() + firstCandidateElement;
  const vector<int>::const_iterator tubeEnd = tube.end();
  vector<int>::const_iterator tubeIt = tube.begin() + firstCandidateElement;
  do
    {
      *sumIt++ += *tubeIt;
    }
  while (++tubeIt != tubeEnd);
}

void DenseFuzzyTube::decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const
{
  vector<int>::iterator sumIt = sums.begin() + firstCandidateElement;
  const vector<int>::const_iterator tubeEnd = tube.end();
  vector<int>::const_iterator tubeIt = tube.begin() + firstCandidateElement;
  do
    {
      *sumIt++ -= *tubeIt;
    }
  while (++tubeIt != tubeEnd);
}

void DenseFuzzyTube::increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  vector<int>::iterator sumIt = sums.begin() + newFirstCandidateElement;
  const vector<int>::const_iterator tubeEnd = tube.begin() + oldFirstCandidateElement;
  vector<int>::const_iterator tubeIt = tube.begin() + newFirstCandidateElement;
  do
    {
      *sumIt++ += *tubeIt;
    }
  while (++tubeIt != tubeEnd);
}

#else

void DenseFuzzyTube::decreaseSumsOnHyperplanes(vector<int>& sums) const
{
  vector<int>::iterator sumIt = sums.begin();
  for (const int membership : tube)
    {
      *sumIt++ -= membership;
    }
}

int DenseFuzzyTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const
{
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sums.begin();
  vector<int>::const_iterator tubeIt = tube.begin();
  {
    // Hyperplanes until the last present one
    const vector<int>::const_iterator tubeBegin = tubeIt;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    do
      {
	for (const vector<int>::const_iterator end = tubeBegin + *presentElementIdIt; tubeIt != end; ++tubeIt)
	  {
	    *sumIt++ -= *tubeIt;
	  }
	*sumIt++ -= *tubeIt;
	sumOnPattern += *tubeIt++;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<int>::const_iterator tubeEnd = tube.end(); tubeIt != tubeEnd; ++tubeIt)
    {
      *sumIt++ -= *tubeIt;
    }
  return sumOnPattern;
}
#endif

#if defined ASSERT || !defined PRUNE
int DenseFuzzyTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const
{
  int sumOnPattern = 0;
  vector<int>::iterator sumIt = sums.begin();
  vector<int>::const_iterator tubeIt = tube.begin();
  {
    // Hyperplanes until the last present one
    const vector<int>::const_iterator tubeBegin = tubeIt;
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    do
      {
	for (const vector<int>::const_iterator end = tubeBegin + *presentElementIdIt; tubeIt != end; ++tubeIt)
	  {
	    *sumIt++ += *tubeIt;
	  }
	*sumIt++ += *tubeIt;
	sumOnPattern += *tubeIt++;
      }
    while (++presentElementIdIt != presentElementIdEnd);
  }
  // Hyperplanes after the last present one
  for (const vector<int>::const_iterator tubeEnd = tube.end(); tubeIt != tubeEnd; ++tubeIt)
    {
      *sumIt++ += *tubeIt;
    }
  return sumOnPattern;
}

void DenseFuzzyTube::increaseSumsOnHyperplanes(vector<int>& sums) const
{
  vector<int>::iterator sumIt = sums.begin();
  for (const int membership : tube)
    {
      *sumIt++ += membership;
    }
}
#endif
