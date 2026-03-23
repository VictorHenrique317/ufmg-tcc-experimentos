// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "DenseRoughGraphTensor.h"

#include <sys/resource.h>

#include "VertexTrie.h"
#include "TrieWithPrediction.h"

DenseRoughGraphTensor::DenseRoughGraphTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension, const bool isVerbose): AbstractDenseRoughTensor()
{
  // TODO
  // vector<FuzzyTuple> fuzzyEdges = getFuzzyEdges(tensorFileName, inputDimensionSeparator, inputElementSeparator, isInput01, firstVertexDimension, secondVertexDimension, isVerbose);
  // LastTrie::is01 = false;
  // shift = new ExpectationShift(fuzzyEdges, ids2Labels, ids2VertexLabels);
  // init(fuzzyEdges);
}

DenseRoughGraphTensor::DenseRoughGraphTensor(vector<FuzzyTuple>& fuzzyEdges, const double constantShift, const unsigned int firstVertexDimension, const unsigned int secondVertexDimension): AbstractDenseRoughTensor(fuzzyEdges, constantShift)
{
  // TODO
  // init(fuzzyEdges);
  // if (LastTrie::is01)
  //   {
  //     SparseCrispTube::setDefaultMembership(unit * -constantShift);
  //   }
}

AbstractTrie* DenseRoughGraphTensor::getTensor() const
{
  // TODO
  return new Trie();
}

VisitedGraphPatterns* DenseRoughGraphTensor::getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelectionParam) const
{
  isNoSelection = isNoSelectionParam;
  const unsigned int totalNbOfElements = nbOfElements();
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  float remainingBytes = availableBytes - 1024 * ru.ru_maxrss - VisitedPattern::init(cardinalities, cardinalities.end()) * sizeof(pair<mutex, vector<const VisitedPattern*>>) - 2 * nbOfJobs * (cardinalities.size() * sizeof(vector<int>) + totalNbOfElements * sizeof(int)); // remove the currently used memory, the memory for VisitedPatterns::firstTuples, and the memory for the nbOfJobs ModifiedPattern::nSet and ModifiedPattern::sumsOnHyperplanes
  if (isNoSelection)
    {
      // memberships will be cleared and freed soon
      remainingBytes += memberships.capacity() * sizeof(double);
    }
  if (remainingBytes < sizeof(VisitedPattern) + 5 * sizeof(VisitedPattern*) + (sizeof(unsigned int) * (1 + totalNbOfElements - cardinalities.size()) / 4096 + 1) * (4096 + sizeof(VisitedPattern*)))
    {
      // Not enough space for the largest possible VisitedPattern
      return nullptr;
    }
  return new VisitedGraphPatterns(remainingBytes);
}

AbstractTrieWithPrediction* DenseRoughGraphTensor::projectTensor()
{
  // TODO
  return new TrieWithPrediction();
}
