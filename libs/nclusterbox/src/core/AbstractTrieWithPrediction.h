// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_TRIE_WITH_PREDICTION_H_
#define ABSTRACT_TRIE_WITH_PREDICTION_H_

#include <vector>

using namespace std;

class AbstractTrieWithPrediction
{
 public:
  virtual ~AbstractTrieWithPrediction();

  virtual void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership) = 0;

  virtual long long membershipSum(const vector<vector<unsigned int>>& nSet) const = 0;
  virtual void membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const = 0;
  void addFirstPatternToModel(const vector<vector<unsigned int>>& tuples, const int density);
  virtual void addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density) = 0;
  void addPatternToModel(const vector<vector<unsigned int>>& tuples, const int density);
  virtual void addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density) = 0;
  long long deltaOfRSSVariationAdding(const vector<vector<unsigned int>>& tuples, const int minDensityOfSelectedAndUpdated) const;
  virtual void deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const = 0;
  long long deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>> tuples, const int updatedDensity, const int selectedDensity) const;
  virtual void deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const = 0;
  long long deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>> tuples, const int updatedDensity) const;
  virtual void deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const = 0;
  void reset(const vector<vector<unsigned int>>& tuples);
  virtual void reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd) = 0;
};

#endif /*ABSTRACT_TRIE_WITH_PREDICTION_H_*/
