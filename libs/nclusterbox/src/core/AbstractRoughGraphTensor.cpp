// Copyright 2023-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "DenseRoughGraphTensor.h"
#include "SparseRoughGraphTensor.h"

#include <iostream>

#include "EdgeFileReader.h"
#include "FuzzyEdgeFileReader.h"
#include "ConcurrentPatternPool.h"
#include "LastTrie.h"
#include "SparseCrispTube.h"
#include "SparseFuzzyTube.h"

vector<string> AbstractRoughGraphTensor::ids2VertexLabels;

AbstractRoughGraphTensor::~AbstractRoughGraphTensor()
{
}

unsigned long long AbstractRoughGraphTensor::graphPatternArea(const vector<vector<unsigned int>>& nSet)
{
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  unsigned long long area = dimensionIt->size();
  for (const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end(); ++dimensionIt != dimensionEnd; )
    {
      area *= dimensionIt->size();
    }
  return area * (nSet[ConcurrentPatternPool::vertexDimension].size() - 1) / 2;
}

void AbstractRoughGraphTensor::printPattern(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const
{
  vector<vector<string>>::const_iterator ids2LabelsIt = ids2Labels.begin();
  vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
  for (; *internalDimensionIdIt != ConcurrentPatternPool::vertexDimension; ++internalDimensionIdIt)
    {
      printDimension(nSet[*internalDimensionIdIt], *ids2LabelsIt, out);
      ++ids2LabelsIt;
    }
  printDimension(nSet[ConcurrentPatternPool::vertexDimension], ids2VertexLabels, out);
  while (*++internalDimensionIdIt != ConcurrentPatternPool::vertexDimension)
    {
      printDimension(nSet[*internalDimensionIdIt], *ids2LabelsIt, out);
      ++ids2LabelsIt;
    }
  printDimension(nSet[ConcurrentPatternPool::vertexDimension], ids2VertexLabels, out);
  for (const vector<unsigned int>::const_iterator internalDimensionIdEnd = external2InternalDimensionOrder.end(); ++internalDimensionIdIt != internalDimensionIdEnd; )
    {
      printDimension(nSet[*internalDimensionIdIt], *ids2LabelsIt, out);
      ++ids2LabelsIt;
    }
  printPatternDensityAndSizes(nSet, density, out);
  if (isAreaPrinted)
    {
      out << areaPrefix << graphPatternArea(nSet);
    }
}

AbstractRoughGraphTensor* AbstractRoughGraphTensor::makeRoughTensor(vector<FuzzyTuple>& fuzzyEdges, const double densityThreshold, const double shift, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension)
{
  if (LastTrie::is01)
    {
      if ((8 * sizeof(double) + 1) * getAreaFromIds2Labels() < 8 * (sizeof(FuzzyTuple) + sizeof(unsigned int) * (ids2Labels.size() + 1)) * fuzzyEdges.size())
      	{
	  // Dense storage (including the rough tensor) takes less space, assuming the sparse storage would only use sparse tubes
	  cerr << "Currently, storing the edges sparsely, despite their high density\n";
	  // TODO: instead of the above warning
	  // return new DenseRoughGraphTensor(fuzzyEdges, shift, firstVertexDimension, secondVertexDimension);
      	}
      setMetadata(fuzzyEdges, shift, firstVertexDimension, secondVertexDimension);
      SparseCrispTube::setDefaultMembershipAndDensityLimit(unit * -shift, densityThreshold / sizeof(unsigned int) / 8);
      return new SparseRoughGraphTensor(fuzzyEdges, shift);
    }
  if ((sizeof(double) + sizeof(int)) * getAreaFromIds2Labels() < (sizeof(FuzzyTuple) + sizeof(unsigned int) * ids2Labels.size() + sizeof(pair<unsigned int, int>)) * fuzzyEdges.size())
    {
      // Dense storage (including the rough tensor) takes less space, assuming the sparse storage would only use sparse tubes
      cerr << "Currently, storing the edges sparsely, despite their high density\n";
      // TODO: instead of the above warning
      // return new DenseRoughGraphTensor(fuzzyEdges, shift, firstVertexDimension, secondVertexDimension);
    }
  setMetadata(fuzzyEdges, shift, firstVertexDimension, secondVertexDimension);
  SparseFuzzyTube::setDefaultMembershipAndDensityLimit(unit * -shift, densityThreshold * sizeof(int) / sizeof(pair<unsigned int, int>));
  return new SparseRoughGraphTensor(fuzzyEdges, shift);
}

AbstractRoughGraphTensor* AbstractRoughGraphTensor::makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose)
{
  vector<FuzzyTuple> fuzzyEdges = getFuzzyEdges(tensorFileName, inputDimensionSeparator, inputElementSeparator, isInput01, firstVertexDimension, secondVertexDimension, isVerbose);
  return makeRoughTensor(fuzzyEdges, densityThreshold, membershipSum(fuzzyEdges) / getAreaFromIds2Labels(), firstVertexDimension, secondVertexDimension);
}

AbstractRoughGraphTensor* AbstractRoughGraphTensor::makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const double shift, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose)
{
  vector<FuzzyTuple> fuzzyEdges = getFuzzyEdges(tensorFileName, inputDimensionSeparator, inputElementSeparator, isInput01, firstVertexDimension, secondVertexDimension, isVerbose);
  return makeRoughTensor(fuzzyEdges, densityThreshold, shift, firstVertexDimension, secondVertexDimension);
}

const vector<string>& AbstractRoughGraphTensor::getIds2VertexLabels()
{
  return ids2VertexLabels;
}

unsigned long long AbstractRoughGraphTensor::getArea()
{
  vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
  unsigned long long area = *cardinalityIt;
  for (const vector<unsigned int>::const_iterator cardinalityEnd = cardinalities.end(); ++cardinalityIt != cardinalityEnd; )
    {
      area *= *cardinalityIt;
    }
  return area * (ids2VertexLabels.size() - 1) / 2;
}

vector<FuzzyTuple> AbstractRoughGraphTensor::getFuzzyEdges(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose)
{
#if defined TIME || defined DETAILED_TIME
  overallBeginning = steady_clock::now();
#endif
  if (isInput01)
    {
      if (isVerbose)
	{
	  cout << "Parsing Boolean tensor ... " << flush;
	}
      LastTrie::is01 = true;
      EdgeFileReader edgeFileReader(tensorFileName, inputDimensionSeparator, inputElementSeparator, firstVertexDimension, secondVertexDimension);
      vector<FuzzyTuple> edges = edgeFileReader.read();
      ids2Labels = std::move(edgeFileReader.getIds2Labels());
      ids2VertexLabels = std::move(edgeFileReader.getIds2VertexLabels());
      if (isVerbose)
	{
	  cout << "\rParsing Boolean tensor: " << edges.size() << '/' << getAreaFromIds2Labels() << " edges with nonzero membership degrees.\n" << flush;
	}
#ifdef DETAILED_TIME
      shiftingBeginning = steady_clock::now();
#ifdef GNUPLOT
      cout << duration_cast<duration<double>>(shiftingBeginning - overallBeginning).count();
#else
      cout << "Tensor parsing time: " << duration_cast<duration<double>>(shiftingBeginning - overallBeginning).count() << "s\n";
#endif
#endif
      if (isVerbose)
	{
	  cout << "Shifting tensor ... " << flush;
	}
      return edges;
    }
  if (isVerbose)
    {
      cout << "Parsing fuzzy tensor ... " << flush;
    }
  FuzzyEdgeFileReader fuzzyEdgeFileReader(tensorFileName, inputDimensionSeparator, inputElementSeparator, firstVertexDimension, secondVertexDimension);
  pair<vector<FuzzyTuple>, bool> fuzzyEdgesAndIs01 = fuzzyEdgeFileReader.read();
  ids2Labels = std::move(fuzzyEdgeFileReader.getIds2Labels());
  ids2VertexLabels = std::move(fuzzyEdgeFileReader.getIds2VertexLabels());
  LastTrie::is01 = fuzzyEdgesAndIs01.second;
  if (isVerbose)
    {
      cout << "\rParsing fuzzy tensor: " << fuzzyEdgesAndIs01.first.size() << '/' << getAreaFromIds2Labels() << " edges with nonzero membership degrees.\n" << flush;
    }
#ifdef DETAILED_TIME
  shiftingBeginning = steady_clock::now();
#ifdef GNUPLOT
  cout << duration_cast<duration<double>>(shiftingBeginning - overallBeginning).count();
#else
  cout << "Tensor parsing time: " << duration_cast<duration<double>>(shiftingBeginning - overallBeginning).count() << "s\n";
#endif
#endif
  if (isVerbose)
    {
      cout << "Shifting tensor ... " << flush;
    }
  return fuzzyEdgesAndIs01.first;
}

unsigned long long AbstractRoughGraphTensor::getAreaFromIds2Labels()
{
  unsigned long long area = ids2VertexLabels.size();
  area *= area - 1;
  area /= 2;
  for (const vector<string>& ids2LabelsInDimension : ids2Labels)
    {
      area *= ids2LabelsInDimension.size();
    }
  return area;
}

void AbstractRoughGraphTensor::orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities(const unsigned int firstVertexDimension, const unsigned int secondVertexDimension)
{
  vector<vector<string>>::const_iterator ids2LabelsInDimensionIt = ids2Labels.begin();
  unsigned int n = ids2Labels.end() - ids2LabelsInDimensionIt;
  vector<pair<unsigned int, unsigned int>> dimensions;
  dimensions.reserve(n);
  unsigned int dimensionId = 0;
  for (; dimensionId != firstVertexDimension; ++dimensionId)
    {
      dimensions.emplace_back(ids2LabelsInDimensionIt->size(), dimensionId);
      ++ids2LabelsInDimensionIt;
    }
  for (; ++dimensionId != secondVertexDimension; ++ids2LabelsInDimensionIt)
    {
      dimensions.emplace_back(ids2LabelsInDimensionIt->size(), dimensionId);
    }
  for (n += 2; ++dimensionId != n; ++ids2LabelsInDimensionIt)
    {
      dimensions.emplace_back(ids2LabelsInDimensionIt->size(), dimensionId);
    }
  dimensionId = 0;
  // Sort non-vertex dimensions by increasing cardinality
  sort(dimensions.begin(), dimensions.end(), [](const pair<unsigned int, unsigned int>& dimension1, const pair<unsigned int, unsigned int>& dimension2) {return dimension1.first < dimension2.first;});
  external2InternalDimensionOrder.resize(n);
  cardinalities.reserve(--n);
  vector<pair<unsigned int, unsigned int>>::const_iterator dimensionIt = dimensions.begin();
  {
    // Until the internal vertex dimension
    const unsigned int nbOfVertices = ids2VertexLabels.size();
    for (--n; dimensionId != n && dimensionIt->first < nbOfVertices; ++dimensionIt)
      {
	external2InternalDimensionOrder[dimensionIt->second] = dimensionId++;
	cardinalities.push_back(dimensionIt->first);
      }
    cardinalities.push_back(nbOfVertices);
  }
  ConcurrentPatternPool::vertexDimension = dimensionId;
  external2InternalDimensionOrder[firstVertexDimension] = dimensionId;
  // For FuzzyTuple::reorder, external2InternalDimensionOrder considers the trie depth (changed later, in setMetadata)
  external2InternalDimensionOrder[secondVertexDimension] = ++dimensionId;
  // After the internal vertex dimension
  for (++n; dimensionId != n; ++dimensionIt)
    {
      external2InternalDimensionOrder[dimensionIt->second] = ++dimensionId;
      cardinalities.push_back(dimensionIt->first);
    }
}

void AbstractRoughGraphTensor::setMetadataForVertexDimension(const unsigned long long area, const double shift, double& unitDenominator, vector<FuzzyTuple>& fuzzyTuples, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension)
{
  // Sparse tensor
  const unsigned int nbOfVertices = ids2VertexLabels.size();
  // Computing positive and, for fuzzy tensors, negative memberships of the elements
  if (LastTrie::is01)
    {
      // shifts not subtracted because Trie::sumsOnPatternAndHyperplanes multiplies by unit (the product cannot overflow) before subtracting the shifts
      vector<pair<unsigned int, unsigned int>> elementPositiveMemberships = pairs0AndId<unsigned int>(nbOfVertices);
      {
	const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
	vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
	do
	  {
	    ++elementPositiveMemberships[fuzzyTupleIt->getElementId(firstVertexDimension)].first;
	    ++elementPositiveMemberships[fuzzyTupleIt->getElementId(secondVertexDimension)].first;
	  }
	while (++fuzzyTupleIt != fuzzyTupleEnd);
      }
      FuzzyTuple::remapElements(firstVertexDimension, secondVertexDimension, sortByPositiveMemberships<unsigned int>(elementPositiveMemberships, ids2VertexLabels), fuzzyTuples);
      if (elementPositiveMemberships.back().first > unitDenominator)
	{
	  unitDenominator = elementPositiveMemberships.back().first;
	}
      return;
    }
  // !is01
  vector<pair<double, unsigned int>> elementPositiveMemberships = pairs0AndId<double>(nbOfVertices);
  {
    vector<double> elementNegativeMemberships(nbOfVertices, 2 * shift * (area / nbOfVertices)); // assumes every membership null and correct that in the loop below
    {
      const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
      vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
      do
	{
	  const unsigned int firstVertexId = fuzzyTupleIt->getElementId(firstVertexDimension);
	  const unsigned int secondVertexId = fuzzyTupleIt->getElementId(secondVertexDimension);
	  const double membership = fuzzyTupleIt->getMembership();
	  if (membership > 0)
	    {
	      elementPositiveMemberships[firstVertexId].first += membership;
	      elementPositiveMemberships[secondVertexId].first += membership;
	      elementNegativeMemberships[firstVertexId] -= shift;
	      elementNegativeMemberships[secondVertexId] -= shift;
	    }
	  else
	    {
	      elementNegativeMemberships[firstVertexId] -= membership + shift;
	      elementNegativeMemberships[secondVertexId] -= membership + shift;
	    }
	}
      while (++fuzzyTupleIt != fuzzyTupleEnd);
    }
    const double maxNegativeMembership = *max_element(elementNegativeMemberships.begin(), elementNegativeMemberships.end());
    if (maxNegativeMembership > unitDenominator)
      {
	unitDenominator = maxNegativeMembership;
      }
  }
  FuzzyTuple::remapElements(firstVertexDimension, secondVertexDimension, sortByPositiveMemberships<double>(elementPositiveMemberships, ids2VertexLabels), fuzzyTuples);
  if (elementPositiveMemberships.back().first > unitDenominator)
    {
      unitDenominator = elementPositiveMemberships.back().first;
    }
}

void AbstractRoughGraphTensor::setMetadata(vector<FuzzyTuple>& fuzzyTuples, const double shift, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension)
{
  // Sparse tensor
  orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities(firstVertexDimension, secondVertexDimension);
  const unsigned long long area = getArea();
  shiftTuplesAndSetNullModelRSS(fuzzyTuples, shift, area);
  double unitDenominator = unitDenominatorGivenNullModelRSS();
  if (LastTrie::is01)
    {
      double maxElementNegativeMembership;
      if (ids2VertexLabels.size() < 2 * cardinalities.front())
	{
	  maxElementNegativeMembership = 2 * shift * (area / ids2VertexLabels.size());
	}
      else
	{
	  maxElementNegativeMembership = shift * (area / cardinalities.front());
	}
      if (maxElementNegativeMembership > unitDenominator)
	{
	  unitDenominator = maxElementNegativeMembership;
	}
    }
  setMetadataForVertexDimension(area, shift, unitDenominator, fuzzyTuples, firstVertexDimension, secondVertexDimension);
  {
    vector<vector<string>>::iterator ids2LabelsInDimensionIt = ids2Labels.begin();
    unsigned int dimensionId = 0;
    for (; dimensionId != firstVertexDimension; ++dimensionId)
      {
	setMetadataForDimension(dimensionId, area, shift, unitDenominator, *ids2LabelsInDimensionIt, fuzzyTuples);
	++ids2LabelsInDimensionIt;
      }
    while (++dimensionId != secondVertexDimension)
      {
	setMetadataForDimension(dimensionId, area, shift, unitDenominator, *ids2LabelsInDimensionIt, fuzzyTuples);
	++ids2LabelsInDimensionIt;
      }
    for (const unsigned int n = ids2Labels.size() + 2; ++dimensionId != n; )
      {
	setMetadataForDimension(dimensionId, area, shift, unitDenominator, *ids2LabelsInDimensionIt, fuzzyTuples);
	++ids2LabelsInDimensionIt;
      }
  }
  unit = static_cast<double>(numeric_limits<int>::max()) / unitDenominator;
  const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  do
    {
      fuzzyTupleIt->reorder(external2InternalDimensionOrder);
      fuzzyTupleIt->ifSmallerIdSwapWithNext(ConcurrentPatternPool::vertexDimension);
      ConcurrentPatternPool::addFuzzyTuple(fuzzyTupleIt->getTuple(), fuzzyTupleIt->getMembership());
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
  // orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities defined external2InternalDimensionOrder as in AbstractRoughTensor, what is suited to call FuzzyTuple::reorder, above; now, for printPattern, decrement every value exceeding ConcurrentPatternPool::vertexDimension
  vector<unsigned int>::iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
  if (*internalDimensionIdIt > ConcurrentPatternPool::vertexDimension)
    {
      --*internalDimensionIdIt;
    }
  ++internalDimensionIdIt;
  const vector<unsigned int>::iterator internalDimensionIdEnd = external2InternalDimensionOrder.end();
  do
    {
      if (*internalDimensionIdIt > ConcurrentPatternPool::vertexDimension)
	{
	  --*internalDimensionIdIt;
	}
    }
  while (++internalDimensionIdIt != internalDimensionIdEnd);
}

// // TODO: consider symmetry (this is copied from AbstractRoughTensor)
// void AbstractRoughGraphTensor::setMetadataForDimension(vector<pair<double, unsigned int>>& elementPositiveMembershipsInDimension, double& unitDenominator, vector<string>& ids2LabelsInDimension)
// {
//   // Dense tensor
//   sort(elementPositiveMembershipsInDimension.begin(), elementPositiveMembershipsInDimension.end(), [](const pair<double, unsigned int>& elementPositiveMembership1, const pair<double, unsigned int>& elementPositiveMembership2) {return elementPositiveMembership1.first < elementPositiveMembership2.first;});
//   // Computing the new ids, in increasing order of the positive membership (for faster lower_bound in SparseFuzzyTube::sumOnSlice and to choose the element with the greatest membership in case of equality) and reorder ids2LabelsInDimension accordingly
//   vector<string> newIds2LabelsInDimension;
//   newIds2LabelsInDimension.reserve(ids2LabelsInDimension.size());
//   const vector<pair<double, unsigned int>>::const_iterator elementPositiveMembershipEnd = elementPositiveMembershipsInDimension.end();
//   vector<pair<double, unsigned int>>::const_iterator elementPositiveMembershipIt = elementPositiveMembershipsInDimension.begin();
//   do
//     {
//       newIds2LabelsInDimension.emplace_back(std::move(ids2LabelsInDimension[elementPositiveMembershipIt->second]));
//     }
//   while (++elementPositiveMembershipIt != elementPositiveMembershipEnd);
//   ids2LabelsInDimension = std::move(newIds2LabelsInDimension);
//   if (elementPositiveMembershipsInDimension.back().first > unitDenominator)
//     {
//       unitDenominator = elementPositiveMembershipsInDimension.back().first;
//     }
// }

// // TODO: consider symmetry (this is copied from AbstractRoughTensor)
// void AbstractRoughGraphTensor::setMetadata(vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, const double maxNegativeMembership, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension)
// {
//   // Dense tensor
//   orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities(firstVertexDimension, secondVertexDimension);
//   double unitDenominator = unitDenominatorGivenNullModelRSS();
//   if (maxNegativeMembership > unitDenominator)
//     {
//       unitDenominator = maxNegativeMembership;
//     }
//   vector<vector<pair<double, unsigned int>>>::iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
//   vector<vector<string>>::iterator ids2LabelsInDimensionIt = ids2Labels.begin();
//   setMetadataForDimension(*elementPositiveMembershipsInDimensionIt, unitDenominator, *ids2LabelsInDimensionIt);
//   ++ids2LabelsInDimensionIt;
//   const vector<vector<string>>::iterator ids2LabelsInDimensionEnd = ids2Labels.end();
//   do
//     {
//       setMetadataForDimension(*++elementPositiveMembershipsInDimensionIt, unitDenominator, *ids2LabelsInDimensionIt);
//     }
//   while (++ids2LabelsInDimensionIt != ids2LabelsInDimensionEnd);
//   unit = static_cast<double>(numeric_limits<int>::max()) / unitDenominator;
// }

#if defined DEBUG_MODIFY || defined ASSERT
void AbstractRoughGraphTensor::printElement(const unsigned int dimensionId, const unsigned int elementId, ostream& out) const
{
  if (dimensionId == ConcurrentPatternPool::vertexDimension)
    {
      out << "vertex " << ids2VertexLabels[elementId];
      return;
    }
  AbstractRoughTensor::printElement(dimensionId, elementId, out);
}

unsigned long long AbstractRoughGraphTensor::area(const vector<vector<unsigned int>>& nSet) const
{
  return graphPatternArea(nSet);
}
#endif
