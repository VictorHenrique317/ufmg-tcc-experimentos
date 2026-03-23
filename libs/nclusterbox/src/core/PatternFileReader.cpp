// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "PatternFileReader.h"

#include <iostream>
#include <boost/lexical_cast.hpp>

#include "../utilities/NoInputException.h"
#include "../utilities/DataFormatException.h"
#include "ConcurrentPatternPool.h"
#include "AbstractRoughGraphTensor.h"

unordered_map<string, unsigned int> labels2IdsInDimension(const vector<string>& labelsInDimension)
{
  vector<string>::const_iterator labelIt = labelsInDimension.begin();
  const vector<string>::const_iterator labelEnd = labelsInDimension.end();
  unordered_map<string, unsigned int> labels2Ids;
  labels2Ids.reserve(labelEnd - labelIt);
  unsigned int id = 0;
  do
    {
      labels2Ids[*labelIt] = id++;
    }
  while (++labelIt != labelEnd);
  return labels2Ids;
}

PatternFileReader::PatternFileReader(): noisyNSetFileName(), noisyNSetStream(nullptr), noisyNSetFile(), labels2Ids(), inputDimensionSeparator(), inputElementSeparator(), lineNb(0)
{
}

void PatternFileReader::openFile(const char* noisyNSetFileNameParam, const char* inputDimensionSeparatorParam, const char* inputElementSeparatorParam)
{
  ConcurrentPatternPool::setReadFromFile();
  noisyNSetFileName = noisyNSetFileNameParam;
  inputDimensionSeparator = char_separator<char>(inputDimensionSeparatorParam);
  inputElementSeparator = char_separator<char>(inputElementSeparatorParam);
  if (noisyNSetFileName == "-")
    {
      noisyNSetStream.rdbuf(cin.rdbuf());
      return;
    }
  noisyNSetFile.open(noisyNSetFileNameParam);
  if (!noisyNSetFile)
    {
      throw NoInputException(noisyNSetFileNameParam);
    }
  noisyNSetStream.rdbuf(noisyNSetFile.rdbuf());
}

void PatternFileReader::read(unsigned long long maxNbOfInitialPatterns)
{
  unsigned int n = AbstractRoughTensor::getIds2Labels().size();
  labels2Ids.reserve(n);
  for (const vector<string>& labelsInDimension : AbstractRoughTensor::getIds2Labels())
    {
      labels2Ids.emplace_back(labels2IdsInDimension(labelsInDimension));
    }
  const vector<unordered_map<string, unsigned int>>::const_iterator labels2IdsBegin = labels2Ids.begin();
  const vector<unsigned int>::const_iterator internalDimensionIdEnd = AbstractRoughTensor::getExternal2InternalDimensionOrder().end();
  const vector<unsigned int>::const_iterator internalDimensionIdBegin = AbstractRoughTensor::getExternal2InternalDimensionOrder().begin();
  if (ConcurrentPatternPool::vertexDimension == numeric_limits<unsigned int>::max())
    {
      while (!noisyNSetStream.eof())
	{
	  ++lineNb;
	  string noisyNSetString;
	  getline(noisyNSetStream, noisyNSetString);
	  tokenizer<char_separator<char>> dimensions(noisyNSetString, inputDimensionSeparator);
	  if (dimensions.begin() != dimensions.end())
	    {
#ifdef VERBOSE_PARSER
	      cout << noisyNSetFileName << ':' << lineNb << ": " << noisyNSetString << '\n';
#endif
	      vector<vector<unsigned int>> nSet(n);
	      {
		vector<unsigned int>::const_iterator internalDimensionIdIt = internalDimensionIdBegin;
		vector<unordered_map<string, unsigned int>>::const_iterator labels2IdsIt = labels2IdsBegin;
		const tokenizer<char_separator<char>>::const_iterator dimensionEnd = dimensions.end();
		tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
		try
		  {
		    nSet[*internalDimensionIdIt] = getNonVertexDimension(dimensionIt, dimensionEnd, *labels2IdsIt);
		    ++internalDimensionIdIt;
		    do
		      {
			nSet[*internalDimensionIdIt] = getNonVertexDimension(++dimensionIt, dimensionEnd, *++labels2IdsIt);
		      }
		    while (++internalDimensionIdIt != internalDimensionIdEnd);
		  }
		catch (DataFormatException& e)
		  {
		    cerr << e.what() << " -> pattern ignored!\n";
		    continue;
		  }
	      }
	      ConcurrentPatternPool::addPattern(nSet);
	      if (!--maxNbOfInitialPatterns)
		{
		  break;
		}
	    }
	}
    }
  else
    {
      ++n;
      const unordered_map<string, unsigned int> vertexLabels2Ids = labels2IdsInDimension(AbstractRoughGraphTensor::getIds2VertexLabels());
      while (!noisyNSetStream.eof())
	{
	  ++lineNb;
	  string noisyNSetString;
	  getline(noisyNSetStream, noisyNSetString);
	  tokenizer<char_separator<char>> dimensions(noisyNSetString, inputDimensionSeparator);
	  if (dimensions.begin() != dimensions.end())
	    {
#ifdef VERBOSE_PARSER
	      cout << noisyNSetFileName << ':' << lineNb << ": " << noisyNSetString << '\n';
#endif
	      vector<vector<unsigned int>> nSet(n);
	      {
		vector<unsigned int>::const_iterator internalDimensionIdIt = internalDimensionIdBegin;
		vector<unordered_map<string, unsigned int>>::const_iterator labels2IdsIt = labels2IdsBegin;
		const tokenizer<char_separator<char>>::const_iterator dimensionEnd = dimensions.end();
		tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
		try
		  {
		    for (; *internalDimensionIdIt != ConcurrentPatternPool::vertexDimension; ++internalDimensionIdIt)
		      {
			nSet[*internalDimensionIdIt] = getNonVertexDimension(dimensionIt, dimensionEnd, *labels2IdsIt);
			++dimensionIt;
			++labels2IdsIt;
		      }
		    nSet[ConcurrentPatternPool::vertexDimension] = getDimension(dimensionIt, dimensionEnd, vertexLabels2Ids);
		    for (++dimensionIt; *++internalDimensionIdIt != ConcurrentPatternPool::vertexDimension; ++dimensionIt)
		      {
			nSet[*internalDimensionIdIt] = getNonVertexDimension(dimensionIt, dimensionEnd, *labels2IdsIt);
			++labels2IdsIt;
		      }
		    if (dimensionIt == dimensionEnd)
		      {
			if (++internalDimensionIdIt != internalDimensionIdEnd)
			  {
			    throw DataFormatException(noisyNSetFileName.c_str(), lineNb, "less than the expected number of dimensions");
			  }
		      }
		    else
		      {
			{
			  const vector<unsigned int> additionalVertices = getDimension(dimensionIt, dimensionEnd, vertexLabels2Ids);
			  nSet[ConcurrentPatternPool::vertexDimension].insert(nSet[ConcurrentPatternPool::vertexDimension].end(), additionalVertices.begin(), additionalVertices.end());
			}
			while (++internalDimensionIdIt != internalDimensionIdEnd)
			  {
			    nSet[*internalDimensionIdIt] = getNonVertexDimension(++dimensionIt, dimensionEnd, *labels2IdsIt);
			    ++labels2IdsIt;
			  }
		      }
		    sort(nSet[ConcurrentPatternPool::vertexDimension].begin(), nSet[ConcurrentPatternPool::vertexDimension].end());
		    nSet[ConcurrentPatternPool::vertexDimension].erase(unique(nSet[ConcurrentPatternPool::vertexDimension].begin(), nSet[ConcurrentPatternPool::vertexDimension].end()), nSet[ConcurrentPatternPool::vertexDimension].end());
		    if (nSet[ConcurrentPatternPool::vertexDimension].size() < 2)
		      {
			throw DataFormatException(noisyNSetFileName.c_str(), lineNb, "at least two distinct vertices required");
		      }
		  }
		catch (DataFormatException& e)
		  {
		    cerr << e.what() << " -> pattern ignored!\n";
		    continue;
		  }
	      }
	      ConcurrentPatternPool::addPattern(nSet);
	      if (!--maxNbOfInitialPatterns)
		{
		  break;
		}
	    }
	}
    }
  ConcurrentPatternPool::allPatternsAdded();
  noisyNSetFile.close();
}

vector<unsigned int> PatternFileReader::getDimension(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd, const unordered_map<string, unsigned int>& labels2Ids) const
{
  if (dimensionIt == dimensionEnd)
    {
      throw DataFormatException(noisyNSetFileName.c_str(), lineNb, "less than the expected number of dimensions");
    }
  vector<unsigned int> nSetDimension;
  const unordered_map<string, unsigned int>::const_iterator label2IdEnd = labels2Ids.end();
  tokenizer<char_separator<char>> elements(*dimensionIt, inputElementSeparator);
  for (const string& element : elements)
    {
      const unordered_map<string, unsigned int>::const_iterator label2IdIt = labels2Ids.find(element);
      if (label2IdIt == label2IdEnd)
	{
	  throw DataFormatException(noisyNSetFileName.c_str(), lineNb, (element + " is not in the corresponding dimension of fuzzy tensor").c_str());
	}
      nSetDimension.push_back(label2IdIt->second);
    }
  return nSetDimension;
}

vector<unsigned int> PatternFileReader::getNonVertexDimension(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd, const unordered_map<string, unsigned int>& labels2Ids) const
{
  vector<unsigned int> nSetDimension = getDimension(dimensionIt, dimensionEnd, labels2Ids);
  if (nSetDimension.empty())
    {
      throw DataFormatException(noisyNSetFileName.c_str(), lineNb, "no element in some dimension");
    }
  sort(nSetDimension.begin(), nSetDimension.end());
  nSetDimension.erase(unique(nSetDimension.begin(), nSetDimension.end()), nSetDimension.end());
  return nSetDimension;
}
