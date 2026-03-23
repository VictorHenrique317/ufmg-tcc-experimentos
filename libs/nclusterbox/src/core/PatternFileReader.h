// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PATTERN_FILE_READER_H_
#define PATTERN_FILE_READER_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

class PatternFileReader
{
 public:
  PatternFileReader();

  void openFile(const char* noisyNSetFileName, const char* inputDimensionSeparator, const char* inputElementSeparator);
  void read(unsigned long long maxNbOfInitialPatterns);

 private:
  string noisyNSetFileName;
  istream noisyNSetStream;
  ifstream noisyNSetFile;
  vector<unordered_map<string, unsigned int>> labels2Ids;
  char_separator<char> inputDimensionSeparator;
  char_separator<char> inputElementSeparator;
  unsigned int lineNb;

  vector<unsigned int> getDimension(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd, const unordered_map<string, unsigned int>& labels2Ids) const;
  vector<unsigned int> getNonVertexDimension(const tokenizer<char_separator<char>>::const_iterator dimensionIt, const tokenizer<char_separator<char>>::const_iterator dimensionEnd, const unordered_map<string, unsigned int>& labels2Ids) const;
};

#endif /*PATTERN_FILE_READER_H_*/
