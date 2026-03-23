// Copyright 2018-2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "VertexTrieWithPrediction.h"
#include "VertexLastTrieWithPrediction.h"

TrieWithPrediction::TrieWithPrediction(): hyperplanes()
{
}

TrieWithPrediction::TrieWithPrediction(TrieWithPrediction&& otherTrieWithPrediction): hyperplanes(std::move(otherTrieWithPrediction.hyperplanes))
{
}

TrieWithPrediction::TrieWithPrediction(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  if (nextNextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int nextNextCardinality = *nextNextCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrieWithPrediction(nextCardinality, nextNextCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new TrieWithPrediction(nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

TrieWithPrediction::TrieWithPrediction(const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      do
	{
	  hyperplanes.push_back(new VertexLastTrieWithPrediction(nextCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  const unsigned int nextDistanceToVertexDimension = distanceToVertexDimension - 1;
  if (nextDistanceToVertexDimension)
    {
      do
	{
	  hyperplanes.push_back(new TrieWithPrediction(nextCardinality, nextNextCardinalityIt, lastCardinalityIt, nextDistanceToVertexDimension));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new VertexTrieWithPrediction(nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

TrieWithPrediction::TrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  if (nextNextCardinalityIt == lastCardinalityIt)
    {
      const unsigned int lastCardinality = *lastCardinalityIt;
      do
	{
	  hyperplanes.push_back(new LastTrieWithPrediction(membershipIt, unit, nextCardinality, lastCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new TrieWithPrediction(membershipIt, unit, nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

TrieWithPrediction::TrieWithPrediction(vector<double>::const_iterator& membershipIt, const unsigned int unit, const unsigned int cardinality, const vector<unsigned int>::const_iterator nextCardinalityIt, const vector<unsigned int>::const_iterator lastCardinalityIt, const unsigned int distanceToVertexDimension): hyperplanes()
{
  hyperplanes.reserve(cardinality);
  unsigned int nbOfMissingHyperplanes = cardinality;
  const unsigned int nextCardinality = *nextCardinalityIt;
  if (nextCardinalityIt == lastCardinalityIt)
    {
      do
	{
	  hyperplanes.push_back(new VertexLastTrieWithPrediction(membershipIt, unit, nextCardinality));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  const vector<unsigned int>::const_iterator nextNextCardinalityIt = nextCardinalityIt + 1;
  const unsigned int nextDistanceToVertexDimension = distanceToVertexDimension - 1;
  if (nextDistanceToVertexDimension)
    {
      do
	{
	  hyperplanes.push_back(new TrieWithPrediction(membershipIt, unit, nextCardinality, nextNextCardinalityIt, lastCardinalityIt, nextDistanceToVertexDimension));
	}
      while (--nbOfMissingHyperplanes);
      return;
    }
  do
    {
      hyperplanes.push_back(new VertexTrieWithPrediction(membershipIt, unit, nextCardinality, nextNextCardinalityIt, lastCardinalityIt));
    }
  while (--nbOfMissingHyperplanes);
}

TrieWithPrediction::~TrieWithPrediction()
{
  deleteHyperplanes();
}

void TrieWithPrediction::deleteHyperplanes()
{
  const vector<AbstractTrieWithPrediction*>::iterator hyperplaneEnd = hyperplanes.end();
  vector<AbstractTrieWithPrediction*>::iterator hyperplaneIt = hyperplanes.begin();
  do
    {
      delete *hyperplaneIt;
    }
  while (++hyperplaneIt != hyperplaneEnd);
}

void TrieWithPrediction::setTuple(const vector<unsigned int>::const_iterator idIt, const int membership)
{
  hyperplanes[*idIt]->setTuple(idIt + 1, membership);
}

void TrieWithPrediction::membershipSumOnSlice(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, int& sum) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->membershipSumOnSlice(nextDimensionIt, nextIdEnd, sum);
    }
  while (++idIt != idEnd);
}

long long TrieWithPrediction::membershipSum(const vector<vector<unsigned int>>& nSet) const
{
  long long sum = 0;
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  const vector<unsigned int>::const_iterator idEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  const vector<unsigned int>::const_iterator nextIdEnd = (++dimensionIt)->end();
  do
    {
      int sumOnSlice = 0;
      hyperplanes[*idIt]->membershipSumOnSlice(dimensionIt, nextIdEnd, sumOnSlice);
      sum += sumOnSlice;
    }
  while (++idIt != idEnd);
  return sum;
}

void TrieWithPrediction::addFirstPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->addFirstPatternToModel(nextDimensionIt, nextIdEnd, density);
    }
  while (++idIt != idEnd);
}

void TrieWithPrediction::addPatternToModel(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int density)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->addPatternToModel(nextDimensionIt, nextIdEnd, density);
    }
  while (++idIt != idEnd);
}

void TrieWithPrediction::deltaOfRSSVariationAdding(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int minDensityOfSelectedAndUpdated, long long& delta) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->deltaOfRSSVariationAdding(nextDimensionIt, nextIdEnd, minDensityOfSelectedAndUpdated, delta);
    }
  while (++idIt != idEnd);
}

void TrieWithPrediction::deltaOfRSSVariationRemovingIfSparserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, const int selectedDensity, long long& delta) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->deltaOfRSSVariationRemovingIfSparserSelected(nextDimensionIt, nextIdEnd, updatedDensity, selectedDensity, delta);
    }
  while (++idIt != idEnd);
}

void TrieWithPrediction::deltaOfRSSVariationRemovingIfDenserSelected(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd, const int updatedDensity, long long& delta) const
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->deltaOfRSSVariationRemovingIfDenserSelected(nextDimensionIt, nextIdEnd, updatedDensity, delta);
    }
  while (++idIt != idEnd);
}

void TrieWithPrediction::reset(const vector<vector<unsigned int>>::const_iterator dimensionIt, const vector<unsigned int>::const_iterator idEnd)
{
  const vector<vector<unsigned int>>::const_iterator nextDimensionIt = dimensionIt + 1;
  const vector<unsigned int>::const_iterator nextIdEnd = nextDimensionIt->end();
  vector<unsigned int>::const_iterator idIt = dimensionIt->begin();
  do
    {
      hyperplanes[*idIt]->reset(nextDimensionIt, nextIdEnd);
    }
  while (++idIt != idEnd);
}
