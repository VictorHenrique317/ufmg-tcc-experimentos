// Copyright 2018-2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "SparseRoughTensor.h"

#include <sys/resource.h>

#include "Trie.h"
#include "LastTrie.h"
#include "TrieWithPrediction.h"
#include "LastTrieWithPrediction.h"

#include "SparseCrispTube.h"
#include "SparseFuzzyTube.h"

SparseRoughTensor::SparseRoughTensor(vector<FuzzyTuple>& fuzzyTuplesParam, const double shiftParam): AbstractSparseRoughTensor(fuzzyTuplesParam, shiftParam)
{
}

AbstractTrie* SparseRoughTensor::getTensor() const
{
  if (cardinalities.size() == 2)
    {
      LastTrie* tensor = new LastTrie(cardinalities.front());
      fillTensor(fuzzyTuples, *tensor);
      return tensor;
    }
  Trie* tensor = new Trie(cardinalities.front(), ++cardinalities.begin(), --cardinalities.end());
  fillTensor(fuzzyTuples, *tensor);
  return tensor;
}

VisitedPatterns* SparseRoughTensor::getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelectionParam) const
{
  isNoSelection = isNoSelectionParam;
  const unsigned int totalNbOfElements = nbOfElements();
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  float remainingBytes = availableBytes - 1024 * ru.ru_maxrss - VisitedPattern::init(cardinalities, --cardinalities.end()) * sizeof(pair<mutex, vector<const VisitedPattern*>>) - 2 * nbOfJobs * (cardinalities.size() * sizeof(vector<int>) + totalNbOfElements * sizeof(int)); // remove the currently used memory, the memory for VisitedPatterns::firstTuples, and the memory for the nbOfJobs ModifiedPattern::nSet and ModifiedPattern::sumsOnHyperplanes
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
  return new VisitedPatterns(remainingBytes);
}

bool SparseRoughTensor::wouldBeEmptyAfterProjection()
{
  // Update cardinalities, ids2Labels, candidateVariables and fuzzyTuples
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
  FuzzyTuple::remapElementsToProject(*internalDimensionIdIt, projectMetadataForDimension(*internalDimensionIdIt, true, *ids2LabelsIt), fuzzyTuples);
  ++internalDimensionIdIt;
  const vector<unsigned int>::const_iterator internalDimensionIdEnd = external2InternalDimensionOrder.end();
  do
    {
      FuzzyTuple::remapElementsToProject(*internalDimensionIdIt, projectMetadataForDimension(*internalDimensionIdIt, true, *++ids2LabelsIt), fuzzyTuples);
    }
  while (++internalDimensionIdIt != internalDimensionIdEnd);
  return fuzzyTuples.empty();
}

AbstractTrieWithPrediction* SparseRoughTensor::projectTensor()
{
  // Construct tensor for selection
  if (cardinalities.size() == 2)
    {
      setUnitForProjectedTensor(cardinalities[1] * shift);
      LastTrieWithPrediction* tensor = new LastTrieWithPrediction(cardinalities[0], cardinalities[1]);
      fillProjectedTensor(*tensor);
      return tensor;
    }
  const vector<unsigned int>::const_iterator lastCardinalityIt = --cardinalities.end();
  {
    // Update unit
    vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
    unsigned int areaOfElementInFirstDimension = *++cardinalityIt;
    do
      {
	areaOfElementInFirstDimension *= *++cardinalityIt;
      }
    while (cardinalityIt != lastCardinalityIt);
    setUnitForProjectedTensor(areaOfElementInFirstDimension * shift);
  }
  TrieWithPrediction* tensor = new TrieWithPrediction(cardinalities.front(), ++cardinalities.begin(), lastCardinalityIt);
  fillProjectedTensor(*tensor);
  return tensor;
}
