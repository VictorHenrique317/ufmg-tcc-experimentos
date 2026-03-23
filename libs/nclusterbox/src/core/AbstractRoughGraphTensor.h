// Copyright 2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_ROUGH_GRAPH_TENSOR_H_
#define ABSTRACT_ROUGH_GRAPH_TENSOR_H_

#include "AbstractRoughTensor.h"
#include "VisitedGraphPatterns.h"

class AbstractRoughGraphTensor : virtual public AbstractRoughTensor
{
 public:
  virtual ~AbstractRoughGraphTensor();

  void printPattern(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const;

  static AbstractRoughGraphTensor* makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose);
  static AbstractRoughGraphTensor* makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const double shift, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose);

  static unsigned long long graphPatternArea(const vector<vector<unsigned int>>& nSet);
  static const vector<string>& getIds2VertexLabels();
  static unsigned long long getArea();

#if defined DEBUG_MODIFY || defined ASSERT
  void printElement(const unsigned int dimensionId, const unsigned int elementId, ostream& out) const;
  unsigned long long area(const vector<vector<unsigned int>>& nSet) const;
#endif

 protected:
  static vector<string> ids2VertexLabels;

  static vector<FuzzyTuple> getFuzzyEdges(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose);
  static void orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities(const unsigned int firstVertexDimension, const unsigned int secondVertexDimension);
  static void setMetadata(vector<FuzzyTuple>& fuzzyTuples, const double shift, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension);
  // TODO: consider symmetry
  // static void setMetadata(vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, const double maxNegativeMembership, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension); /* the inner vectors of elementPositiveMemberships are reordered by increasing element membership, hence a mapping from new ids (the index) and old ids (the second components of the pairs) */

 private:
  static AbstractRoughGraphTensor* makeRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double densityThreshold, const double shift, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension);
  static unsigned long long getAreaFromIds2Labels();
  static void setMetadataForVertexDimension(const unsigned long long area, const double shift, double& unitDenominator, vector<FuzzyTuple>& fuzzyTuples, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension);
  // TODO: consider symmetry
  // static void setMetadataForDimension(vector<pair<double, unsigned int>>& elementPositiveMembershipsInDimension, double& unitDenominator, vector<string>& ids2LabelsInDimension);
};

#endif /*ABSTRACT_ROUGH_GRAPH_TENSOR_H_*/
