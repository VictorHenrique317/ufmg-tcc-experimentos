// Copyright 2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef SPARSE_FUZZY_TUBE_WITH_SIZE_H_
#define SPARSE_FUZZY_TUBE_WITH_SIZE_H_

#include "SparseFuzzyTube.h"

class SparseFuzzyTubeWithSize final : public SparseFuzzyTube
{
 public:
  SparseFuzzyTubeWithSize(const unsigned int size);

#ifdef PRUNE
  int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const;
  int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const;
  void increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const;
  void decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const;

#else

  void decreaseSumsOnHyperplanes(vector<int>& sums) const;
  int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const;
#endif

#if defined ASSERT || !defined PRUNE
  int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const;
  void increaseSumsOnHyperplanes(vector<int>& sums) const;
#endif

 private:
  const unsigned int size;
};

#endif /*SPARSE_FUZZY_TUBE_WITH_SIZE_H_*/
