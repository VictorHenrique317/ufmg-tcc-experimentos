// Copyright 2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DATA_FILE_READER_H_
#define DATA_FILE_READER_H_

#include <unordered_map>
#include <fstream>
#include <boost/tokenizer.hpp>

#include "FuzzyTuple.h"

using namespace boost;

class DataFileReader
{
 public:
  DataFileReader(const char* tensorFileName, const char* elementSeparator);

  virtual ~DataFileReader();

  vector<vector<string>>& getIds2Labels();

 protected:
  const string tensorFileName;
  istream tensorStream;
  vector<vector<string>> ids2Labels;
  vector<unordered_map<string, unsigned int>> labels2Ids;
  vector<vector<unsigned int>> nSet;
  vector<vector<unsigned int>::const_iterator> tupleIts;

  const string nextLine();
  void initTupleEnumeration(const unsigned int n);
  vector<FuzzyTuple>& terminate(vector<FuzzyTuple>& fuzzyTuples);
  void parseDimension(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd, const vector<vector<unsigned int>>::iterator nSetIt, unordered_map<string, unsigned int>& labels2IdsInDimension, vector<string>& ids2LabelsInDimension);
  void throwIfNotEndOfRecord(const bool isNotEndOfRecord);
  double readMembership(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd);

 private:
  ifstream tensorFile;
  const char_separator<char> elementSeparator;
  unsigned int lineNb;
};

#endif /*DATA_FILE_READER_H_*/
