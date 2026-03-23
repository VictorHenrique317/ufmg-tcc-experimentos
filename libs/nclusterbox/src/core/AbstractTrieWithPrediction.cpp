// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "AbstractTrieWithPrediction.h"

AbstractTrieWithPrediction::~AbstractTrieWithPrediction()
{
}

void AbstractTrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>& tuples, const int density)
{
  addFirstPatternToModel(tuples.begin(), tuples.front().end(), density);
}

void AbstractTrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>& tuples, const int density)
{
  addPatternToModel(tuples.begin(), tuples.front().end(), density);
}

long long AbstractTrieWithPrediction::deltaOfRSSVariationAdding(const vector<vector<unsigned int>>& tuples, const int minDensityOfSelectedAndUpdated) const
{
  long long delta = 0;
  deltaOfRSSVariationAdding(tuples.begin(), tuples.front().end(), minDensityOfSelectedAndUpdated, delta);
  return delta;
}

long long AbstractTrieWithPrediction::deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>> tuples, const int updatedDensity, const int selectedDensity) const
{
  long long delta = 0;
  deltaOfRSSVariationRemovingIfSparserSelected(tuples.begin(), tuples.front().end(), updatedDensity, selectedDensity, delta);
  return delta;
}


long long AbstractTrieWithPrediction::deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>> tuples, const int updatedDensity) const
{
  long long delta = 0;
  deltaOfRSSVariationRemovingIfDenserSelected(tuples.begin(), tuples.front().end(), updatedDensity, delta);
  return delta;
}

void AbstractTrieWithPrediction::reset(const vector<vector<unsigned int>>& tuples)
{
  reset(tuples.begin(), tuples.front().end());
}
