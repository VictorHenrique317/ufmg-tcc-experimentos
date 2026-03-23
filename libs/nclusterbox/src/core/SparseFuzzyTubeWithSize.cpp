// Copyright 2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseFuzzyTubeWithSize.h"

SparseFuzzyTubeWithSize::SparseFuzzyTubeWithSize(const unsigned int sizeParam): SparseFuzzyTube(), size(sizeParam)
{
}

#ifdef PRUNE
int SparseFuzzyTubeWithSize::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const
{
  return SparseFuzzyTube::sumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, firstCandidateElement, sums.begin(), sums.begin() + size);
}

int SparseFuzzyTubeWithSize::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const
{
  return SparseFuzzyTube::minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, firstCandidateElement, sums.begin(), sums.begin() + size);
}

void SparseFuzzyTubeWithSize::increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const
{
  SparseFuzzyTube::increaseSumsOnHyperplanes(firstCandidateElement, sums.begin(), sums.begin() + size);
}

void SparseFuzzyTubeWithSize::decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const
{
  SparseFuzzyTube::decreaseSumsOnHyperplanes(firstCandidateElement, sums.begin(), sums.begin() + size);
}

#else

void SparseFuzzyTubeWithSize::decreaseSumsOnHyperplanes(vector<int>& sums) const
{
  SparseFuzzyTube::decreaseSumsOnHyperplanes(sums.begin(), sums.begin() + size);
}

int SparseFuzzyTubeWithSize::minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const
{
  return SparseFuzzyTube::minusSumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, sums.begin(), sums.begin() + size);
}
#endif

#if defined ASSERT || !defined PRUNE
int SparseFuzzyTubeWithSize::sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const
{
  return SparseFuzzyTube::sumsOnHyperplanes(presentElementIdBegin, presentElementIdEnd, sums.begin(), sums.begin() + size);
}

void SparseFuzzyTubeWithSize::increaseSumsOnHyperplanes(vector<int>& sums) const
{
  SparseFuzzyTube::increaseSumsOnHyperplanes(sums.begin(), sums.begin() + size);
}
#endif
