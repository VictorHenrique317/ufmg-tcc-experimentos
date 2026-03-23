// Copyright 2023,2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "EdgeFileReader.h"

#include <boost/lexical_cast.hpp>

#include "../utilities/UsageException.h"

EdgeFileReader::EdgeFileReader(const char* tensorFileNameParam, const char* dimensionSeparatorParam, const char* elementSeparatorParam, const unsigned int firstVertexDimensionParam, const unsigned int secondVertexDimensionParam): DataFileReader(tensorFileNameParam, elementSeparatorParam), dimensionSeparator(dimensionSeparatorParam), firstVertexDimension(firstVertexDimensionParam), secondVertexDimension(secondVertexDimensionParam), ids2VertexLabels(), vertexLabels2Ids()
{
  init();
}

void EdgeFileReader::init()
{
  if (tensorStream.eof())
    {
      throw UsageException(("No edge in " + tensorFileName + '!').c_str());
    }
  string line = nextLine();
  tokenizer<char_separator<char>> dimensions(line, dimensionSeparator);
  if (dimensions.begin() == dimensions.end())
    {
      init();
      return;
    }
  initTupleEnumeration(std::distance(dimensions.begin(), dimensions.end()));
  if (nSet.size() <= secondVertexDimension)
    {
      throw UsageException(("communities option should provide two dimension ids in [1, " + lexical_cast<string>(nSet.size()) + "], given " + tensorFileName + '!').c_str());
    }
  ids2Labels.resize(nSet.size() - 2);
  labels2Ids.resize(ids2Labels.size());
  while (!parseLine(dimensions))
    {
      if (tensorStream.eof())
	{
	  throw UsageException(("Only self loops in " + tensorFileName + '!').c_str());
	}
      line = nextLine();
      dimensions = tokenizer<char_separator<char>>(line, dimensionSeparator);
    }
}

vector<FuzzyTuple> EdgeFileReader::read()
{
  const vector<vector<unsigned int>::const_iterator>::iterator tupleItsEnd = tupleIts.end();
  const vector<vector<unsigned int>>::const_iterator nSetEnd = nSet.end();
  const vector<vector<unsigned int>::const_iterator>::iterator tupleItsFirstVertexIt = tupleIts.begin() + firstVertexDimension;
  const vector<vector<unsigned int>::const_iterator>::iterator tupleItsSecondVertexIt = tupleIts.begin() + secondVertexDimension;
  for (vector<FuzzyTuple> fuzzyTuples; ; )
    {
      if (**tupleItsSecondVertexIt < **tupleItsFirstVertexIt)
	{
	  fuzzyTuples.emplace_back(tupleIts, 1.);
	}
      else
	{
	  if (**tupleItsFirstVertexIt != **tupleItsSecondVertexIt)
	    {
	      const vector<unsigned int>::const_iterator tuplesItToLargerId = *tupleItsSecondVertexIt;
	      *tupleItsSecondVertexIt = *tupleItsFirstVertexIt;
	      *tupleItsFirstVertexIt = tuplesItToLargerId;
	      fuzzyTuples.emplace_back(tupleIts, 1.);
	      *tupleItsFirstVertexIt = *tupleItsSecondVertexIt;
	      *tupleItsSecondVertexIt = tuplesItToLargerId;
	    }
	}
      // Advance tuple in nSet, little-endian-like
      vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
      for (vector<vector<unsigned int>>::const_iterator nSetIt = nSet.begin(); nSetIt != nSetEnd && ++*tupleItsIt == nSetIt->end(); ++nSetIt)
	{
	  *tupleItsIt++ = nSetIt->begin();
	}
      if (tupleItsIt == tupleItsEnd)
	{
	  // All tuples in nSet enumerated: find and parse the next line (if any)
	  for (; ; )
	    {
	      if (tensorStream.eof())
		{
		  return terminate(fuzzyTuples);
		}
	      const string line = nextLine();
	      const tokenizer<char_separator<char>> dimensions(line, dimensionSeparator);
	      if (dimensions.begin() != dimensions.end() && parseLine(dimensions))
		{
		  break;
		}
	    }
	}
    }
}

vector<string>& EdgeFileReader::getIds2VertexLabels()
{
  return ids2VertexLabels;
}

bool EdgeFileReader::parseLine(const tokenizer<char_separator<char>>& dimensions)
{
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  vector<vector<unsigned int>>::iterator nSetIt = nSet.begin();
  const tokenizer<char_separator<char>>::const_iterator dimensionEnd = dimensions.end();
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
  vector<unordered_map<string, unsigned int>>::iterator labels2IdsIt = labels2Ids.begin();
  {
    unsigned int dimensionId = 0;
    for (; dimensionId != firstVertexDimension; ++dimensionId)
      {
	parseDimension(dimensionIt, dimensionEnd, nSetIt, *labels2IdsIt, *ids2LabelsIt);
	*tupleItsIt++ = nSetIt->begin();
	++ids2LabelsIt;
	++nSetIt;
	++dimensionIt;
	++labels2IdsIt;
      }
    parseDimension(dimensionIt, dimensionEnd, nSetIt, vertexLabels2Ids, ids2VertexLabels); // first vertex dimension
    *tupleItsIt++ = nSetIt->begin();
    ++nSetIt;
    ++dimensionIt;
    while (++dimensionId != secondVertexDimension)
      {
	parseDimension(dimensionIt, dimensionEnd, nSetIt, *labels2IdsIt, *ids2LabelsIt);
	*tupleItsIt++ = nSetIt->begin();
	++ids2LabelsIt;
	++nSetIt;
	++dimensionIt;
	++labels2IdsIt;
      }
  }
  parseDimension(dimensionIt, dimensionEnd, nSetIt, vertexLabels2Ids, ids2VertexLabels); // second vertex dimension
  if (nSetIt->size() == 1 && nSetIt->front() == nSet[firstVertexDimension].front())
    {
      return false;
    }
  *tupleItsIt++ = nSetIt->begin();
  ++nSetIt;
  ++dimensionIt;
  for (const vector<unordered_map<string, unsigned int>>::iterator labels2IdsEnd = labels2Ids.end(); labels2IdsIt != labels2IdsEnd; ++labels2IdsIt)
    {
      parseDimension(dimensionIt, dimensionEnd, nSetIt, *labels2IdsIt, *ids2LabelsIt);
      *tupleItsIt++ = nSetIt->begin();
      ++ids2LabelsIt;
      ++nSetIt;
      ++dimensionIt;
    }
  throwIfNotEndOfRecord(dimensionIt != dimensionEnd);
  return true;
}
