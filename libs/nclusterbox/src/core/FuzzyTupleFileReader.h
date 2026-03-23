// Copyright 2018-2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FUZZY_TUPLE_FILE_READER_H_
#define FUZZY_TUPLE_FILE_READER_H_

#include "DataFileReader.h"

class FuzzyTupleFileReader : public DataFileReader
{
 public:
  FuzzyTupleFileReader(const char* tensorFileName, const char* dimensionSeparator, const char* elementSeparator);

  pair<vector<FuzzyTuple>, bool> read(); /* returns the unique fuzzy tuples, ordered by FuzzyTuple::operator<, and whether they all have memberships equal to 1 (or 0, but these are not returned) */

 private:
  const char_separator<char> dimensionSeparator;
  double membership;

  void init();			/* membership remains 0 if and only if the tensor is empty */
  bool parseLine(const tokenizer<char_separator<char>>& dimensions); /* returns whether the line is to be considered (nonzero membership) */
};

#endif /*FUZZY_TUPLE_FILE_READER_H_*/
