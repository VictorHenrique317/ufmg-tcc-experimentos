// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseFuzzyTube.h"

#include <algorithm>

int SparseFuzzyTube::defaultMembership;
double SparseFuzzyTube::densityThreshold;

SparseFuzzyTube::SparseFuzzyTube(): tube()
{
}

bool SparseFuzzyTube::isFullSparseTube(const unsigned int size) const
{
  return tube.size() == static_cast<unsigned int>(size * densityThreshold);
}

int SparseFuzzyTube::at(const unsigned int element) const
{
  const vector<pair<unsigned int, int>>::const_iterator it = lower_bound(tube.begin(), tube.end(), element, [](const pair<unsigned int, int>& entry, const unsigned int id) {return entry.first < id;});
  if (it == tube.end() || it->first != element)
    {
      return defaultMembership;
    }
  return it->second;
}

void SparseFuzzyTube::set(const unsigned int element, const int membership)
{
  tube.emplace_back(element, membership);
}

DenseFuzzyTube* SparseFuzzyTube::getDenseRepresentation(const unsigned int size) const
{
  return new DenseFuzzyTube(tube, defaultMembership, size);
}

void SparseFuzzyTube::sortTubes()
{
  tube.shrink_to_fit();
  sort(tube.begin(), tube.end(), [](const pair<unsigned int, int>& entry1, const pair<unsigned int, int>& entry2) {return entry1.first < entry2.first;});
}

void SparseFuzzyTube::sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const
{
  unsigned int nbOfDefaultMemberships = 0;
  const vector<pair<unsigned int, int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, int>>::const_iterator tubeBegin = tube.begin();
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt, [](const pair<unsigned int, int>& entry, const unsigned int id) {return entry.first < id;});
      if (tubeBegin == tubeEnd)
	{
	  sum += static_cast<int>(presentElementIdEnd - idIt + nbOfDefaultMemberships) * defaultMembership;
	  return;
	}
      if (tubeBegin->first == *idIt)
	{
	  sum += tubeBegin->second;
	}
      else
	{
	  ++nbOfDefaultMemberships;
	}
    }
  while (++idIt != presentElementIdEnd);
  sum += static_cast<int>(nbOfDefaultMemberships) * defaultMembership;
}

void SparseFuzzyTube::minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const
{
  unsigned int nbOfDefaultMemberships = 0;
  const vector<pair<unsigned int, int>>::const_iterator tubeEnd = tube.end();
  vector<pair<unsigned int, int>>::const_iterator tubeBegin = tube.begin();
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      tubeBegin = lower_bound(tubeBegin, tubeEnd, *idIt, [](const pair<unsigned int, int>& entry, const unsigned int id) {return entry.first < id;});
      if (tubeBegin == tubeEnd)
	{
	  sum -= static_cast<int>(presentElementIdEnd - idIt + nbOfDefaultMemberships) * defaultMembership;
	  return;
	}
      if (tubeBegin->first == *idIt)
	{
	  sum -= tubeBegin->second;
	}
      else
	{
	  ++nbOfDefaultMemberships;
	}
    }
  while (++idIt != presentElementIdEnd);
  sum -= static_cast<int>(nbOfDefaultMemberships) * defaultMembership;
}

#ifdef PRUNE
int SparseFuzzyTube::increasePositiveMemberships(vector<int>& sums) const
{
  int sum = 0;
  for (const pair<unsigned int, int>& entry : tube)
    {
      if (entry.second > 0)
	{
	  sums[entry.first] += entry.second;
	  sum += entry.second;
	}
    }
  return sum;
}

int SparseFuzzyTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end();
  vector<pair<unsigned int, int>>::const_iterator entryIt = lower_bound(tube.begin(), entryEnd, firstCandidateElement, [](const pair<unsigned int, int>& entry1, const unsigned int id) {return entry1.first < id;});
  if (entryIt == entryEnd)
    {
      vector<int>::iterator sumIt = sumBegin + firstCandidateElement;
      addDefaultMembership(sumIt, sumEnd);
      return static_cast<int>(presentElementIdEnd - presentElementIdBegin) * defaultMembership;
    }
  vector<int>::iterator sumIt = sumBegin + firstCandidateElement;
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back().first); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; entryIt->first < *presentElementIdIt; ++entryIt)
	  {
	    addDefaultMembership(sumIt, sumBegin + entryIt->first);
	    *sumIt++ += entryIt->second;
	  }
	addDefaultMembership(sumIt, sumBegin + *presentElementIdIt);
	if (entryIt->first == *presentElementIdIt)
	  {
	    *sumIt++ += entryIt->second;
	    sumOnPattern += entryIt->second;
	    ++entryIt;
	  }
	else
	  {
	    *sumIt++ += defaultMembership;
	    sumOnPattern += defaultMembership;
	  }
      }
    sumOnPattern += static_cast<int>(presentElementIdEnd - presentElementIdIt) * defaultMembership;
  }
  for (; entryIt != entryEnd; ++entryIt)
    {
      addDefaultMembership(sumIt, sumBegin + entryIt->first);
      *sumIt++ += entryIt->second;
    }
  addDefaultMembership(sumIt, sumEnd);
  return sumOnPattern;
}

int SparseFuzzyTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const
{
  return sumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, firstCandidateElement, sums.begin(), sums.end());
}

int SparseFuzzyTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end();
  vector<pair<unsigned int, int>>::const_iterator entryIt = lower_bound(tube.begin(), entryEnd, firstCandidateElement, [](const pair<unsigned int, int>& entry1, const unsigned int id) {return entry1.first < id;});
  if (entryIt == entryEnd)
    {
      vector<int>::iterator sumIt = sumBegin + firstCandidateElement;
      subtractDefaultMembership(sumIt, sumEnd);
      return static_cast<int>(presentElementIdEnd - presentElementIdBegin) * defaultMembership;
    }
  vector<int>::iterator sumIt = sumBegin + firstCandidateElement;
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back().first); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; entryIt->first < *presentElementIdIt; ++entryIt)
	  {
	    subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
	    *sumIt++ -= entryIt->second;
	  }
	subtractDefaultMembership(sumIt, sumBegin + *presentElementIdIt);
	if (entryIt->first == *presentElementIdIt)
	  {
	    *sumIt++ -= entryIt->second;
	    sumOnPattern += entryIt->second;
	    ++entryIt;
	  }
	else
	  {
	    *sumIt++ -= defaultMembership;
	    sumOnPattern += defaultMembership;
	  }
      }
    sumOnPattern += static_cast<int>(presentElementIdEnd - presentElementIdIt) * defaultMembership;
  }
  for (; entryIt != entryEnd; ++entryIt)
    {
      subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
      *sumIt++ -= entryIt->second;
    }
  subtractDefaultMembership(sumIt, sumEnd);
  return sumOnPattern;
}

int SparseFuzzyTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const
{
  return minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, firstCandidateElement, sums.begin(), sums.end());
}

void SparseFuzzyTube::increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  vector<int>::iterator sumIt = sumBegin;
  {
    const vector<int>::iterator sumBegin = sumIt;
    sumIt += firstCandidateElement;
    const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end();
    for (vector<pair<unsigned int, int>>::const_iterator entryIt = lower_bound(tube.begin(), entryEnd, firstCandidateElement, [](const pair<unsigned int, int>& entry1, const unsigned int id) {return entry1.first < id;}); entryIt != entryEnd; ++entryIt)
      {
	addDefaultMembership(sumIt, sumBegin + entryIt->first);
	*sumIt++ += entryIt->second;
      }
  }
  addDefaultMembership(sumIt, sumEnd);
}

void SparseFuzzyTube::increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const
{
  increaseSumsOnHyperplanes(firstCandidateElement, sums.begin(), sums.end());
}

void SparseFuzzyTube::decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  vector<int>::iterator sumIt = sumBegin;
  {
    const vector<int>::iterator sumBegin = sumIt;
    sumIt += firstCandidateElement;
    const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end();
    for (vector<pair<unsigned int, int>>::const_iterator entryIt = lower_bound(tube.begin(), entryEnd, firstCandidateElement, [](const pair<unsigned int, int>& entry1, const unsigned int id) {return entry1.first < id;}); entryIt != entryEnd; ++entryIt)
      {
	subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
	*sumIt++ -= entryIt->second;
      }
  }
  subtractDefaultMembership(sumIt, sumEnd);
}

void SparseFuzzyTube::decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const
{
  decreaseSumsOnHyperplanes(firstCandidateElement, sums.begin(), sums.end());
}

void SparseFuzzyTube::increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const
{
  const vector<int>::iterator sumBegin = sums.begin();
  vector<int>::iterator sumIt = sumBegin + newFirstCandidateElement;
  {
    const vector<pair<unsigned int, int>>::const_iterator entryEnd = lower_bound(tube.begin(), tube.end(), oldFirstCandidateElement, [](const pair<unsigned int, int>& entry1, const unsigned int id) {return entry1.first < id;});
    for (vector<pair<unsigned int, int>>::const_iterator entryIt = lower_bound(tube.begin(), entryEnd, newFirstCandidateElement, [](const pair<unsigned int, int>& entry1, const unsigned int id) {return entry1.first < id;}); entryIt != entryEnd; ++entryIt)
      {
	addDefaultMembership(sumIt, sumBegin + entryIt->first);
	*sumIt++ += entryIt->second;
      }
  }
  addDefaultMembership(sumIt, sumBegin + oldFirstCandidateElement);
}

#else

void SparseFuzzyTube::decreaseSumsOnHyperplanes(const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  vector<int>::iterator sumIt = sumBegin;
  for (const pair<unsigned int, int>& entry : tube)
    {
      subtractDefaultMembership(sumIt, sumBegin + entry.first);
      *sumIt++ -= entry.second;
    }
  subtractDefaultMembership(sumIt, sumEnd);
}

void SparseFuzzyTube::decreaseSumsOnHyperplanes(vector<int>& sums) const
{
  decreaseSumsOnHyperplanes(sums.begin(), sums.end());
}

int SparseFuzzyTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  vector<int>::iterator sumIt = sumBegin;
  if (tube.empty())
    {
      subtractDefaultMembership(sumIt, sumEnd);
      return static_cast<int>(presentElementIdEnd - presentElementIdBegin) * defaultMembership;
    }
  vector<pair<unsigned int, int>>::const_iterator entryIt = tube.begin();
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back().first); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; entryIt->first < *presentElementIdIt; ++entryIt)
	  {
	    subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
	    *sumIt++ -= entryIt->second;
	  }
	subtractDefaultMembership(sumIt, sumBegin + *presentElementIdIt);
	if (entryIt->first == *presentElementIdIt)
	  {
	    *sumIt++ -= entryIt->second;
	    sumOnPattern += entryIt->second;
	    ++entryIt;
	  }
	else
	  {
	    *sumIt++ -= defaultMembership;
	    sumOnPattern += defaultMembership;
	  }
      }
    sumOnPattern += static_cast<int>(presentElementIdEnd - presentElementIdIt) * defaultMembership;
  }
  for (const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end(); entryIt != entryEnd; ++entryIt)
    {
      subtractDefaultMembership(sumIt, sumBegin + entryIt->first);
      *sumIt++ -= entryIt->second;
    }
  subtractDefaultMembership(sumIt, sumEnd);
  return sumOnPattern;
}

int SparseFuzzyTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const
{
  return minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, sums.begin(), sums.end());
}
#endif

#if defined ASSERT || !defined PRUNE
int SparseFuzzyTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  vector<int>::iterator sumIt = sumBegin;
  if (tube.empty())
    {
      addDefaultMembership(sumIt, sumEnd);
      return static_cast<int>(presentElementIdEnd - presentElementIdBegin) * defaultMembership;
    }
  vector<pair<unsigned int, int>>::const_iterator entryIt = tube.begin();
  int sumOnPattern = 0;
  {
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    for (const vector<unsigned int>::const_iterator end = upper_bound(presentElementIdIt, presentElementIdEnd, tube.back().first); presentElementIdIt != end; ++presentElementIdIt)
      {
	for (; entryIt->first < *presentElementIdIt; ++entryIt)
	  {
	    addDefaultMembership(sumIt, sumBegin + entryIt->first);
	    *sumIt++ += entryIt->second;
	  }
	addDefaultMembership(sumIt, sumBegin + *presentElementIdIt);
	if (entryIt->first == *presentElementIdIt)
	  {
	    *sumIt++ += entryIt->second;
	    sumOnPattern += entryIt->second;
	    ++entryIt;
	  }
	else
	  {
	    *sumIt++ += defaultMembership;
	    sumOnPattern += defaultMembership;
	  }
      }
    sumOnPattern += static_cast<int>(presentElementIdEnd - presentElementIdIt) * defaultMembership;
  }
  for (const vector<pair<unsigned int, int>>::const_iterator entryEnd = tube.end(); entryIt != entryEnd; ++entryIt)
    {
      addDefaultMembership(sumIt, sumBegin + entryIt->first);
      *sumIt++ += entryIt->second;
    }
  addDefaultMembership(sumIt, sumEnd);
  return sumOnPattern;
}

int SparseFuzzyTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const
{
  return sumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, sums.begin(), sums.end());
}

void SparseFuzzyTube::increaseSumsOnHyperplanes(const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const
{
  vector<int>::iterator sumIt = sumBegin;
  for (const pair<unsigned int, int>& entry : tube)
    {
      addDefaultMembership(sumIt, sumBegin + entry.first);
      *sumIt++ += entry.second;
    }
  addDefaultMembership(sumIt, sumEnd);
}

void SparseFuzzyTube::increaseSumsOnHyperplanes(vector<int>& sums) const
{
  increaseSumsOnHyperplanes(sums.begin(), sums.end());
}
#endif

void SparseFuzzyTube::setDefaultMembershipAndDensityLimit(const int defaultMembershipParam, const double densityThresholdParam)
{
  defaultMembership = defaultMembershipParam;
  densityThreshold = densityThresholdParam;
}

void SparseFuzzyTube::addDefaultMembership(vector<int>::iterator& it, const vector<int>::iterator end)
{
  while (it != end)
    {
      *it++ += defaultMembership;
    }
}

void SparseFuzzyTube::subtractDefaultMembership(vector<int>::iterator& it, const vector<int>::iterator end)
{
  while (it != end)
    {
      *it++ -= defaultMembership;
    }
}
