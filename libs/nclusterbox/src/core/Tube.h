// Copyright 2024-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TUBE_H_
#define TUBE_H_

#include <vector>

#include "../../Parameters.h"

using namespace std;

class Tube
{
 public:
  virtual ~Tube();

  virtual bool isFullSparseTube(const unsigned int size) const;

  virtual int at(const unsigned int element) const = 0;
  virtual void set(const unsigned int element);
  virtual void set(const unsigned int element, const int membership);
  virtual void sortTubes();

  virtual void sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const = 0;
  virtual void minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const = 0;

#ifdef PRUNE
  virtual int increasePositiveMemberships(vector<int>& sums) const = 0;
  virtual int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const = 0;
  virtual int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const = 0;
  virtual void increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const = 0;
  virtual void decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const = 0;
  virtual void increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const = 0;

#else

  virtual void decreaseSumsOnHyperplanes(vector<int>& sums) const = 0;
  virtual int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const = 0;
#endif

#if defined ASSERT || !defined PRUNE
  virtual int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const = 0;
  virtual void increaseSumsOnHyperplanes(vector<int>& sums) const = 0;
#endif
};

#endif /*TUBE_H_*/
