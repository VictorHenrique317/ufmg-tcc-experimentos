// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENSE_CRISP_TUBE_H_
#define DENSE_CRISP_TUBE_H_

#include <boost/dynamic_bitset.hpp>

#include "Tube.h"

using namespace boost;

class DenseCrispTube final : public Tube
{
 public:
  DenseCrispTube(vector<double>::const_iterator& membershipIt, const unsigned int size);
  DenseCrispTube(const vector<unsigned int>& sparseTube, const unsigned int size);

  int at(const unsigned int element) const;
  void set(const unsigned int element);

  void sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& nbOfPresentTuples) const;
  void minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& nbOfPresentTuples) const;

#ifdef PRUNE
  int increasePositiveMemberships(vector<int>& numbersOfPresentTuples) const;
  int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const;
  int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const;
  void increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const;
  void decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& numbersOfPresentTuples) const;
  void increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& numbersOfPresentTuples) const;

#else

  void decreaseSumsOnHyperplanes(vector<int>& numbersOfPresentTuples) const;
  int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& numbersOfPresentTuples) const;
#endif

#if defined ASSERT || !defined PRUNE
  int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& numbersOfPresentTuples) const;
  void increaseSumsOnHyperplanes(vector<int>& numbersOfPresentTuples) const;
#endif

 private:
  dynamic_bitset<> tube;		/* indicates the presence of the shifted tuple */
  /* TODO: test whether storing is faster than computing tube.find_first() every time it is needed */

#ifdef PRUNE
  int find_first(const unsigned int firstCandidateElement) const;
#endif
};

#endif /*DENSE_CRISP_TUBE_H_*/
