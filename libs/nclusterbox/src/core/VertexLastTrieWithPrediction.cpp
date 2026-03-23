// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VertexLastTrieWithPrediction.h"

VertexLastTrieWithPrediction::VertexLastTrieWithPrediction(): LastTrieWithPrediction()
{
}

VertexLastTrieWithPrediction::VertexLastTrieWithPrediction(VertexLastTrieWithPrediction&& otherVertexLastTrieWithPrediction): LastTrieWithPrediction(std::move(otherVertexLastTrieWithPrediction))
{
}

VertexLastTrieWithPrediction::VertexLastTrieWithPrediction(const unsigned int cardinality): LastTrieWithPrediction()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfVerticesBefore = 0;
  do
    {
      hyperplanes.emplace_back(nbOfVerticesBefore);
    }
  while (++nbOfVerticesBefore != cardinality);
}

VertexLastTrieWithPrediction::VertexLastTrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality): LastTrieWithPrediction()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfVerticesBefore = 0;
  do
    {
      hyperplanes.emplace_back(membershipIt, nbOfVerticesBefore, unit);
    }
  while (++nbOfVerticesBefore != cardinality);
}

void VertexLastTrieWithPrediction::membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].membershipSumOnSlice(idBegin, idIt, sum);
    }
  while (++idIt != idEnd);
}

long long VertexLastTrieWithPrediction::membershipSum(const vector<vector<unsigned int>>& nSet) const
{
  long long sum = 0;
  const vector<unsigned int>::const_iterator idEnd = nSet.front().end();
  const vector<unsigned int>::const_iterator idBegin = nSet.front().begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      int sumOnSlice = 0;
      hyperplanes[*idIt].membershipSumOnSlice(idBegin, idIt, sumOnSlice);
      sum += sumOnSlice;
    }
  while (++idIt != idEnd);
  return sum;
}

void VertexLastTrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].addFirstPatternToModel(idBegin, idIt, density);
    }
  while (++idIt != idEnd);
}

void VertexLastTrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].addPatternToModel(idBegin, idIt, density);
    }
  while (++idIt != idEnd);
}

void VertexLastTrieWithPrediction::deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].deltaOfRSSVariationAdding(idBegin, idIt, minDensityOfSelectedAndUpdated, delta);
    }
  while (++idIt != idEnd);
}

void VertexLastTrieWithPrediction::deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].deltaOfRSSVariationRemovingIfSparserSelected(idBegin, idIt, updatedDensity, selectedDensity, delta);
    }
  while (++idIt != idEnd);
}

void VertexLastTrieWithPrediction::deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].deltaOfRSSVariationRemovingIfDenserSelected(idBegin, idIt, updatedDensity, delta);
    }
  while (++idIt != idEnd);
}

void VertexLastTrieWithPrediction::reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd)
{
  const vector<unsigned int>::const_iterator idBegin = dimensionIt->begin();
  vector<unsigned int>::const_iterator idIt = idBegin + 1;
  do
    {
      hyperplanes[*idIt].reset(idBegin, idIt);
    }
  while (++idIt != idEnd);
}
