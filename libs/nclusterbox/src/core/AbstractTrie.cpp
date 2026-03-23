// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "AbstractTrie.h"
#include "SparseCrispTube.h"

AbstractTrie::~AbstractTrie()
{
}

#ifdef PRUNE
void AbstractTrie::positiveMembershipsOnHyperplanes(vector<vector<int>>& elementPositiveMemberships, const int unit) const
{
  // is01
  vector<vector<int>>::iterator sumsIt = elementPositiveMemberships.begin();
  positiveMembershipsOnHyperplanes(sumsIt);
  const vector<vector<int>>::iterator sumsEnd = elementPositiveMemberships.end();
  const int factor = unit + SparseCrispTube::getDefaultMembership();
  do
    {
      const vector<int>::iterator nbOfPresentTuplesEnd = sumsIt->end();
      vector<int>::iterator nbOfPresentTuplesIt = sumsIt->begin();
      do
	{
	  *nbOfPresentTuplesIt *= factor;
	}
      while (++nbOfPresentTuplesIt != nbOfPresentTuplesEnd);
    }
  while (++sumsIt != sumsEnd);
}

long long AbstractTrie::sumOnPattern(const vector<vector<unsigned int>>::const_iterator nSetBegin, const unsigned long long area, const int unit) const
{
  // is01
  return sumOnPattern(nSetBegin) * unit + SparseCrispTube::getDefaultMembership() * static_cast<long long>(area);
}

void AbstractTrie::sumsOnHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit) const
{
  // is01
  sumsOnHyperplanes(nSetBegin, firstCandidateElementBegin, sums);
  vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElementBegin;
  vector<vector<int>>::iterator sumsIt = sums.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSetBegin;
  scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / dimensionIt->size()), sumsIt->begin() + *firstCandidateElementIt, sumsIt->end());
  ++sumsIt;
  const vector<vector<int>>::iterator sumsEnd = sums.end();
  do
    {
      scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / (++dimensionIt)->size()), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
    }
  while (++sumsIt != sumsEnd);
}

void AbstractTrie::sumsOnGraphHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, const vector<unsigned int>::const_iterator firstCandidateElementBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit, const unsigned int vertexDimension) const
{
  // is01
  sumsOnHyperplanes(nSetBegin, firstCandidateElementBegin, sums);
  vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElementBegin;
  vector<vector<int>>::iterator sumsIt = sums.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSetBegin;
  for (const vector<vector<int>>::iterator vertexSumsIt = sumsIt + vertexDimension; sumsIt != vertexSumsIt; ++sumsIt)
    {
      scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / dimensionIt->size()), sumsIt->begin() + *firstCandidateElementIt, sumsIt->end());
      ++dimensionIt;
      ++firstCandidateElementIt;
    }
  // *dimensionIt contains vertices; the number of tuples a vertex covers depends on its absence/presence in the pattern: 2 * area / (dimensionIt->size() - 1) if absent; 2 * area / dimensionIt->size() if present
  const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
  const int shiftForPresent = static_cast<long long>(2 * area) * SparseCrispTube::getDefaultMembership() / (presentVertexEnd - presentVertexIt);
  const int shiftForAbsent = static_cast<long long>(2 * area) * SparseCrispTube::getDefaultMembership() / ((presentVertexEnd - presentVertexIt) - 1);
  const vector<int>::iterator sumBegin = sumsIt->begin();
  vector<int>::iterator sumIt = sumBegin + *firstCandidateElementIt;
  for (const vector<int>::iterator end = sumBegin + *presentVertexIt; sumIt != end; ++sumIt)
    {
      *sumIt *= unit;
      *sumIt += shiftForAbsent;
    }
  *sumIt *= unit;
  *sumIt += shiftForPresent;
  ++presentVertexIt;
  do
    {
      for (const vector<int>::iterator end = sumBegin + *presentVertexIt; ++sumIt != end; )
	{
	  *sumIt *= unit;
	  *sumIt += shiftForAbsent;
	}
      *sumIt *= unit;
      *sumIt += shiftForPresent;
    }
  while (++presentVertexIt != presentVertexEnd);
  for (const vector<int>::iterator end = sumsIt->end(); ++sumIt != end; )
    {
      *sumIt *= unit;
      *sumIt += shiftForAbsent;
    }
  for (const vector<vector<int>>::iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / (++dimensionIt)->size()), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
    }
}

void AbstractTrie::reset(const vector<unsigned int>::const_iterator firstCandidateElementBegin, vector<vector<int>>& sums)
{
  vector<vector<int>>::iterator sumsIt = sums.begin();
  fill(sumsIt->begin() + *firstCandidateElementBegin, sumsIt->end(), 0);
  ++sumsIt;
  const vector<vector<int>>::iterator sumsEnd = sums.end();
  vector<unsigned int>::const_iterator firstCandidateElementIt = firstCandidateElementBegin;
  do
    {
      fill(sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end(), 0);
    }
  while (++sumsIt != sumsEnd);
}

vector<int> AbstractTrie::backupAndResetSums(vector<int>::iterator sumIt, vector<int>::iterator sumEnd)
{
  vector<int> backup;
  backup.reserve(sumEnd - sumIt);
  do
    {
      backup.push_back(*sumIt);
      *sumIt = 0;
    }
  while (++sumIt != sumEnd);
  return backup;
}

vector<vector<int>> AbstractTrie::backupAndResetSumsAfterFirstCandidateElements(vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sums)
{
  // is01
  const vector<vector<int>>::iterator sumsEnd = sums.end();
  vector<vector<int>>::iterator sumsIt = sums.begin();
  vector<vector<int>> backup;
  backup.reserve(sumsEnd - sumsIt);
  do
    {
      backup.emplace_back(backupAndResetSums(sumsIt->begin() + *firstCandidateElementIt, sumsIt->end()));
      ++firstCandidateElementIt;
    }
  while (++sumsIt != sumsEnd);
  return backup;
}

vector<vector<int>> AbstractTrie::backupAndResetSumsAfterFirstCandidateElements(const unsigned int increasedDimensionId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>& sums)
{
  // is01
  vector<vector<int>> backup;
  backup.reserve(sums.size() - 1);
  vector<vector<int>>::iterator sumsIt = sums.begin();
  for (const vector<vector<int>>::iterator end = sums.begin() + increasedDimensionId; sumsIt != end; ++sumsIt)
    {
      backup.emplace_back(backupAndResetSums(sumsIt->begin() + *firstCandidateElementIt, sumsIt->end()));
      ++firstCandidateElementIt;
    }
  for (const vector<vector<int>>::iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      backup.emplace_back(backupAndResetSums(sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end()));
    }
  return backup;
}

void AbstractTrie::scaleShiftAndAddBackup(const int unit, const int shift, vector<int>::const_iterator backupIt, vector<int>::iterator sumIt, const vector<int>::const_iterator sumEnd)
{
  do
    {
      *sumIt *= unit;
      *sumIt += shift + *backupIt++;
    }
  while (++sumIt != sumEnd);
}

void AbstractTrie::scaleShiftAndAddBackupAfterFirstCandidateElements(const unsigned int increasedDimensionId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>::const_iterator backupIt, const int unit, const long long defaultNSetMembership, vector<vector<unsigned int>>::const_iterator dimensionIt, vector<vector<int>>& sums)
{
  // All dimensions but the one with an added element have been backed up
  vector<vector<int>>::iterator sumsIt = sums.begin();
  for (const vector<vector<int>>::iterator end = sums.begin() + increasedDimensionId; sumsIt != end; ++sumsIt)
    {
      scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), backupIt->begin(), sumsIt->begin() + *firstCandidateElementIt, sumsIt->end());
      ++firstCandidateElementIt;
      ++backupIt;
      ++dimensionIt;
    }
  for (const vector<vector<int>>::iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), backupIt->begin(), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
      ++backupIt;
    }
}

void AbstractTrie::scaleAndShift(const int unit, const int shift, vector<int>::iterator nbOfPresentTuplesIt, const vector<int>::iterator nbOfPresentTuplesEnd)
{
  do
    {
      *nbOfPresentTuplesIt *= unit;
      *nbOfPresentTuplesIt += shift;
    }
  while (++nbOfPresentTuplesIt != nbOfPresentTuplesEnd);
}

void AbstractTrie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums, const long long area, const int unit) const
{
  // is01
  const vector<vector<int>> backup = backupAndResetSumsAfterFirstCandidateElements(increasedDimensionId, firstCandidateElements.begin(), sums);
  addElement(increasedDimensionId, element, nSet, firstCandidateElements, sums);
  scaleShiftAndAddBackupAfterFirstCandidateElements(increasedDimensionId, firstCandidateElements.begin(), backup.begin(), unit, SparseCrispTube::getDefaultMembership() * area, nSet.begin(), sums);
}

void AbstractTrie::increaseSumsOnVerticesWithShift(const vector<int>& increases, const unsigned int firstCandidateVertex, const int unit, const long long area, vector<int>& sums, const vector<unsigned int>& presentVertices)
{
  // The number of edges a vertex covers depends on its absence/presence in the pattern: 2 * area / (presentVertices.size() - 1) if absent; 2 * area / presentVertices.size() if present
  const vector<unsigned int>::const_iterator presentVertexEnd = presentVertices.end();
  vector<unsigned int>::const_iterator presentVertexIt = presentVertices.begin();
  const int shiftForAbsent = 2 * SparseCrispTube::getDefaultMembership() * (area / ((presentVertexEnd - presentVertexIt) - 1));
  vector<int>::iterator sumIt = sums.begin() + firstCandidateVertex;
  const vector<int>::const_iterator increaseBegin = increases.begin();
  vector<int>::const_iterator increaseIt = increaseBegin + firstCandidateVertex;
  for (const vector<int>::const_iterator end = increaseBegin + *presentVertexIt; increaseIt != end; ++increaseIt)
    {
      *sumIt++ += *increaseIt * unit + shiftForAbsent;
    }
  const int shiftForPresent = 2 * SparseCrispTube::getDefaultMembership() * (area / (presentVertexEnd - presentVertexIt));
  *sumIt += *increaseIt * unit + shiftForPresent;
  ++presentVertexIt;
  do
    {
      for (const vector<int>::const_iterator end = increaseBegin + *presentVertexIt; ++increaseIt != end; )
	{
	  *++sumIt += *increaseIt * unit + shiftForAbsent;
	}
      *++sumIt += *increaseIt * unit + shiftForPresent;
    }
  while (++presentVertexIt != presentVertexEnd);
  for (const vector<int>::const_iterator increaseEnd = increases.end(); ++increaseIt != increaseEnd; )
    {
      *++sumIt += *increaseIt * unit + shiftForAbsent;
    }
}

void AbstractTrie::scaleShiftForVerticesAndAddBackup(const int unit, const long long defaultNSetMembership, const unsigned int firstCandidateElement, vector<int>::const_iterator backupIt, const vector<int>::iterator sumBegin, const vector<int>::const_iterator sumEnd, const vector<unsigned int>& presentVertices)
{
  // is01
  // The number of edges a vertex covers depends on its absence/presence in the pattern: 2 * area / (presentVertices.size() - 1) if absent; 2 * area / presentVertices.size() if present
  const vector<unsigned int>::const_iterator presentVertexEnd = presentVertices.end();
  vector<unsigned int>::const_iterator presentVertexIt = presentVertices.begin();
  const int shiftForAbsent = 2 * defaultNSetMembership / ((presentVertexEnd - presentVertexIt) - 1);
  vector<int>::iterator sumIt = sumBegin + firstCandidateElement;
  for (const vector<int>::iterator end = sumBegin + *presentVertexIt; sumIt != end; ++sumIt)
    {
      *sumIt *= unit;
      *sumIt += shiftForAbsent + *backupIt++;
    }
  *sumIt *= unit;
  const int shiftForPresent = 2 * defaultNSetMembership / (presentVertexEnd - presentVertexIt);
  *sumIt += shiftForPresent + *backupIt;
  ++presentVertexIt;
  do
    {
      for (const vector<int>::iterator end = sumBegin + *presentVertexIt; ++sumIt != end; )
	{
	  *sumIt *= unit;
	  *sumIt += shiftForAbsent + *++backupIt;
	}
      *sumIt *= unit;
      *sumIt += shiftForPresent + *++backupIt;
    }
  while (++presentVertexIt != presentVertexEnd);
  while (++sumIt != sumEnd)
    {
      *sumIt *= unit;
      *sumIt += shiftForAbsent + *++backupIt;
    }
}

void AbstractTrie::scaleShiftAndAddBackupAfterFirstCandidateElements(const unsigned int increasedDimensionId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>::const_iterator backupIt, const int unit, const unsigned int vertexDimensionId, const long long defaultNSetMembership, vector<vector<unsigned int>>::const_iterator dimensionIt, vector<vector<int>>& sums)
{
  // All dimensions but the one with an added non-vertex element have been backed up
  vector<vector<int>>::iterator sumsIt = sums.begin();
  if (increasedDimensionId < vertexDimensionId)
    {
      for (const vector<vector<int>>::iterator end = sums.begin() + increasedDimensionId; sumsIt != end; ++sumsIt)
	{
	  scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), backupIt->begin(), sumsIt->begin() + *firstCandidateElementIt, sumsIt->end());
	  ++firstCandidateElementIt;
	  ++backupIt;
	  ++dimensionIt;
	}
      for (const vector<vector<int>>::iterator end = sums.begin() + vertexDimensionId; ++sumsIt != end; )
	{
	  scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), backupIt->begin(), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
	  ++backupIt;
	}
      scaleShiftForVerticesAndAddBackup(unit, defaultNSetMembership, *++firstCandidateElementIt, backupIt->begin(), sumsIt->begin(), sumsIt->end(), *++dimensionIt);
    }
  else
    {
      for (const vector<vector<int>>::iterator end = sums.begin() + vertexDimensionId; sumsIt != end; ++sumsIt)
	{
	  scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), backupIt->begin(), sumsIt->begin() + *firstCandidateElementIt, sumsIt->end());
	  ++firstCandidateElementIt;
	  ++backupIt;
	  ++dimensionIt;
	}
      scaleShiftForVerticesAndAddBackup(unit, defaultNSetMembership, *firstCandidateElementIt, backupIt->begin(), sumsIt->begin(), sumsIt->end(), *dimensionIt);
      for (const vector<vector<int>>::iterator end = sums.begin() + increasedDimensionId; ++sumsIt != end; )
	{
	  scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), (++backupIt)->begin(), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
	}
      ++backupIt;
    }
  for (const vector<vector<int>>::iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), backupIt->begin(), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
      ++backupIt;
    }
}

void AbstractTrie::addNonVertexElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums, const long long area, const int unit, const unsigned int vertexDimensionId) const
{
  // is01
  const vector<vector<int>> backup = backupAndResetSumsAfterFirstCandidateElements(increasedDimensionId, firstCandidateElements.begin(), sums);
  addElement(increasedDimensionId, element, nSet, firstCandidateElements, sums);
  scaleShiftAndAddBackupAfterFirstCandidateElements(increasedDimensionId, firstCandidateElements.begin(), backup.begin(), unit, vertexDimensionId, SparseCrispTube::getDefaultMembership() * area, nSet.begin(), sums);
}

void AbstractTrie::scaleShiftAndAddBackupAfterFirstCandidateElements(const unsigned int vertexDimensionId, const unsigned int addedVertexId, vector<unsigned int>::const_iterator firstCandidateElementIt, vector<vector<int>>::const_iterator backupIt, const int unit, const long long defaultNSetMembership, vector<vector<unsigned int>>::const_iterator dimensionIt, vector<vector<int>>& sums)
{
  // All dimensions have been backed up before adding a vertex
  vector<vector<int>>::iterator sumsIt = sums.begin();
  for (const vector<vector<int>>::iterator end = sums.begin() + vertexDimensionId; sumsIt != end; ++sumsIt)
    {
      scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), backupIt->begin(), sumsIt->begin() + *firstCandidateElementIt, sumsIt->end());
      ++firstCandidateElementIt;
      ++backupIt;
      ++dimensionIt;
    }
  {
    // Every vertex but the added one additionally covers defaultNSetMembership / (static_cast<int>(dimensionIt->size()) - 1) tuples
    vector<int>::iterator sumIt = sumsIt->begin() + *firstCandidateElementIt;
    const int shiftForVertex = defaultNSetMembership / (static_cast<int>(dimensionIt->size()) - 1);
    vector<int>::const_iterator backupedSumIt = backupIt->begin();
    for (const vector<int>::iterator addedVertexSumIt = sumsIt->begin() + addedVertexId; sumIt != addedVertexSumIt; ++sumIt)
      {
	*sumIt *= unit;
	*sumIt += shiftForVertex + *backupedSumIt++;
      }
    *sumIt = *backupedSumIt;
    for (const vector<int>::iterator sumEnd = sumsIt->end(); ++sumIt != sumEnd; )
      {
	*sumIt *= unit;
	*sumIt += shiftForVertex + *++backupedSumIt;
      }
  }
  for (const vector<vector<int>>::iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      scaleShiftAndAddBackup(unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), (++backupIt)->begin(), sumsIt->begin() + *++firstCandidateElementIt, sumsIt->end());
    }
}

void AbstractTrie::addVertex(const unsigned int increasedDimensionId, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, const vector<unsigned int>& firstCandidateElements, vector<vector<int>>& sums, const long long area, const int unit) const
{
  // is01
  const vector<vector<int>> backup = backupAndResetSumsAfterFirstCandidateElements(firstCandidateElements.begin(), sums);
  addVertex(vertexIt, nSet, firstCandidateElements, sums);
  scaleShiftAndAddBackupAfterFirstCandidateElements(increasedDimensionId, *vertexIt, firstCandidateElements.begin(), backup.begin(), unit, SparseCrispTube::getDefaultMembership() * area, nSet.begin(), sums);
}

void AbstractTrie::sumsOnNewCandidateVertices(const vector<vector<unsigned int>>& nSet, const unsigned int newFirstCandidateVertex, const unsigned int oldFirstCandidateVertex, vector<int>& sumsOnHyperplanes) const
{
  // !is01
  fill_n(sumsOnHyperplanes.begin() + newFirstCandidateVertex, oldFirstCandidateVertex - newFirstCandidateVertex, 0);
  increaseSumsOnNewCandidateVertices(nSet.begin(), nSet.front().end(), newFirstCandidateVertex, oldFirstCandidateVertex, sumsOnHyperplanes);
}

void AbstractTrie::sumsOnNewCandidateHyperplanes(const vector<vector<unsigned int>>& nSet, const vector<vector<unsigned int>>::iterator dimensionWithCandidatesIt, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes, const unsigned long long area, const int unit) const
{
  // is01
  sumsOnNewCandidateHyperplanes(nSet, dimensionWithCandidatesIt, newFirstCandidateElement, oldFirstCandidateElement, sumsOnHyperplanes);
  scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / dimensionWithCandidatesIt->size()), sumsOnHyperplanes.begin() + newFirstCandidateElement, sumsOnHyperplanes.begin() + oldFirstCandidateElement);
}

void AbstractTrie::sumsOnNewCandidateVertices(const vector<vector<unsigned int>>& nSet, const unsigned int newFirstCandidateElement, const unsigned int oldFirstCandidateElement, vector<int>& sumsOnHyperplanes, const unsigned long long areaOfACandidate, const int unit) const
{
  // is01
  sumsOnNewCandidateVertices(nSet, newFirstCandidateElement, oldFirstCandidateElement, sumsOnHyperplanes);
  scaleAndShift(unit, areaOfACandidate * SparseCrispTube::getDefaultMembership(), sumsOnHyperplanes.begin() + newFirstCandidateElement, sumsOnHyperplanes.begin() + oldFirstCandidateElement);
}

#else

void AbstractTrie::addElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums, const long long area, const int unit) const
{
  // is01
  vector<vector<int>> increases = increasesForAddingElement(increasedDimensionId, sums);
  addElement(increasedDimensionId, element, nSet, increases);
  vector<vector<int>>::iterator sumsIt = sums.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  const long long defaultNSetMembership = SparseCrispTube::getDefaultMembership() * area;
  vector<vector<int>>::const_iterator increasesIt = increases.begin();
  for (; !increasesIt->empty(); ++increasesIt)
    {
      increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), *sumsIt);
      ++sumsIt;
      ++dimensionIt;
    }
  for (const vector<vector<int>>::const_iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      increaseWithShift(*++increasesIt, unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), *sumsIt);
    }
}

void AbstractTrie::addNonVertexElement(const unsigned int increasedDimensionId, const unsigned int element, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums, const long long area, const int unit, const unsigned int vertexDimensionId) const
{
  // is01
  vector<vector<int>> increases = increasesForAddingElement(increasedDimensionId, sums);
  addElement(increasedDimensionId, element, nSet, increases);
  vector<vector<int>>::iterator sumsIt = sums.begin();
  const long long defaultNSetMembership = SparseCrispTube::getDefaultMembership() * area;
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  vector<vector<int>>::const_iterator increasesIt = increases.begin();
  if (increasedDimensionId < vertexDimensionId)
    {
      for (; !increasesIt->empty(); ++increasesIt)
	{
	  increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), *sumsIt);
	  ++sumsIt;
	  ++dimensionIt;
	}
      for (const vector<vector<int>>::const_iterator vertexIncreasesIt = increases.begin() + vertexDimensionId; ++increasesIt != vertexIncreasesIt; )
	{
	  increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), *++sumsIt);
	}
      increaseSumsOnVerticesWithShift(*increasesIt, unit, area, *++sumsIt, *++dimensionIt);
    }
  else
    {
      for (const vector<vector<int>>::const_iterator vertexIncreasesIt = increasesIt + vertexDimensionId; increasesIt != vertexIncreasesIt; ++increasesIt)
	{
	  increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), *sumsIt);
	  ++sumsIt;
	  ++dimensionIt;
	}
      increaseSumsOnVerticesWithShift(*increasesIt, unit, area, *sumsIt, *dimensionIt);
      for (++increasesIt; !increasesIt->empty(); ++increasesIt)
	{
	  increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), *++sumsIt);
	}
    }
  for (const vector<vector<int>>::const_iterator increasesEnd = increases.end(); ++increasesIt != increasesEnd; )
    {
      increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), *++sumsIt);
    }
}

void AbstractTrie::addVertex(const unsigned int increasedDimensionId, const vector<unsigned int>::const_iterator vertexIt, const vector<vector<unsigned int>>& nSet, vector<vector<int>>& sums, const long long area, const int unit) const
{
  // is01
  vector<vector<int>> increases;
  increases.reserve(sums.size());
  vector<vector<int>>::iterator sumsIt = sums.begin();
  increases.emplace_back(sumsIt->size());
  const vector<vector<int>>::const_iterator sumsEnd = sums.end();
  while (++sumsIt != sumsEnd)
    {
      increases.emplace_back(sumsIt->size());
    }
  sumsIt = sums.begin();
  addVertex(vertexIt, nSet, increases);
  const long long defaultNSetMembership = SparseCrispTube::getDefaultMembership() * area;
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  vector<vector<int>>::const_iterator increasesIt = increases.begin();
  // Dimensions before that of the vertices
  for (const vector<vector<int>>::iterator vertexSumsIt = sumsIt + increasedDimensionId; sumsIt != vertexSumsIt; ++sumsIt)
    {
      increaseWithShift(*increasesIt, unit, defaultNSetMembership / static_cast<int>(dimensionIt->size()), *sumsIt);
      ++dimensionIt;
      ++increasesIt;
    }
  {
    // *dimensionIt contains the vertices; every vertex but the added one additionally covers defaultNSetMembership / (static_cast<int>(dimensionIt->size()) - 1) tuples
    vector<int>::iterator sumIt = sumsIt->begin();
    const int shiftForVertex = defaultNSetMembership / (static_cast<int>(dimensionIt->size()) - 1);
    vector<int>::const_iterator increaseIt = increasesIt->begin();
    for (const vector<int>::const_iterator end = increaseIt + *vertexIt; increaseIt != end; ++increaseIt)
      {
	*sumIt++ += *increaseIt * unit + shiftForVertex;
      }
    for (const vector<int>::const_iterator end = increasesIt->end(); ++increaseIt != end; )
      {
	*++sumIt += *increaseIt * unit + shiftForVertex;
      }
  }
  // Dimensions after that of the vertices
  while (++sumsIt != sumsEnd)
    {
      increaseWithShift(*++increasesIt, unit, defaultNSetMembership / static_cast<int>((++dimensionIt)->size()), *sumsIt);
    }
}

void AbstractTrie::increaseWithShift(const vector<int>& increases, const int unit, const int shift, vector<int>& sums)
{
  const vector<int>::const_iterator increaseEnd = increases.end();
  vector<int>::const_iterator increaseIt = increases.begin();
  vector<int>::iterator sumIt = sums.begin();
  do
    {
      *sumIt++ += *increaseIt * unit + shift;
    }
  while (++increaseIt != increaseEnd);
}

vector<vector<int>> AbstractTrie::increasesForAddingElement(const unsigned int increasedDimensionId, const vector<vector<int>>& sums)
{
  // is01
  vector<vector<int>> increases;
  increases.reserve(sums.size());
  // With the following code, the vectors in increases are of the sizes of the vectors in sums, despite firstCandidateElements; avoiding the leading 0s, that addElement will never access, would require rewriting essentially all the data class functions, which may end up slower, because accessing the proper position in increases would require subtracting the corresponding first candidate element to the index
  vector<vector<int>>::const_iterator sumsIt = sums.begin();
  for (const vector<vector<int>>::const_iterator end = sums.begin() + increasedDimensionId; sumsIt != end; ++sumsIt)
    {
      increases.emplace_back(sumsIt->size());
    }
  increases.emplace_back();
  for (const vector<vector<int>>::const_iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      increases.emplace_back(sumsIt->size());
    }
  return increases;
}

void AbstractTrie::increaseSumsOnVerticesWithShift(const vector<int>& increases, const int unit, const long long area, vector<int>& sums, const vector<unsigned int>& presentVertices)
{
  // The number of edges a vertex covers depends on its absence/presence in the pattern: 2 * area / (presentVertices.size() - 1) if absent; 2 * area / presentVertices.size() if present
  const vector<unsigned int>::const_iterator presentVertexEnd = presentVertices.end();
  vector<unsigned int>::const_iterator presentVertexIt = presentVertices.begin();
  const int shiftForPresent = static_cast<long long>(2 * area) * SparseCrispTube::getDefaultMembership() / (presentVertexEnd - presentVertexIt);
  const int shiftForAbsent = static_cast<long long>(2 * area) * SparseCrispTube::getDefaultMembership() / ((presentVertexEnd - presentVertexIt) - 1);
  vector<int>::iterator sumIt = sums.begin();
  vector<int>::const_iterator increaseIt = increases.begin();
  const vector<int>::const_iterator increaseBegin = increaseIt;
  for (const vector<int>::const_iterator end = increaseBegin + *presentVertexIt; increaseIt != end; ++increaseIt)
    {
      *sumIt++ += *increaseIt * unit + shiftForAbsent;
    }
  *sumIt += *increaseIt * unit + shiftForPresent;
  ++presentVertexIt;
  do
    {
      for (const vector<int>::const_iterator end = increaseBegin + *presentVertexIt; ++increaseIt != end; )
	{
	  *++sumIt += *increaseIt * unit + shiftForAbsent;
	}
      *++sumIt += *increaseIt * unit + shiftForPresent;
    }
  while (++presentVertexIt != presentVertexEnd);
  for (const vector<int>::const_iterator increaseEnd = increases.end(); ++increaseIt != increaseEnd; )
    {
      *++sumIt += *increaseIt * unit + shiftForAbsent;
    }
}
#endif

#if defined ASSERT || !defined PRUNE
void AbstractTrie::reset(vector<vector<int>>& sums)
{
  const vector<vector<int>>::iterator sumsEnd = sums.end();
  vector<vector<int>>::iterator sumsIt = sums.begin();
  fill(sumsIt->begin(), sumsIt->end(), 0);
  ++sumsIt;
  do
    {
      fill(sumsIt->begin(), sumsIt->end(), 0);
    }
  while (++sumsIt != sumsEnd);
}

long long AbstractTrie::sumsOnPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit) const
{
  // is01
  const long long sumOnPattern = sumsOnPatternAndHyperplanes(nSetBegin, sums) * unit + SparseCrispTube::getDefaultMembership() * static_cast<long long>(area);
  const vector<vector<int>>::iterator sumsEnd = sums.end();
  vector<vector<int>>::iterator sumsIt = sums.begin();
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSetBegin;
  do
    {
      scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / dimensionIt->size()), *sumsIt);
      ++dimensionIt;
    }
  while (++sumsIt != sumsEnd);
  return sumOnPattern;
}

long long AbstractTrie::sumsOnGraphPatternAndHyperplanes(const vector<vector<unsigned int>>::const_iterator nSetBegin, vector<vector<int>>& sums, const unsigned long long area, const int unit, const unsigned int vertexDimension) const
{
  // is01
  const long long sumOnPattern = sumsOnPatternAndHyperplanes(nSetBegin, sums) * unit + SparseCrispTube::getDefaultMembership() * static_cast<long long>(area);
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSetBegin;
  vector<vector<int>>::iterator sumsIt = sums.begin();
  for (const vector<vector<int>>::iterator vertexSumsIt = sumsIt + vertexDimension; sumsIt != vertexSumsIt; ++sumsIt)
    {
      scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / dimensionIt->size()), *sumsIt);
      ++dimensionIt;
    }
  // *dimensionIt contains vertices; the number of tuples a vertex covers depends on its absence/presence in the pattern: 2 * area / (dimensionIt->size() - 1) if absent; 2 * area / dimensionIt->size() if present
  const vector<unsigned int>::const_iterator presentVertexEnd = dimensionIt->end();
  vector<unsigned int>::const_iterator presentVertexIt = dimensionIt->begin();
  const int shiftForPresent = static_cast<long long>(2 * area) * SparseCrispTube::getDefaultMembership() / (presentVertexEnd - presentVertexIt);
  const int shiftForAbsent = static_cast<long long>(2 * area) * SparseCrispTube::getDefaultMembership() / ((presentVertexEnd - presentVertexIt) - 1);
  vector<int>::iterator sumIt = sumsIt->begin();
  const vector<int>::iterator sumBegin = sumIt;
  for (const vector<int>::iterator end = sumBegin + *presentVertexIt; sumIt != end; ++sumIt)
    {
      *sumIt *= unit;
      *sumIt += shiftForAbsent;
    }
  *sumIt *= unit;
  *sumIt += shiftForPresent;
  ++presentVertexIt;
  do
    {
      for (const vector<int>::iterator end = sumBegin + *presentVertexIt; ++sumIt != end; )
	{
	  *sumIt *= unit;
	  *sumIt += shiftForAbsent;
	}
      *sumIt *= unit;
      *sumIt += shiftForPresent;
    }
  while (++presentVertexIt != presentVertexEnd);
  for (const vector<int>::iterator end = sumsIt->end(); ++sumIt != end; )
    {
      *sumIt *= unit;
      *sumIt += shiftForAbsent;
    }
  for (const vector<vector<int>>::iterator sumsEnd = sums.end(); ++sumsIt != sumsEnd; )
    {
      scaleAndShift(unit, SparseCrispTube::getDefaultMembership() * static_cast<long long>(area / (++dimensionIt)->size()), *sumsIt);
    }
  return sumOnPattern;
}

void AbstractTrie::scaleAndShift(const int unit, const int shift, vector<int>& nbOfPresentTuples)
{
  const vector<int>::iterator nbOfPresentTuplesEnd = nbOfPresentTuples.end();
  vector<int>::iterator nbOfPresentTuplesIt = nbOfPresentTuples.begin();
  do
    {
      *nbOfPresentTuplesIt *= unit;
      *nbOfPresentTuplesIt += shift;
    }
  while (++nbOfPresentTuplesIt != nbOfPresentTuplesEnd);
}
#endif
