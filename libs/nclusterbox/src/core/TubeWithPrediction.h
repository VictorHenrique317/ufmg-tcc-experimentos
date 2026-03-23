// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TUBE_WITH_PREDICTION_H_
#define TUBE_WITH_PREDICTION_H_

#include <vector>

#include "TupleWithPrediction.h"

using namespace std;

class TubeWithPrediction
{
 public:
  TubeWithPrediction();
  TubeWithPrediction(const unsigned int size);
  TubeWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int size, const unsigned int unit);

  void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership);

  void membershipSumOnSlice(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd, int& sum) const;
  void addFirstPatternToModel(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd, const int density);
  void addPatternToModel(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd, const int density);
  void deltaOfRSSVariationAdding(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const;
  void deltaOfRSSVariationRemovingIfSparserSelected(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const;
  void deltaOfRSSVariationRemovingIfDenserSelected(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const;
  void reset(const vector<unsigned int>::const_iterator idBegin, const vector<unsigned int>::const_iterator idEnd);

  static void setDefaultMembership(const int defaultMembership);

 private:
  vector<TupleWithPrediction> tube;

  static int defaultMembership;
};

#endif /*TUBE_WITH_PREDICTION_H_*/
