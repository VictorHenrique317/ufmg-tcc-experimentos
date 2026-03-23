// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef SPARSE_FUZZY_TUBE_H_
#define SPARSE_FUZZY_TUBE_H_

#include "DenseFuzzyTube.h"

class SparseFuzzyTube : public Tube
{
 public:
  SparseFuzzyTube();

  bool isFullSparseTube(const unsigned int size) const;

  int at(const unsigned int element) const;
  void set(const unsigned int element, const int membership);
  DenseFuzzyTube* getDenseRepresentation(const unsigned int size) const;
  void sortTubes();

  void sumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const;
  void minusSumOnPattern(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, int& sum) const;

#ifdef PRUNE
  int increasePositiveMemberships(vector<int>& sums) const;
  virtual int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const;
  virtual int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, vector<int>& sums) const;
  virtual void increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const;
  virtual void decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, vector<int>& sums) const;
  void increaseSumsOnNewCandidateHyperplanes(const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sums) const;

#else

  virtual void decreaseSumsOnHyperplanes(vector<int>& sums) const;
  virtual int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const;
#endif

#if defined ASSERT || !defined PRUNE
  virtual int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, vector<int>& sums) const;
  virtual void increaseSumsOnHyperplanes(vector<int>& sums) const;
#endif

  static void setDefaultMembershipAndDensityLimit(const int defaultMembership, const double densityThreshold);

 protected:
#ifdef PRUNE
  int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
  int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
  void increaseSumsOnHyperplanes(const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
  void decreaseSumsOnHyperplanes(const unsigned int firstCandidateElement, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;

#else

  void decreaseSumsOnHyperplanes(const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
  int minusSumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
#endif

#if defined ASSERT || !defined PRUNE
  int sumsOnHyperplanes(const vector<unsigned int>::const_iterator presentElementIdBegin, const vector<unsigned int>::const_iterator presentElementIdEnd, const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
  void increaseSumsOnHyperplanes(const vector<int>::iterator sumBegin, const vector<int>::iterator sumEnd) const;
#endif

 private:
  vector<pair<unsigned int, int>> tube;		/* every value is an id followed by a membership */

  static int defaultMembership;
  static double densityThreshold;

  static void addDefaultMembership(vector<int>::iterator& it, const vector<int>::iterator end);
  static void subtractDefaultMembership(vector<int>::iterator& it, const vector<int>::iterator end);
};

#endif /*SPARSE_FUZZY_TUBE_H_*/
