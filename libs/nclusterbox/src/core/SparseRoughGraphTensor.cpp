// Copyright 2023,2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseRoughGraphTensor.h"

#include <sys/resource.h>

#include "ConcurrentPatternPool.h"
#include "VertexTrie.h"
#include "VertexLastTrie.h"
#include "VertexTrieWithPrediction.h"
#include "VertexLastTrieWithPrediction.h"

SparseRoughGraphTensor::SparseRoughGraphTensor(vector<FuzzyTuple>& fuzzyTuplesParam, const double shiftParam): AbstractSparseRoughTensor(fuzzyTuplesParam, shiftParam)
{
}

AbstractTrie* SparseRoughGraphTensor::getTensor() const
{
  if (cardinalities.size() == 1)
    {
      VertexLastTrie* tensor = new VertexLastTrie(cardinalities.front());
      fillTensor(fuzzyTuples, *tensor);
      return tensor;
    }
  if (ConcurrentPatternPool::vertexDimension)
    {
      Trie* tensor = new Trie(cardinalities.front(), ++cardinalities.begin(), --cardinalities.end(), ConcurrentPatternPool::vertexDimension);
      fillTensor(fuzzyTuples, *tensor);
      return tensor;
    }
  VertexTrie* tensor = new VertexTrie(cardinalities.begin(), --cardinalities.end());
  fillTensor(fuzzyTuples, *tensor);
  return tensor;
}

VisitedGraphPatterns* SparseRoughGraphTensor::getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelectionParam) const
{
  isNoSelection = isNoSelectionParam;
  const unsigned int totalNbOfElements = nbOfElements();
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  float remainingBytes = availableBytes - 1024 * ru.ru_maxrss - VisitedPattern::init(cardinalities, cardinalities.end()) * sizeof(pair<mutex, vector<const VisitedPattern*>>) - 2 * nbOfJobs * (cardinalities.size() * sizeof(vector<int>) + totalNbOfElements * sizeof(int)); // remove the currently used memory, the memory for VisitedPatterns::firstTuples, and the memory for the nbOfJobs ModifiedPattern::nSet and ModifiedPattern::sumsOnHyperplanes
  if (isNoSelection)
    {
      // fuzzyTuples will be cleared and freed soon
      remainingBytes += fuzzyTuples.capacity() * sizeof(FuzzyTuple);
    }
  if (remainingBytes < sizeof(VisitedPattern) + 5 * sizeof(VisitedPattern*) + (sizeof(unsigned int) * (1 + totalNbOfElements - cardinalities.size()) / 4096 + 1) * (4096 + sizeof(VisitedPattern*)))
    {
      // Not enough space for the largest possible VisitedPattern
      return nullptr;
    }
  return new VisitedGraphPatterns(remainingBytes);
}

void SparseRoughGraphTensor::remapElementsToProject(const unsigned int internalDimensionId, const vector<unsigned int>& mapping)
{
  if (internalDimensionId < ConcurrentPatternPool::vertexDimension)
    {
      FuzzyTuple::remapElementsToProject(internalDimensionId, mapping, fuzzyTuples);
      return;
    }
  FuzzyTuple::remapElementsToProject(internalDimensionId + 1, mapping, fuzzyTuples);
}

bool SparseRoughGraphTensor::wouldBeEmptyAfterProjection()
{
  // Update cardinalities, ids2Labels, candidateVariables and fuzzyTuples
  FuzzyTuple::remapElementsToProject(ConcurrentPatternPool::vertexDimension, ConcurrentPatternPool::vertexDimension + 1, projectMetadataForDimension(ConcurrentPatternPool::vertexDimension, true, ids2VertexLabels), fuzzyTuples);
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
  for (; *internalDimensionIdIt != ConcurrentPatternPool::vertexDimension; ++ids2LabelsIt)
    {
      remapElementsToProject(*internalDimensionIdIt, projectMetadataForDimension(*internalDimensionIdIt, true, *ids2LabelsIt));
      ++internalDimensionIdIt;
    }
  for (; *++internalDimensionIdIt != ConcurrentPatternPool::vertexDimension; ++ids2LabelsIt)
    {
      remapElementsToProject(*internalDimensionIdIt, projectMetadataForDimension(*internalDimensionIdIt, true, *ids2LabelsIt));
    }
  for (const vector<unsigned int>::const_iterator internalDimensionIdEnd = external2InternalDimensionOrder.end(); ++internalDimensionIdIt != internalDimensionIdEnd; ++ids2LabelsIt)
    {
      remapElementsToProject(*internalDimensionIdIt, projectMetadataForDimension(*internalDimensionIdIt, true, *ids2LabelsIt));
    }
  return fuzzyTuples.empty();
}

AbstractTrieWithPrediction* SparseRoughGraphTensor::projectTensor()
{
  // Construct tensor for selection
  if (cardinalities.size() == 1)
    {
      setUnitForProjectedTensor((cardinalities.front() - 1) * shift);
      VertexLastTrieWithPrediction* tensor = new VertexLastTrieWithPrediction(cardinalities.front());
      fillProjectedTensor(*tensor);
      return tensor;
    }
  const vector<unsigned int>::const_iterator lastCardinalityIt = --cardinalities.end();
  {
    // Update unit
    vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
    unsigned int areaOfElementInFirstDimension = ids2VertexLabels.size() - 1;
    do
      {
	areaOfElementInFirstDimension *= *++cardinalityIt;
      }
    while (cardinalityIt != lastCardinalityIt);
    setUnitForProjectedTensor(areaOfElementInFirstDimension * shift);
  }
  AbstractTrieWithPrediction* tensor;
  if (ConcurrentPatternPool::vertexDimension)
    {
      tensor = new TrieWithPrediction(cardinalities.front(), ++cardinalities.begin(), lastCardinalityIt, ConcurrentPatternPool::vertexDimension);
    }
  else
    {
      tensor = new VertexTrieWithPrediction(cardinalities.front(), ++cardinalities.begin(), lastCardinalityIt);
    }
  fillProjectedTensor(*tensor);
  return tensor;
}
