// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "CandidateVariable.h"

#include <algorithm>

#include "ConcurrentPatternPool.h"

bool CandidateVariable::isGraphPattern;

CandidateVariable::CandidateVariable(CandidateVariable&& otherCandidateVariable): nSet(std::move(otherCandidateVariable.nSet)), density(otherCandidateVariable.density), rssVariation(otherCandidateVariable.rssVariation)
{
}

CandidateVariable::CandidateVariable(vector<vector<unsigned int>>& nSetParam, const long long membershipSum): nSet(std::move(nSetParam)), density(), rssVariation()
{
  const long long area = getArea();
  density = membershipSum / area;
  rssVariation = -area * density * density;
}

CandidateVariable& CandidateVariable::operator=(CandidateVariable&& otherCandidateVariable)
{
  if (this != &otherCandidateVariable) // Guard self assignment
    {
      nSet = std::move(otherCandidateVariable.nSet);
      density = otherCandidateVariable.density;
      rssVariation = otherCandidateVariable.rssVariation;
    }
  return *this;
}

const vector<vector<unsigned int>>& CandidateVariable::getNSet() const
{
  return nSet;
}

int CandidateVariable::getDensity() const
{
  return density;
}

long long CandidateVariable::getRSSVariation() const
{
  return rssVariation;
}

long long CandidateVariable::getArea() const
{
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  long long area = dimensionIt->size();
  if (isGraphPattern)
    {
      for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
	{
	  area *= dimensionIt->size();
	}
      area *= nSet[ConcurrentPatternPool::vertexDimension].size() - 1;
      return area / 2;
    }
  ++dimensionIt;
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  do
    {
      area *= dimensionIt->size();
    }
  while (++dimensionIt != dimensionEnd);
  return area;
}

vector<vector<unsigned int>> CandidateVariable::inter(const vector<vector<unsigned int>>::const_iterator otherDimensionBegin) const
{
  vector<vector<unsigned int>>::const_iterator otherDimensionIt = otherDimensionBegin;
  vector<vector<unsigned int>> intersection;
  intersection.reserve(nSet.size());
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  if (isGraphPattern)
    {
      // A graph pattern: intersect dimensions until the vertex one, where a one-vertex intersection equates to an empty intersection
      for (const vector<vector<unsigned int>>::const_iterator vertexDimensionIt = dimensionIt + ConcurrentPatternPool::vertexDimension; dimensionIt != vertexDimensionIt; ++dimensionIt)
	{
	  intersection.emplace_back();
	  set_intersection(dimensionIt->begin(), dimensionIt->end(), otherDimensionIt->begin(), otherDimensionIt->end(), back_inserter(intersection.back()));
	  if (intersection.back().empty())
	    {
	      return {};
	    }
	  ++otherDimensionIt;
	}
      intersection.emplace_back();
      set_intersection(dimensionIt->begin(), dimensionIt->end(), otherDimensionIt->begin(), otherDimensionIt->end(), back_inserter(intersection.back()));
      if (intersection.back().size() < 2)
	{
	  return {};
	}
    }
  else
    {
      // Not a grah pattern: intersect the first two dimensions
      intersection.emplace_back();
      set_intersection(dimensionIt->begin(), dimensionIt->end(), otherDimensionIt->begin(), otherDimensionIt->end(), back_inserter(intersection.back()));
      if (intersection.back().empty())
	{
	  return {};
	}
      ++dimensionIt;
      ++otherDimensionIt;
      intersection.emplace_back();
      set_intersection(dimensionIt->begin(), dimensionIt->end(), otherDimensionIt->begin(), otherDimensionIt->end(), back_inserter(intersection.back()));
      if (intersection.back().empty())
	{
	  return {};
	}
    }
  // Intersect the last dimensions
  for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
    {
      ++otherDimensionIt;
      intersection.emplace_back();
      set_intersection(dimensionIt->begin(), dimensionIt->end(), otherDimensionIt->begin(), otherDimensionIt->end(), back_inserter(intersection.back()));
      if (intersection.back().empty())
	{
	  return {};
	}
    }
  return intersection;
}

void CandidateVariable::addToRSSVariation(const long long delta)
{
  rssVariation += delta;
}

void CandidateVariable::reset()
{
  rssVariation = static_cast<long long>(-density) * density;
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  rssVariation *= dimensionIt->size();
  ++dimensionIt;
  if (isGraphPattern)
    {
      for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); dimensionIt != dimensionEnd; ++dimensionIt)
	{
	  rssVariation *= dimensionIt->size();
	}
      rssVariation *= nSet[ConcurrentPatternPool::vertexDimension].size() - 1;
      rssVariation /= 2;
      return;
    }
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  do
    {
      rssVariation *= dimensionIt->size();
    }
  while (++dimensionIt != dimensionEnd);
}

void CandidateVariable::setIsGraphPattern()
{
  isGraphPattern = ConcurrentPatternPool::vertexDimension != numeric_limits<unsigned int>::max();
}
