// Copyright 2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "TupleFileReader.h"

#include "../utilities/UsageException.h"

TupleFileReader::TupleFileReader(const char* tensorFileNameParam, const char* dimensionSeparatorParam, const char* elementSeparatorParam): DataFileReader(tensorFileNameParam, elementSeparatorParam), dimensionSeparator(dimensionSeparatorParam)
{
  init();
}

void TupleFileReader::init()
{
  if (tensorStream.eof())
    {
      throw UsageException(("No tuple in " + tensorFileName + '!').c_str());
    }
  const string line = nextLine();
  const tokenizer<char_separator<char>> dimensions(line, dimensionSeparator);
  if (dimensions.begin() == dimensions.end())
    {
      init();
      return;
    }
  initTupleEnumeration(std::distance(dimensions.begin(), dimensions.end()));
  ids2Labels.resize(nSet.size());
  labels2Ids.resize(nSet.size());
  parseLine(dimensions);
}

vector<FuzzyTuple> TupleFileReader::read()
{
  const vector<vector<unsigned int>::const_iterator>::iterator tupleItsEnd = tupleIts.end();
  const vector<vector<unsigned int>>::const_iterator nSetEnd = nSet.end();
  for (vector<FuzzyTuple> fuzzyTuples; ; )
    {
      fuzzyTuples.emplace_back(tupleIts, 1.);
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
	      if (dimensions.begin() != dimensions.end())
		{
		  parseLine(dimensions);
		  break;
		}
	    }
	}
    }
}

void TupleFileReader::parseLine(const tokenizer<char_separator<char>>& dimensions)
{
  vector<vector<unsigned int>::const_iterator>::iterator tupleItsIt = tupleIts.begin();
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  vector<vector<unsigned int>>::iterator nSetIt = nSet.begin();
  const tokenizer<char_separator<char>>::const_iterator dimensionEnd = dimensions.end();
  tokenizer<char_separator<char>>::const_iterator dimensionIt = dimensions.begin();
  for (unordered_map<string, unsigned int>& labels2IdsInDimension : labels2Ids)
    {
      parseDimension(dimensionIt, dimensionEnd, nSetIt, labels2IdsInDimension, *ids2LabelsIt);
      *tupleItsIt++ = nSetIt->begin();
      ++ids2LabelsIt;
      ++nSetIt;
      ++dimensionIt;
    }
  throwIfNotEndOfRecord(dimensionIt != dimensionEnd);
}
