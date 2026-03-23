// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef VISITED_GRAPH_PATTERNS_H_
#define VISITED_GRAPH_PATTERNS_H_

#include "VisitedPatterns.h"

class VisitedGraphPatterns final : public VisitedPatterns
{
public:
  VisitedGraphPatterns(const float bytesForVisitedPatterns);

  bool visited(const vector<vector<unsigned int>>& nSet, const double g);
};

#endif /*VISITED_GRAPH_PATTERNS_H_*/
