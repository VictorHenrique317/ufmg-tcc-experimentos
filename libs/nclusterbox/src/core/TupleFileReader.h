// Copyright 2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef TUPLE_FILE_READER_H_
#define TUPLE_FILE_READER_H_

#include "DataFileReader.h"

class TupleFileReader final : public DataFileReader
{
 public:
  TupleFileReader(const char* tensorFileName, const char* dimensionSeparator, const char* elementSeparator);

  vector<FuzzyTuple> read(); /* returns the unique fuzzy tuples, ordered by FuzzyTuple::operator< */

 private:
  const char_separator<char> dimensionSeparator;

  void init();
  void parseLine(const tokenizer<char_separator<char>>& dimensions);
};

#endif /*TUPLE_FILE_READER_H_*/
