// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENSE_ROUGH_GRAPH_TENSOR_H_
#define DENSE_ROUGH_GRAPH_TENSOR_H_

#include "AbstractDenseRoughTensor.h"
#include "AbstractRoughGraphTensor.h"

class DenseRoughGraphTensor final : public AbstractDenseRoughTensor, public AbstractRoughGraphTensor
{
 public:
  DenseRoughGraphTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose);
  DenseRoughGraphTensor(vector<FuzzyTuple>& fuzzyEdges, const double constantShift, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension);

  AbstractTrie* getTensor() const;
  VisitedGraphPatterns* getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelection) const;
  AbstractTrieWithPrediction* projectTensor();
};

#endif /*DENSE_ROUGH_GRAPH_TENSOR_H_*/
