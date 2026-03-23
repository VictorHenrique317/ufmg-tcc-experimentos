// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "DenseCrispTube.h"

DenseCrispTube::DenseCrispTube(vector<double>::const_iterator& membershipIt, const unsigned int size): tube(size)
{
  unsigned int elementId = 0;
  do
    {
      if (*membershipIt > 0)
	{
	  tube.set(elementId);
	}
      ++membershipIt;
    }
  while (++elementId != size);
}

DenseCrispTube::DenseCrispTube(const vector<unsigned int>& sparseTube, const unsigned int size): tube(size)
{
  for (const unsigned int elementId : sparseTube)
    {
      tube.set(elementId);
    }
}

int DenseCrispTube::at(const unsigned int element) const
{
  return tube[element];
}

void DenseCrispTube::set(const unsigned int element)
{
  tube.set(element);
}

void DenseCrispTube::sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& nbOfPresentTuples) const
{
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      if (tube[*idIt])
	{
	  ++nbOfPresentTuples;
	}
    }
  while (++idIt != presentElementIdEnd);
}

void DenseCrispTube::minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& nbOfPresentTuples) const
{
  vector<unsigned int>::const_iterator idIt = presentElementIdBegin;
  do
    {
      if (tube[*idIt])
	{
	  --nbOfPresentTuples;
	}
    }
  while (++idIt != presentElementIdEnd);
}

#ifdef PRUNE
int DenseCrispTube::increasePositiveMemberships(vector<int>& numbersOfPresentTuples) const
{
  int overall = 0;
  dynamic_bitset<>::size_type elementId = tube.find_first();
  do
    {
      ++numbersOfPresentTuples[elementId];
      ++overall;
      elementId = tube.find_next(elementId);
    }
  while (elementId != dynamic_bitset<>::npos);
  return overall;
}

int DenseCrispTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  int sumOnPattern = 0;
  dynamic_bitset<>::size_type elementId = find_first(firstCandidateElement);
  {
    // Hyperplanes until the last present one
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    unsigned int presentElementId = *presentElementIdIt;
    for (; elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
      {
	++numbersOfPresentTuples[elementId];
	if (elementId >= presentElementId)
	  {
	    if (elementId == presentElementId)
	      {
		++sumOnPattern;
		if (++presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
	      }
	    else
	      {
		presentElementIdIt = lower_bound(++presentElementIdIt, presentElementIdEnd, elementId);
		if (presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
		if (elementId == *presentElementIdIt)
		  {
		    ++sumOnPattern;
		    if (++presentElementIdIt == presentElementIdEnd)
		      {
			break;
		      }
		  }
	      }
	    presentElementId = *presentElementIdIt;
	  }
      }
  }
  // Hyperplanes after the last present one
  for (elementId = tube.find_next(elementId); elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
    {
      ++numbersOfPresentTuples[elementId];
    }
  return sumOnPattern;
}

int DenseCrispTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  int sumOnPattern = 0;
  dynamic_bitset<>::size_type elementId = find_first(firstCandidateElement);
  {
    // Hyperplanes until the last present one
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    unsigned int presentElementId = *presentElementIdIt;
    for (; elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
      {
	--numbersOfPresentTuples[elementId];
	if (elementId >= presentElementId)
	  {
	    if (elementId == presentElementId)
	      {
		++sumOnPattern;
		if (++presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
	      }
	    else
	      {
		presentElementIdIt = lower_bound(++presentElementIdIt, presentElementIdEnd, elementId);
		if (presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
		if (elementId == *presentElementIdIt)
		  {
		    ++sumOnPattern;
		    if (++presentElementIdIt == presentElementIdEnd)
		      {
			break;
		      }
		  }
	      }
	    presentElementId = *presentElementIdIt;
	  }
      }
  }
  // Hyperplanes after the last present one
  for (elementId = tube.find_next(elementId); elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
    {
      --numbersOfPresentTuples[elementId];
    }
  return sumOnPattern;
}

void DenseCrispTube::increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  for (dynamic_bitset<>::size_type elementId = find_first(firstCandidateElement); elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
    {
      ++numbersOfPresentTuples[elementId];
    }
}

void DenseCrispTube::decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  for (dynamic_bitset<>::size_type elementId = find_first(firstCandidateElement); elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
    {
      --numbersOfPresentTuples[elementId];
    }
}

void DenseCrispTube::increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& numbersOfPresentTuples) const
{
  for (dynamic_bitset<>::size_type elementId = find_first(newFirstCandidateElement); elementId < oldFirstCandidateElement; elementId = tube.find_next(elementId))
    {
      ++numbersOfPresentTuples[elementId];
    }
}

// TODO: replace the calls to the function below with tube.find_first(firstCandidateElement) when the Boost version implementing that is old enough
int DenseCrispTube::find_first(const unsigned int firstCandidateElement) const
{
  if (firstCandidateElement)
    {
      return tube.find_next(firstCandidateElement - 1);
    }
  return tube.find_first();
}

#else

void DenseCrispTube::decreaseSumsOnHyperplanes(vector<int>& numbersOfPresentTuples) const
{
  dynamic_bitset<>::size_type elementId = tube.find_first();
  do
    {
      --numbersOfPresentTuples[elementId];
      elementId = tube.find_next(elementId);
    }
  while (elementId != dynamic_bitset<>::npos);
}

int DenseCrispTube::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& numbersOfPresentTuples) const
{
  int sumOnPattern = 0;
  dynamic_bitset<>::size_type elementId = tube.find_first();
  {
    // Hyperplanes until the last present one
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    unsigned int presentElementId = *presentElementIdIt;
    do
      {
	--numbersOfPresentTuples[elementId];
	if (elementId >= presentElementId)
	  {
	    if (elementId == presentElementId)
	      {
		++sumOnPattern;
		if (++presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
	      }
	    else
	      {
		presentElementIdIt = lower_bound(++presentElementIdIt, presentElementIdEnd, elementId);
		if (presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
		if (elementId == *presentElementIdIt)
		  {
		    ++sumOnPattern;
		    if (++presentElementIdIt == presentElementIdEnd)
		      {
			break;
		      }
		  }
	      }
	    presentElementId = *presentElementIdIt;
	  }
	elementId = tube.find_next(elementId);
      }
    while (elementId != dynamic_bitset<>::npos);
  }
  // Hyperplanes after the last present one
  for (elementId = tube.find_next(elementId); elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
    {
      --numbersOfPresentTuples[elementId];
    }
  return sumOnPattern;
}
#endif

#if defined ASSERT || !defined PRUNE
int DenseCrispTube::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& numbersOfPresentTuples) const
{
  int sumOnPattern = 0;
  dynamic_bitset<>::size_type elementId = tube.find_first();
  {
    // Hyperplanes until the last present one
    vector<unsigned int>::const_iterator presentElementIdIt = presentElementIdBegin;
    unsigned int presentElementId = *presentElementIdIt;
    do
      {
	++numbersOfPresentTuples[elementId];
	if (elementId >= presentElementId)
	  {
	    if (elementId == presentElementId)
	      {
		++sumOnPattern;
		if (++presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
	      }
	    else
	      {
		presentElementIdIt = lower_bound(++presentElementIdIt, presentElementIdEnd, elementId);
		if (presentElementIdIt == presentElementIdEnd)
		  {
		    break;
		  }
		if (elementId == *presentElementIdIt)
		  {
		    ++sumOnPattern;
		    if (++presentElementIdIt == presentElementIdEnd)
		      {
			break;
		      }
		  }
	      }
	    presentElementId = *presentElementIdIt;
	  }
	elementId = tube.find_next(elementId);
      }
    while (elementId != dynamic_bitset<>::npos);
  }
  // Hyperplanes after the last present one
  for (elementId = tube.find_next(elementId); elementId != dynamic_bitset<>::npos; elementId = tube.find_next(elementId))
    {
      ++numbersOfPresentTuples[elementId];
    }
  return sumOnPattern;
}

void DenseCrispTube::increaseSumsOnHyperplanes(vector<int>& numbersOfPresentTuples) const
{
  dynamic_bitset<>::size_type elementId = tube.find_first();
  do
    {
      ++numbersOfPresentTuples[elementId];
      elementId = tube.find_next(elementId);
    }
  while (elementId != dynamic_bitset<>::npos);
}
#endif
