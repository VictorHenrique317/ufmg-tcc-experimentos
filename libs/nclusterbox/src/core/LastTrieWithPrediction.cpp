// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "LastTrieWithPrediction.h"

LastTrieWithPrediction::LastTrieWithPrediction(): hyperplanes()
{
}

LastTrieWithPrediction::LastTrieWithPrediction(LastTrieWithPrediction&& otherLastTrieWithPrediction): hyperplanes(std::move(otherLastTrieWithPrediction.hyperplanes))
{
}

LastTrieWithPrediction::LastTrieWithPrediction(const unsigned int cardinality, const unsigned int lastCardinality): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingTubes = cardinality;
  do
    {
      hyperplanes.emplace_back(lastCardinality);
    }
  while (--nbOfMissingTubes);
}

LastTrieWithPrediction::LastTrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const unsigned int lastCardinality): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingTubes = cardinality;
  do
    {
      hyperplanes.emplace_back(membershipIt, lastCardinality, unit);
    }
  while (--nbOfMissingTubes);
}

LastTrieWithPrediction::~LastTrieWithPrediction()
{
}

void LastTrieWithPrediction::setTuple(const vector<unsigned int>::const_iterator idIt, const int membership)
{
  hyperplanes[*idIt].setTuple(idIt + 1, membership);
}

void LastTrieWithPrediction::membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].membershipSumOnSlice(nextIdBegin, nextIdEnd, sum);
    }
  while (++idIt != idEnd);
}

long long LastTrieWithPrediction::membershipSum(const vector<vector<unsigned int>>& nSet) const
{
  long long sum = 0;
  const vector<unsigned int>::const_iterator nextIdEnd = nSet.back().end();
  const vector<unsigned int>::const_iterator nextIdBegin = nSet.back().begin();
  const vector<unsigned int>::const_iterator idEnd = nSet.front().end();
  vector<unsigned int>::const_iterator idIt = nSet.front().begin();
  do
    {
      int sumOnSlice = 0;
      hyperplanes[*idIt].membershipSumOnSlice(nextIdBegin, nextIdEnd, sumOnSlice);
      sum += sumOnSlice;
    }
  while (++idIt != idEnd);
  return sum;
}

void LastTrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].addFirstPatternToModel(nextIdBegin, nextIdEnd, density);
    }
  while (++idIt != idEnd);
}

void LastTrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].addPatternToModel(nextIdBegin, nextIdEnd, density);
    }
  while (++idIt != idEnd);
}

void LastTrieWithPrediction::deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].deltaOfRSSVariationAdding(nextIdBegin, nextIdEnd, minDensityOfSelectedAndUpdated, delta);
    }
  while (++idIt != idEnd);
}

void LastTrieWithPrediction::deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].deltaOfRSSVariationRemovingIfSparserSelected(nextIdBegin, nextIdEnd, updatedDensity, selectedDensity, delta);
    }
  while (++idIt != idEnd);
}

void LastTrieWithPrediction::deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].deltaOfRSSVariationRemovingIfDenserSelected(nextIdBegin, nextIdEnd, updatedDensity, delta);
    }
  while (++idIt != idEnd);
}

void LastTrieWithPrediction::reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd)
{
  const vector<unsigned int>::const_iterator nextIdEnd = (dimensionIt + 1)->end();
  const vector<unsigned int>::const_iterator nextIdBegin = (dimensionIt + 1)->begin();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt].reset(nextIdBegin, nextIdEnd);
    }
  while (++idIt != idEnd);
}
