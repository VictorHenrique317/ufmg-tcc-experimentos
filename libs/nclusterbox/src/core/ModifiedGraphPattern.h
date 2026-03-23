// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef MODIFIED_GRAPH_PATTERN_H_
#define MODIFIED_GRAPH_PATTERN_H_

#include "ModifiedPattern.h"

class ModifiedGraphPattern final : public ModifiedPattern
{
 public:
  static void modify();
  static void grow();

 private:
  ModifiedGraphPattern();
  ~ModifiedGraphPattern();

  void doModifyGraphPattern();
  void doGrowGraphPattern();

  bool doGrowingStepForGraphPattern(const vector<vector<unsigned int>>& firstNonInitialAndSubsequentInitial); // returns whether to go on

  void computeAllSums();
  void init();
  bool insertOrErase();

#ifdef PRUNE
#ifdef UPDATE_SUMS
  void increaseFirstCandidatesInGraphPattern();
  void considerInsertingElementsBeforeFirstCandidatesInGraphPattern();
#endif

  void computeFirstCandidateElements();
#endif
};

#endif	/* MODIFIED_GRAPH_PATTERN_H_ */
