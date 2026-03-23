// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VERTEX_TRIE_WITH_PREDICTION_H_
#define VERTEX_TRIE_WITH_PREDICTION_H_

#include "TrieWithPrediction.h"

class VertexTrieWithPrediction final : public TrieWithPrediction
{
 public:
  VertexTrieWithPrediction();
  VertexTrieWithPrediction(VertexTrieWithPrediction&& otherVertexTrieWithPrediction);

  /* Sparse constructor */
  VertexTrieWithPrediction(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);

  /* Dense constructor */
  VertexTrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt);

  void deleteHyperplanes();

  long long membershipSum(const vector<vector<unsigned int>>& nSet) const;
  void membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const;
  void addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density);
  void addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density);
  void deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const;
  void deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const;
  void deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const;
  void reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd);
};

#endif /*VERTEX_TRIE_WITH_PREDICTION_H_*/
