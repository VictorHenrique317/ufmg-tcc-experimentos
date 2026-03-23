// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TRIE_WITH_PREDICTION_H_
#define TRIE_WITH_PREDICTION_H_

#include "AbstractTrieWithPrediction.h"

class TrieWithPrediction : public AbstractTrieWithPrediction
{
 public:
  TrieWithPrediction();
  TrieWithPrediction(TrieWithPrediction&& otherTrieWithPrediction);
  TrieWithPrediction(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);
  TrieWithPrediction(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension);

  /* Dense constructor */
  TrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);
  TrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension);

  ~TrieWithPrediction();

  virtual void deleteHyperplanes();

  void setTuple(const vector<unsigned int>::const_iterator idIt, const int membership);

  virtual long long membershipSum(const vector<vector<unsigned int>>& nSet) const;
  virtual void membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const;
  virtual void addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density);
  virtual void addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density);
  virtual void deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const;
  virtual void deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const;
  virtual void deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const;
  virtual void reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd);

 protected:
  vector<AbstractTrieWithPrediction*> hyperplanes;
};

#endif /*TRIE_WITH_PREDICTION_H_*/
