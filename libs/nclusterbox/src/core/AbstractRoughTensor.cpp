// Copyright 2018-2026 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include <cmath>
#include <iostream>

#include "DenseRoughTensor.h"

#include "../utilities/NoOutputException.h"
#include "TupleFileReader.h"
#include "FuzzyTupleFileReader.h"
#include "SparseRoughTensor.h"
#include "ConcurrentPatternPool.h"
#include "LastTrie.h"
#include "SparseCrispTube.h"
#include "SparseFuzzyTube.h"

vector<vector<string>> AbstractRoughTensor::ids2Labels;
double AbstractRoughTensor::nullModelRSS;
vector<unsigned int> AbstractRoughTensor::cardinalities;
int AbstractRoughTensor::unit;
bool AbstractRoughTensor::isNoSelection;
vector<unsigned int> AbstractRoughTensor::external2InternalDimensionOrder;
vector<vector<vector<unsigned int>>> AbstractRoughTensor::candidateVariables;
bool AbstractRoughTensor::isPrintLambda;

string AbstractRoughTensor::sizePrefix;
string AbstractRoughTensor::sizeSeparator;
string AbstractRoughTensor::areaPrefix;
bool AbstractRoughTensor::isSizePrinted;
bool AbstractRoughTensor::isAreaPrinted;

ostream AbstractRoughTensor::outputStream(nullptr);
ofstream AbstractRoughTensor::outputFile;
string AbstractRoughTensor::outputDimensionSeparator;
string AbstractRoughTensor::outputElementSeparator;
string AbstractRoughTensor::rssPrefix;

#if defined TIME || defined DETAILED_TIME
steady_clock::time_point AbstractRoughTensor::overallBeginning;
#endif
#ifdef DETAILED_TIME
steady_clock::time_point AbstractRoughTensor::shiftingBeginning;
#endif

AbstractRoughTensor::~AbstractRoughTensor()
{
}

double AbstractRoughTensor::membershipSum(const vector<FuzzyTuple>& fuzzyTuples)
{
  if (LastTrie::is01)
    {
      return fuzzyTuples.size();
    }
  vector<FuzzyTuple>::const_iterator fuzzyTupleIt = fuzzyTuples.begin();
  double sum = fuzzyTupleIt->getMembership();
  for (const vector<FuzzyTuple>::const_iterator fuzzyTupleEnd = fuzzyTuples.end(); ++fuzzyTupleIt != fuzzyTupleEnd; )
    {
      sum += fuzzyTupleIt->getMembership();
    }
  return sum;
}

void AbstractRoughTensor::printDimension(const vector<unsigned int>& dimension, const vector<string>& ids2LabelsInDimension, ostream& out)
{
  vector<unsigned int>::const_iterator elementIt = dimension.begin();
  out << ids2LabelsInDimension[*elementIt];
  for (const vector<unsigned int>::const_iterator end = dimension.end(); ++elementIt != end; )
    {
      out << outputElementSeparator << ids2LabelsInDimension[*elementIt];
    }
  out << outputDimensionSeparator;
}

void AbstractRoughTensor::printPatternDensityAndSizes(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const
{
  if (isPrintLambda)
    {
      out << density / unit;
    }
  else
    {
      out << density / unit + getAverageShift(nSet);
    }
  if (isSizePrinted)
    {
      vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
      out << sizePrefix << nSet[*internalDimensionIdIt].size();
      ++internalDimensionIdIt;
      const vector<unsigned int>::const_iterator internalDimensionIdEnd = external2InternalDimensionOrder.end();
      do
	{
	  out << sizeSeparator << nSet[*internalDimensionIdIt].size();
	}
      while (++internalDimensionIdIt != internalDimensionIdEnd);
    }
}

unsigned int AbstractRoughTensor::nbOfElements() const
{
  vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
  unsigned int nbOfElements = *cardinalityIt;
  for (const vector<unsigned int>::const_iterator cardinalityEnd = cardinalities.end(); ++cardinalityIt != cardinalityEnd; )
    {
      nbOfElements += *cardinalityIt;
    }
  return nbOfElements;
}

unsigned long long AbstractRoughTensor::patternArea(const vector<vector<unsigned int>>& nSet)
{
  vector<vector<unsigned int>>::const_iterator dimensionIt = nSet.begin();
  unsigned long long area = dimensionIt->size();
  ++dimensionIt;
  const vector<vector<unsigned int>>::const_iterator dimensionEnd = nSet.end();
  do
    {
      area *= dimensionIt->size();
    }
  while (++dimensionIt != dimensionEnd);
  return area;
}

void AbstractRoughTensor::printPattern(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const
{
  {
    // Actual pattern printing
    vector<vector<string>>::const_iterator ids2LabelsIt = ids2Labels.begin();
    vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
    printDimension(nSet[*internalDimensionIdIt], *ids2LabelsIt, out);
    ++ids2LabelsIt;
    ++internalDimensionIdIt;
    const vector<unsigned int>::const_iterator internalDimensionIdEnd = external2InternalDimensionOrder.end();
    do
      {
	printDimension(nSet[*internalDimensionIdIt], *ids2LabelsIt, out);
	++ids2LabelsIt;
      }
    while (++internalDimensionIdIt != internalDimensionIdEnd);
  }
  printPatternDensityAndSizes(nSet, density, out);
  if (isAreaPrinted)
    {
      out << areaPrefix << patternArea(nSet);
    }
}

void AbstractRoughTensor::output(const vector<vector<unsigned int>>& nSet, const float density) const
{
  printPattern(nSet, density, outputStream);
  outputStream << '\n';
}

void AbstractRoughTensor::output(const vector<vector<unsigned int>>& nSet, const float density, const double rss) const
{
  printPattern(nSet, density, outputStream);
  outputStream << rssPrefix << rss / unit / unit << '\n';
}

AbstractRoughTensor* AbstractRoughTensor::makeRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double densityThreshold, const double shift)
{
  if (LastTrie::is01)
    {
      if ((8 * sizeof(double) + 1) * getAreaFromIds2Labels() < 8 * (sizeof(FuzzyTuple) + sizeof(unsigned int) * (ids2Labels.size() + 1)) * fuzzyTuples.size())
      	{
	  // Dense storage (including the rough tensor) takes less space, assuming the sparse storage would only use sparse tubes
	  return new DenseRoughTensor(fuzzyTuples, shift);
      	}
      setMetadata(fuzzyTuples, shift);
      SparseCrispTube::setDefaultMembershipAndDensityLimit(unit * -shift, densityThreshold / sizeof(unsigned int) / 8);
      return new SparseRoughTensor(fuzzyTuples, shift);
    }
  if ((sizeof(double) + sizeof(int)) * getAreaFromIds2Labels() < (sizeof(FuzzyTuple) + sizeof(unsigned int) * ids2Labels.size() + sizeof(pair<unsigned int, int>)) * fuzzyTuples.size())
    {
      // Dense storage (including the rough tensor) takes less space, assuming the sparse storage would only use sparse tubes
      return new DenseRoughTensor(fuzzyTuples, shift);
    }
  setMetadata(fuzzyTuples, shift);
  SparseFuzzyTube::setDefaultMembershipAndDensityLimit(unit * -shift, densityThreshold * sizeof(int) / sizeof(pair<unsigned int, int>));
  return new SparseRoughTensor(fuzzyTuples, shift);
}

AbstractRoughTensor* AbstractRoughTensor::makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const bool isInput01, const bool isVerbose)
{
  vector<FuzzyTuple> fuzzyTuples = getFuzzyTuples(tensorFileName, inputDimensionSeparator, inputElementSeparator, isInput01, isVerbose);
  return makeRoughTensor(fuzzyTuples, densityThreshold, membershipSum(fuzzyTuples) / getAreaFromIds2Labels());
}

AbstractRoughTensor* AbstractRoughTensor::makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const double shift, const bool isInput01, const bool isVerbose)
{
  vector<FuzzyTuple> fuzzyTuples = getFuzzyTuples(tensorFileName, inputDimensionSeparator, inputElementSeparator, isInput01, isVerbose);
  return makeRoughTensor(fuzzyTuples, densityThreshold, shift);
}

void AbstractRoughTensor::setOutput(const char* outputFileName, const char* outputDimensionSeparatorParam, const char* outputElementSeparatorParam, const char* sizePrefixParam, const char* sizeSeparatorParam, const char* areaPrefixParam, const char* rssPrefixParam, const bool isPrintLambdaParam, const bool isSizePrintedParam, const bool isAreaPrintedParam)
{
#ifdef GNUPLOT
#ifdef DETAILED_TIME
#ifdef NUMERIC_PRECISION
  cout << '\t' << 1. / unit << '\t' << duration_cast<duration<double>>(steady_clock::now() - shiftingBeginning).count();
#else
  cout << '\t' << duration_cast<duration<double>>(steady_clock::now() - shiftingBeginning).count();
#endif
#else
#ifdef NUMERIC_PRECISION
  cout << 1. / unit << '\n';
#endif
#endif
#else
#ifdef NUMERIC_PRECISION
  cout << "Numeric precision: " << 1. / unit << '\n';
#endif
#ifdef DETAILED_TIME
  cout << "Tensor shifting time: " << duration_cast<duration<double>>(steady_clock::now() - shiftingBeginning).count() << "s\n";
#endif
#endif
  outputDimensionSeparator = outputDimensionSeparatorParam;
  outputElementSeparator = outputElementSeparatorParam;
  sizePrefix = sizePrefixParam;
  sizeSeparator = sizeSeparatorParam;
  areaPrefix = areaPrefixParam;
  rssPrefix = rssPrefixParam;
  isPrintLambda = isPrintLambdaParam;
  isSizePrinted = isSizePrintedParam;
  isAreaPrinted = isAreaPrintedParam;
  if (string(outputFileName) == "-")
    {
      outputStream.rdbuf(cout.rdbuf());
      return;
    }
  outputFile.open(outputFileName);
  if (!outputFile)
    {
      throw NoOutputException(outputFileName);
    }
  outputStream.rdbuf(outputFile.rdbuf());
}

int AbstractRoughTensor::getUnit()
{
  return unit;
}

bool AbstractRoughTensor::isDirectOutput()
{
  return isNoSelection;
}

const vector<vector<string>>& AbstractRoughTensor::getIds2Labels()
{
  return ids2Labels;
}

const vector<unsigned int>& AbstractRoughTensor::getExternal2InternalDimensionOrder()
{
  return external2InternalDimensionOrder;
}

unsigned long long AbstractRoughTensor::getArea()
{
  const vector<unsigned int>::const_iterator cardinalityEnd = cardinalities.end();
  vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
  unsigned long long area = *cardinalityIt++;
  do
    {
      area *= *cardinalityIt;
    }
  while (++cardinalityIt != cardinalityEnd);
  return area;
}

double AbstractRoughTensor::unitDenominatorGivenNullModelRSS()
{
  if (nullModelRSS > 1)
    {
      // sqrt(nullModelRSS) is a possible unit denominator (sqrt because the RSS is stored in a long long)
      return sqrt(nullModelRSS);
    }
  return 1;
}

void AbstractRoughTensor::setUnitForProjectedTensor(const double rss, const double maxMembership)
{
  if (rss > 1)
    {
      unit = numeric_limits<int>::max() / max(sqrt(rss), maxMembership);
      return;
    }
  unit = static_cast<double>(numeric_limits<int>::max()) / max(1., maxMembership);
}

vector<FuzzyTuple> AbstractRoughTensor::getFuzzyTuples(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const bool isVerbose)
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
      TupleFileReader tupleFileReader(tensorFileName, inputDimensionSeparator, inputElementSeparator);
      vector<FuzzyTuple> tuples = tupleFileReader.read();
      ids2Labels = std::move(tupleFileReader.getIds2Labels());
      if (isVerbose)
	{
	  cout << "\rParsing Boolean tensor: " << tuples.size() << '/' << getAreaFromIds2Labels() << " tuples with nonzero membership degrees.\n" << flush;
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
      return tuples;
    }
  if (isVerbose)
    {
      cout << "Parsing fuzzy tensor ... " << flush;
    }
  FuzzyTupleFileReader fuzzyTupleFileReader(tensorFileName, inputDimensionSeparator, inputElementSeparator);
  pair<vector<FuzzyTuple>, bool> fuzzyTuplesAndIs01 = fuzzyTupleFileReader.read();
  ids2Labels = std::move(fuzzyTupleFileReader.getIds2Labels());
  LastTrie::is01 = fuzzyTuplesAndIs01.second;
  if (isVerbose)
    {
      cout << "\rParsing fuzzy tensor: " << fuzzyTuplesAndIs01.first.size() << '/' << getAreaFromIds2Labels() << " tuples with nonzero membership degrees.\n" << flush;
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
  return fuzzyTuplesAndIs01.first;
}

unsigned long long AbstractRoughTensor::getAreaFromIds2Labels()
{
  vector<vector<string>>::const_iterator ids2LabelsInDimensionIt = ids2Labels.begin();
  unsigned long long area = ids2LabelsInDimensionIt->size();
  ++ids2LabelsInDimensionIt;
  const vector<vector<string>>::const_iterator ids2LabelsInDimensionEnd = ids2Labels.end();
  do
    {
      area *= ids2LabelsInDimensionIt->size();
    }
  while (++ids2LabelsInDimensionIt != ids2LabelsInDimensionEnd);
  return area;
}

void AbstractRoughTensor::orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities()
{
  vector<vector<string>>::const_iterator ids2LabelsInDimensionIt = ids2Labels.begin();
  const unsigned int n = ids2Labels.end() - ids2LabelsInDimensionIt;
  vector<pair<unsigned int, unsigned int>> dimensions;
  dimensions.reserve(n);
  dimensions.emplace_back(ids2LabelsInDimensionIt->size(), 0);
  unsigned int dimensionId = 1;
  do
    {
      dimensions.emplace_back((++ids2LabelsInDimensionIt)->size(), dimensionId);
    }
  while (++dimensionId != n);
  // Sort dimensions by increasing cardinality
  sort(dimensions.begin(), dimensions.end(), [](const pair<unsigned int, unsigned int>& dimension1, const pair<unsigned int, unsigned int>& dimension2) {return dimension1.first < dimension2.first;});
  cardinalities.reserve(n);
  external2InternalDimensionOrder.resize(n);
  vector<pair<unsigned int, unsigned int>>::const_iterator dimensionIt = dimensions.begin();
  external2InternalDimensionOrder[dimensionIt->second] = 0;
  cardinalities.push_back(dimensionIt->first);
  dimensionId = 1;
  do
    {
      external2InternalDimensionOrder[(++dimensionIt)->second] = dimensionId;
      cardinalities.push_back(dimensionIt->first);
    }
  while (++dimensionId != n);
}

void AbstractRoughTensor::setMetadataForDimension(const unsigned int dimensionId, const unsigned long long area, const double shift, double& unitDenominator, vector<string>& ids2LabelsInDimension, vector<FuzzyTuple>& fuzzyTuples)
{
  // Sparse tensor
  const unsigned int nbOfElements = ids2LabelsInDimension.size();
  // Computing positive and, for fuzzy tensors, negative memberships of the elements
  if (LastTrie::is01)
    {
      // shifts not subtracted because Trie::sumsOnPatternAndHyperplanes multiplies by unit (the product cannot overflow) before subtracting the shifts
      vector<pair<unsigned int, unsigned int>> elementPositiveMemberships = pairs0AndId<unsigned int>(nbOfElements);
      const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
      vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
      do
	{
	  ++elementPositiveMemberships[fuzzyTupleIt->getElementId(dimensionId)].first;
	}
      while (++fuzzyTupleIt != fuzzyTupleEnd);
      FuzzyTuple::remapElements(dimensionId, sortByPositiveMemberships<unsigned int>(elementPositiveMemberships, ids2LabelsInDimension), fuzzyTuples);
      if (elementPositiveMemberships.back().first > unitDenominator)
	{
	  unitDenominator = elementPositiveMemberships.back().first;
	}
      return;
    }
  // !is01
  vector<pair<double, unsigned int>> elementPositiveMemberships = pairs0AndId<double>(nbOfElements);
  {
    vector<double> elementNegativeMemberships(nbOfElements, shift * (area / nbOfElements)); // assumes every membership null and correct that in the loop below
    {
      const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
      vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
      do
	{
	  const unsigned int elementId = fuzzyTupleIt->getElementId(dimensionId);
	  const double membership = fuzzyTupleIt->getMembership();
	  if (membership > 0)
	    {
	      elementPositiveMemberships[elementId].first += membership;
	      elementNegativeMemberships[elementId] -= shift;
	    }
	  else
	    {
	      elementNegativeMemberships[elementId] -= membership + shift;
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
  FuzzyTuple::remapElements(dimensionId, sortByPositiveMemberships<double>(elementPositiveMemberships, ids2LabelsInDimension), fuzzyTuples);
  if (elementPositiveMemberships.back().first > unitDenominator)
    {
      unitDenominator = elementPositiveMemberships.back().first;
    }
}

template<typename T> vector<pair<T, unsigned int>> AbstractRoughTensor::pairs0AndId(const unsigned int size)
{
  vector<pair<T, unsigned int>> pairs;
  pairs.reserve(size);
  unsigned int id = 0;
  do
    {
      pairs.emplace_back(0, id);
    }
  while (++id != size);
  return pairs;
}

template<typename T> vector<unsigned int> AbstractRoughTensor::sortByPositiveMemberships(vector<pair<T, unsigned int>>& elementPositiveMemberships, vector<string>& ids2LabelsInDimension)
{
  // Sparse tensor
  sort(elementPositiveMemberships.begin(), elementPositiveMemberships.end(), [](const pair<T, unsigned int>& elementPositiveMembership1, const pair<T, unsigned int>& elementPositiveMembership2) {return elementPositiveMembership1.first < elementPositiveMembership2.first;});
  // Computing the new ids, in increasing order of the positive membership (for faster lower_bound in SparseFuzzyTube::sumOnSlice and to choose the element with the greatest membership in case of equality) and reorder ids2LabelsInDimension accordingly
  const unsigned int nbOfElements = elementPositiveMemberships.size();
  vector<unsigned int> mapping(nbOfElements);
  vector<string> newIds2LabelsInDimension;
  newIds2LabelsInDimension.reserve(nbOfElements);
  {
    unsigned int newId = 0;
    typename vector<pair<T, unsigned int>>::const_iterator elementPositiveMembershipIt = elementPositiveMemberships.begin();
    do
      {
	newIds2LabelsInDimension.emplace_back(std::move(ids2LabelsInDimension[elementPositiveMembershipIt->second]));
	mapping[elementPositiveMembershipIt->second] = newId;
	++elementPositiveMembershipIt;
      }
    while (++newId != nbOfElements);
  }
  ids2LabelsInDimension = std::move(newIds2LabelsInDimension);
  return mapping;
}

void AbstractRoughTensor::shiftTuplesAndSetNullModelRSS(vector<FuzzyTuple>& fuzzyTuples, const double shift, const unsigned long long area)
{
  // Sparse tensor
  const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  if (LastTrie::is01)
    {
      nullModelRSS = (shift * area - 2 * (fuzzyTupleEnd - fuzzyTupleIt)) * shift + (fuzzyTupleEnd - fuzzyTupleIt);
      // Shift the membership of every tuple
      do
	{
	  fuzzyTupleIt->shiftMembership(shift);
	}
      while (++fuzzyTupleIt != fuzzyTupleEnd);
      return;
    }
  nullModelRSS = shift * shift * (area - (fuzzyTupleEnd - fuzzyTupleIt));
  // Shift the membership of every tuple
  do
    {
      fuzzyTupleIt->shiftMembership(shift);
      nullModelRSS += fuzzyTupleIt->getMembershipSquared();
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
}

void AbstractRoughTensor::setMetadata(vector<FuzzyTuple>& fuzzyTuples, const double shift)
{
  // Sparse tensor
  orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities();
  const unsigned long long area = getArea();
  shiftTuplesAndSetNullModelRSS(fuzzyTuples, shift, area);
  double unitDenominator = unitDenominatorGivenNullModelRSS();
  if (LastTrie::is01)
    {
      const double maxElementNegativeMembership = shift * (area / cardinalities.front());
      if (maxElementNegativeMembership > unitDenominator)
	{
	  unitDenominator = maxElementNegativeMembership;
	}
    }
  {
    vector<vector<string>>::iterator ids2LabelsInDimensionIt = ids2Labels.begin();
    setMetadataForDimension(0, area, shift, unitDenominator, *ids2LabelsInDimensionIt, fuzzyTuples);
    const unsigned int n = ids2Labels.end() - ids2LabelsInDimensionIt;
    unsigned int dimensionId = 1;
    do
      {
	setMetadataForDimension(dimensionId, area, shift, unitDenominator, *++ids2LabelsInDimensionIt, fuzzyTuples);
      }
    while (++dimensionId != n);
  }
  unit = static_cast<double>(numeric_limits<int>::max()) / unitDenominator;
  const vector<FuzzyTuple>::iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::iterator fuzzyTupleIt = fuzzyTuples.begin();
  do
    {
      fuzzyTupleIt->reorder(external2InternalDimensionOrder);
      ConcurrentPatternPool::addFuzzyTuple(fuzzyTupleIt->getTuple(), fuzzyTupleIt->getMembership());
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
}

void AbstractRoughTensor::fillTensor(const vector<FuzzyTuple>& fuzzyTuples, AbstractTrie& tensor)
{
  // Sparse tensor
  const unsigned int cardinalityOfLastDimension = cardinalities.back();
  const vector<FuzzyTuple>::const_iterator fuzzyTupleEnd = fuzzyTuples.end();
  vector<FuzzyTuple>::const_iterator fuzzyTupleIt = fuzzyTuples.begin();
  if (LastTrie::is01)
    {
      do
	{
	  tensor.setTuple(fuzzyTupleIt->getTuple().begin(), cardinalityOfLastDimension);
	}
      while (++fuzzyTupleIt != fuzzyTupleEnd);
      tensor.sortTubes();
      return;
    }
  do
    {
      tensor.setTuple(fuzzyTupleIt->getTuple().begin(), cardinalityOfLastDimension, unit * fuzzyTupleIt->getMembership());
    }
  while (++fuzzyTupleIt != fuzzyTupleEnd);
  tensor.sortTubes();
}

void AbstractRoughTensor::setMetadataForDimension(vector<pair<double, unsigned int>>& elementPositiveMembershipsInDimension, double& unitDenominator, vector<string>& ids2LabelsInDimension)
{
  // Dense tensor
  sort(elementPositiveMembershipsInDimension.begin(), elementPositiveMembershipsInDimension.end(), [](const pair<double, unsigned int>& elementPositiveMembership1, const pair<double, unsigned int>& elementPositiveMembership2) {return elementPositiveMembership1.first < elementPositiveMembership2.first;});
  // Computing the new ids, in increasing order of the positive membership (for faster lower_bound in SparseFuzzyTube::sumOnSlice and to choose the element with the greatest membership in case of equality) and reorder ids2LabelsInDimension accordingly
  vector<string> newIds2LabelsInDimension;
  newIds2LabelsInDimension.reserve(ids2LabelsInDimension.size());
  const vector<pair<double, unsigned int>>::const_iterator elementPositiveMembershipEnd = elementPositiveMembershipsInDimension.end();
  vector<pair<double, unsigned int>>::const_iterator elementPositiveMembershipIt = elementPositiveMembershipsInDimension.begin();
  do
    {
      newIds2LabelsInDimension.emplace_back(std::move(ids2LabelsInDimension[elementPositiveMembershipIt->second]));
    }
  while (++elementPositiveMembershipIt != elementPositiveMembershipEnd);
  ids2LabelsInDimension = std::move(newIds2LabelsInDimension);
  if (elementPositiveMembershipsInDimension.back().first > unitDenominator)
    {
      unitDenominator = elementPositiveMembershipsInDimension.back().first;
    }
}

void AbstractRoughTensor::setMetadata(vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, const double maxNegativeMembership)
{
  // Dense tensor
  orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities();
  double unitDenominator = unitDenominatorGivenNullModelRSS();
  if (maxNegativeMembership > unitDenominator)
    {
      unitDenominator = maxNegativeMembership;
    }
  vector<vector<pair<double, unsigned int>>>::iterator elementPositiveMembershipsInDimensionIt = elementPositiveMemberships.begin();
  vector<vector<string>>::iterator ids2LabelsInDimensionIt = ids2Labels.begin();
  setMetadataForDimension(*elementPositiveMembershipsInDimensionIt, unitDenominator, *ids2LabelsInDimensionIt);
  ++ids2LabelsInDimensionIt;
  const vector<vector<string>>::iterator ids2LabelsInDimensionEnd = ids2Labels.end();
  do
    {
      setMetadataForDimension(*++elementPositiveMembershipsInDimensionIt, unitDenominator, *ids2LabelsInDimensionIt);
    }
  while (++ids2LabelsInDimensionIt != ids2LabelsInDimensionEnd);
  unit = static_cast<double>(numeric_limits<int>::max()) / unitDenominator;
}

// TODO: if only used for dense, it should be elsewhere, shouldn't it?  And maybe make projectMetadataForDimension return newIds2OldIdsInDimension and use it directly (instead of storing for all dimension), as in SparseRoughTensor and in SparseRoughGraphTensor
void AbstractRoughTensor::projectMetadataForDimension(const unsigned int internalDimensionId, const bool isReturningOld2New, vector<string>& ids2LabelsInDimension, vector<unsigned int>& newIds2OldIdsInDimension)
{
  unsigned int& cardinality = cardinalities[internalDimensionId];
  dynamic_bitset<> elementsInDimension(cardinality);
  const vector<vector<vector<unsigned int>>>::const_iterator end = candidateVariables.end();
  vector<vector<vector<unsigned int>>>::const_iterator patternIt = candidateVariables.begin();
  do
    {
      const vector<unsigned int>::const_iterator idEnd = (*patternIt)[internalDimensionId].end();
      vector<unsigned int>::const_iterator idIt = (*patternIt)[internalDimensionId].begin();
      do
	{
	  elementsInDimension.set(*idIt);
	}
      while (++idIt != idEnd);
    }
  while (++patternIt != end);
  vector<unsigned int> oldIds2NewIdsInDimension(cardinality, numeric_limits<unsigned int>::max());
  cardinality = 0;
  if (isReturningOld2New)
    {
      dynamic_bitset<>::size_type id = elementsInDimension.find_first();
      do
	{
	  ids2LabelsInDimension[id].swap(ids2LabelsInDimension[cardinality]);
	  oldIds2NewIdsInDimension[id] = cardinality++;
	  id = elementsInDimension.find_next(id);
	}
      while (id != dynamic_bitset<>::npos);
    }
  else
    {
      newIds2OldIdsInDimension.reserve(elementsInDimension.count());
      dynamic_bitset<>::size_type id = elementsInDimension.find_first();
      do
	{
	  newIds2OldIdsInDimension.push_back(id);
	  ids2LabelsInDimension[id].swap(ids2LabelsInDimension[cardinality]);
	  oldIds2NewIdsInDimension[id] = cardinality++;
	  id = elementsInDimension.find_next(id);
	}
      while (id != dynamic_bitset<>::npos);
    }
  ids2LabelsInDimension.resize(cardinality);
  ids2LabelsInDimension.shrink_to_fit();
  const vector<vector<vector<unsigned int>>>::iterator candidateVariableEnd = candidateVariables.end();
  vector<vector<vector<unsigned int>>>::iterator candidateVariableIt = candidateVariables.begin();
  do
    {
      const vector<unsigned int>::iterator idEnd = (*candidateVariableIt)[internalDimensionId].end();
      vector<unsigned int>::iterator idIt = (*candidateVariableIt)[internalDimensionId].begin();
      do
	{
	  *idIt = oldIds2NewIdsInDimension[*idIt];
	}
      while (++idIt != idEnd);
    }
  while (++candidateVariableIt != candidateVariableEnd);
  if (isReturningOld2New)
    {
      newIds2OldIdsInDimension = std::move(oldIds2NewIdsInDimension);
    }
}

// TODO: if only used for dense, it should be elsewhere, shouldn't it?  And maybe make projectMetadataForDimension return newIds2OldIdsInDimension and use it directly (instead of storing for all dimension), as in SparseRoughTensor and in SparseRoughGraphTensor
vector<vector<unsigned int>> AbstractRoughTensor::projectMetadata(const bool isReturningOld2New)
{
  // Compute new cardinalities, new ids and rewrite ids2Labels and candidateVariables accordingly
  vector<vector<string>>::iterator ids2LabelsIt = ids2Labels.begin();
  const vector<unsigned int>::const_iterator internalDimensionIdEnd = external2InternalDimensionOrder.end();
  vector<unsigned int>::const_iterator internalDimensionIdIt = external2InternalDimensionOrder.begin();
  vector<vector<unsigned int>> idMapping(internalDimensionIdEnd - internalDimensionIdIt);
  projectMetadataForDimension(*internalDimensionIdIt, isReturningOld2New, *ids2LabelsIt, idMapping[*internalDimensionIdIt]);
  ++internalDimensionIdIt;
  do
    {
      projectMetadataForDimension(*internalDimensionIdIt, isReturningOld2New, *++ids2LabelsIt, idMapping[*internalDimensionIdIt]);
    }
  while (++internalDimensionIdIt != internalDimensionIdEnd);
  return idMapping;
}

double AbstractRoughTensor::getNullModelRSS()
{
  return nullModelRSS * unit * unit;
}

vector<vector<int>> AbstractRoughTensor::nullSumsOnHyperplanes()
{
  vector<unsigned int>::const_iterator cardinalityIt = cardinalities.begin();
  const vector<unsigned int>::const_iterator cardinalityEnd = cardinalities.end();
  vector<vector<int>> sumsOnHyperplanes;
  sumsOnHyperplanes.reserve(cardinalityEnd - cardinalityIt);
  do
    {
      sumsOnHyperplanes.emplace_back(*cardinalityIt);
    }
  while (++cardinalityIt != cardinalityEnd);
  return sumsOnHyperplanes;
}

#if defined DEBUG_MODIFY || defined ASSERT
void AbstractRoughTensor::printElement(const unsigned int dimensionId, const unsigned int elementId, ostream& out) const
{
  const unsigned int externalId = find(external2InternalDimensionOrder.begin(), external2InternalDimensionOrder.end(), dimensionId) - external2InternalDimensionOrder.begin();
  out << ids2Labels[externalId][elementId] << " of dimension " << externalId;
}

unsigned long long AbstractRoughTensor::area(const vector<vector<unsigned int>>& nSet) const
{
  return patternArea(nSet);
}
#endif

#ifdef TIME
void AbstractRoughTensor::printCurrentDuration()
{
#ifdef GNUPLOT
#if defined NUMERIC_PRECISION || defined NB_OF_PATTERNS || defined DETAILED_TIME
  cout << '\t';
#endif
  cout << duration_cast<duration<double>>(steady_clock::now() - overallBeginning).count();
#else
  cout << "Total time: " << duration_cast<duration<double>>(steady_clock::now() - overallBeginning).count() << "s\n";;
#endif
}
#endif
