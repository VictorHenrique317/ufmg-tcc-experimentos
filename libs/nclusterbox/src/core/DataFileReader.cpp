// Copyright 2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "DataFileReader.h"

#include <iostream>
#include <boost/lexical_cast.hpp>

#include "../../Parameters.h"
#include "../utilities/NoInputException.h"
#include "../utilities/DataFormatException.h"

DataFileReader::DataFileReader(const char* tensorFileNameParam, const char* elementSeparatorParam): tensorFileName(tensorFileNameParam), tensorStream(nullptr), ids2Labels(), labels2Ids(), nSet(), tupleIts(), tensorFile(), elementSeparator(elementSeparatorParam), lineNb(0)
{
   if (tensorFileName == "-")
    {
      tensorStream.rdbuf(cin.rdbuf());
      return;
    }
   tensorFile.open(tensorFileNameParam);
   if (!tensorFile)
    {
      throw NoInputException(tensorFileNameParam);
    }
  tensorStream.rdbuf(tensorFile.rdbuf());
}

DataFileReader::~DataFileReader()
{
}

const string DataFileReader::nextLine()
{
  ++lineNb;
  string line;
  getline(tensorStream, line);
#ifdef VERBOSE_PARSER
  cout << tensorFileName << ':' << lineNb << ": " << line << '\n';
#endif
  return line;
}

void DataFileReader::initTupleEnumeration(const unsigned int n)
{
  if (n < 2)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, (lexical_cast<string>(n) + " dimension, but at least 2 required!").c_str());
    }
  nSet.resize(n);
  tupleIts.resize(n);
}

vector<FuzzyTuple>& DataFileReader::terminate(vector<FuzzyTuple>& fuzzyTuples)
{
  tensorFile.close();
  stable_sort(fuzzyTuples.begin(), fuzzyTuples.end());
  fuzzyTuples.erase(unique(fuzzyTuples.begin(), fuzzyTuples.end()), fuzzyTuples.end());
  fuzzyTuples.shrink_to_fit();
  return fuzzyTuples;
}

vector<vector<string>>& DataFileReader::getIds2Labels()
{
  return ids2Labels;
}

void DataFileReader::parseDimension(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd, const vector<vector<unsigned int>>::iterator nSetIt, unordered_map<string, unsigned int>& labels2IdsInDimension, vector<string>& ids2LabelsInDimension)
{
  if (dimensionIt == dimensionEnd)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("fewer than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
    }
  nSetIt->clear();
  tokenizer<char_separator<char>> elements(*dimensionIt, elementSeparator);
  const tokenizer<char_separator<char>>::const_iterator elementEnd = elements.end();
  tokenizer<char_separator<char>>::const_iterator elementIt = elements.begin();
  if (elementIt == elementEnd)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("no element in dimension " + lexical_cast<string>((nSetIt + 1) - nSet.begin()) + '!').c_str());
    }
  do
    {
      const pair<unordered_map<string, unsigned int>::const_iterator, bool> label2Id = labels2IdsInDimension.insert({*elementIt, ids2LabelsInDimension.size()});
      if (label2Id.second)
	{
	  ids2LabelsInDimension.push_back(*elementIt);
	}
      nSetIt->push_back(label2Id.first->second);
    }
  while (++elementIt != elementEnd);
}

void DataFileReader::throwIfNotEndOfRecord(const bool isNotEndOfRecord)
{
  if (isNotEndOfRecord)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("more than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
    }
}

double DataFileReader::readMembership(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd)
{
  double membership;
  if (dimensionIt == dimensionEnd)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("fewer than the expected " + lexical_cast<string>(nSet.size()) + " dimensions!").c_str());
    }
  try
    {
      membership = lexical_cast<double>(*dimensionIt);
      if (membership < 0 || membership > 1)
	{
	  throw bad_lexical_cast();
	}
    }
  catch (bad_lexical_cast &)
    {
      throw DataFormatException(tensorFileName.c_str(), lineNb, ("the membership, " + *dimensionIt + ", should be a double in [0, 1]!").c_str());
    }
  return membership;
}
