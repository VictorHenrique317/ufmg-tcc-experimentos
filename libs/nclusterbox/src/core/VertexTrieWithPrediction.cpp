// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VertexTrieWithPrediction.h"
#include "LastTrieWithPrediction.h"

VertexTrieWithPrediction::VertexTrieWithPrediction(): TrieWithPrediction()
{
}

VertexTrieWithPrediction::VertexTrieWithPrediction(VertexTrieWithPrediction&& otherVertexTrieWithPrediction): TrieWithPrediction(std::move(otherVertexTrieWithPrediction))
{
}

VertexTrieWithPrediction::VertexTrieWithPrediction(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): TrieWithPrediction()
{
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrieWithPrediction(nbOfVerticesBefore, lastCardinality));
	}
      while (++nbOfVerticesBefore != cardinality);
      return;
    }
  do
    {
      hyperplanes.push_back(new TrieWithPrediction(nbOfVerticesBefore, nextCardinalityIt, lastCardinalityIt));
    }
  while (++nbOfVerticesBefore != cardinality);
}

VertexTrieWithPrediction::VertexTrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): TrieWithPrediction()
{
  hyperplanes.reserve(cardinality);
  hyperplanes.push_back(nullptr); // never accessed
  unsigned int nbOfVerticesBefore = 1;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrieWithPrediction(membershipIt, unit, nbOfVerticesBefore, lastCardinality));
	}
      while (++nbOfVerticesBefore != cardinality);
      return;
    }
  do
    {
      hyperplanes.push_back(new TrieWithPrediction(membershipIt, unit, nbOfVerticesBefore, nextCardinalityIt, lastCardinalityIt));
    }
  while (++nbOfVerticesBefore != cardinality);
}

void VertexTrieWithPrediction::deleteHyperplanes()
{
  const vector<AbstractTrieWithPrediction*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrieWithPrediction*>::iterator hyperplaneIt = ++hyperplanes.begin();
  do
    {
      delete *hyperplaneIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void VertexTrieWithPrediction::membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->membershipSumOnSlice(dimensionIt, idIt, sum);
    }
  while (++idIt != idEnd);
}

long long VertexTrieWithPrediction::membershipSum(const vector<vector<unsigned int>>& nSet) const
{
  long long sum = 0;
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      int sumOnSlice = 0;
      hyperplanes[*idIt]->membershipSumOnSlice(dimensionIt, idIt, sumOnSlice);
      sum += sumOnSlice;
    }
  while (++idIt != idEnd);
  return sum;
}

void VertexTrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->addFirstPatternToModel(dimensionIt, idIt, density);
    }
  while (++idIt != idEnd);
}

void VertexTrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->addPatternToModel(dimensionIt, idIt, density);
    }
  while (++idIt != idEnd);
}

void VertexTrieWithPrediction::deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->deltaOfRSSVariationAdding(dimensionIt, idIt, minDensityOfSelectedAndUpdated, delta);
    }
  while (++idIt != idEnd);
}

void VertexTrieWithPrediction::deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->deltaOfRSSVariationRemovingIfSparserSelected(dimensionIt, idIt, updatedDensity, selectedDensity, delta);
    }
  while (++idIt != idEnd);
}

void VertexTrieWithPrediction::deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->deltaOfRSSVariationRemovingIfDenserSelected(dimensionIt, idIt, updatedDensity, delta);
    }
  while (++idIt != idEnd);
}

void VertexTrieWithPrediction::reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd)
{
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin() + 1;
  do
    {
      hyperplanes[*idIt]->reset(dimensionIt, idIt);
    }
  while (++idIt != idEnd);
}
