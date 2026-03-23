// Copyright 2018-2025 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ABSTRACT_ROUGH_TENSOR_H_
#define ABSTRACT_ROUGH_TENSOR_H_

#include <string>
#include <fstream>

#include "FuzzyTuple.h"
#include "AbstractTrie.h"
#include "VisitedPatterns.h"
#include "AbstractTrieWithPrediction.h"

#if defined TIME || defined DETAILED_TIME
#include <chrono>

using namespace std::chrono;
#endif

class AbstractRoughTensor
{
 public:
  static vector<vector<vector<unsigned int>>> candidateVariables;

  virtual ~AbstractRoughTensor();

  virtual void setNoSelection() = 0;
  virtual AbstractTrie* getTensor() const = 0;
  virtual bool wouldBeEmptyAfterProjection() = 0;
  virtual AbstractTrieWithPrediction* projectTensor() = 0;
  virtual double getAverageShift(const vector<vector<unsigned int>>& nSet) const = 0;

  virtual VisitedPatterns* getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelection) const = 0;
  virtual void printPattern(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const;
  void output(const vector<vector<unsigned int>>& nSet, const float density) const;
  void output(const vector<vector<unsigned int>>& nSet, const float density, const double rss) const;

  static AbstractRoughTensor* makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const bool isInput01, const bool isVerbose);
  static AbstractRoughTensor* makeRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const double densityThreshold, const double shift, const bool isInput01, const bool isVerbose);

  static unsigned long long patternArea(const vector<vector<unsigned int>>& nSet);
  static void setOutput(const char* outputFileName, const char* outputDimensionSeparator, const char* outputElementSeparator, const char* sizePrefix, const char* sizeSeparator, const char* areaPrefix, const char* rssPrefix, const bool isPrintLambda, const bool isSizePrinted, const bool isAreaPrinted);
  static int getUnit();
  static bool isDirectOutput();
  static const vector<vector<string>>& getIds2Labels();
  static const vector<unsigned int>& getExternal2InternalDimensionOrder();
  static unsigned long long getArea();
  static double getNullModelRSS();
  static vector<vector<int>> nullSumsOnHyperplanes();

#if defined DEBUG_MODIFY || defined ASSERT
  virtual void printElement(const unsigned int dimensionId, const unsigned int elementId, ostream& out) const;
  virtual unsigned long long area(const vector<vector<unsigned int>>& nSet) const;
#endif
#ifdef TIME
  static void printCurrentDuration();
#endif

 protected:
  static vector<vector<string>> ids2Labels;
  static double nullModelRSS;
  static vector<unsigned int> cardinalities; // of the nSet, hence one cardinality less if a graph tensor
  static int unit;
  static bool isNoSelection;
  static vector<unsigned int> external2InternalDimensionOrder;

  static string sizePrefix;
  static string sizeSeparator;
  static string areaPrefix;
  static bool isSizePrinted;
  static bool isAreaPrinted;

#if defined TIME || defined DETAILED_TIME
  static steady_clock::time_point overallBeginning;
#ifdef DETAILED_TIME
  static steady_clock::time_point shiftingBeginning;
#endif
#endif

  void printPatternDensityAndSizes(const vector<vector<unsigned int>>& nSet, const float density, ostream& out) const;
  unsigned int nbOfElements() const;

  static double membershipSum(const vector<FuzzyTuple>& fuzzyTuples);
  static void printDimension(const vector<unsigned int>& dimension, const vector<string>& ids2LabelsInDimension, ostream& out);
  static double unitDenominatorGivenNullModelRSS();
  static vector<FuzzyTuple> getFuzzyTuples(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const bool isVerbose);
  static void setUnitForProjectedTensor(const double rss, const double maxMembership);
  static void setMetadataForDimension(const unsigned int dimensionId, const unsigned long long area, const double shift, double& unitDenominator, vector<string>& ids2LabelsInDimension, vector<FuzzyTuple>& fuzzyTuples);
  template<typename T> static vector<pair<T, unsigned int>> pairs0AndId(const unsigned int size);
  template<typename T> static vector<unsigned int> sortByPositiveMemberships(vector<pair<T, unsigned int>>& elementPositiveMemberships, vector<string>& ids2LabelsInDimension);
  static void shiftTuplesAndSetNullModelRSS(vector<FuzzyTuple>& fuzzyTuples, const double shift, const unsigned long long area);
  static void setMetadata(vector<FuzzyTuple>& fuzzyTuples, const double shift);
  static void fillTensor(const vector<FuzzyTuple>& fuzzyTuples, AbstractTrie& tensor);
  static void setMetadataForDimension(vector<pair<double, unsigned int>>& elementPositiveMembershipsInDimension, double& unitDenominator, vector<string>& ids2LabelsInDimension);
  static void setMetadata(vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, const double maxNegativeMembership); /* the inner vectors of elementPositiveMemberships are reordered by increasing element membership, hence a mapping from new ids (the index) and old ids (the second components of the pairs) */
  static void projectMetadataForDimension(const unsigned int internalDimensionId, const bool isReturningOld2New, vector<string>& ids2LabelsInDimension, vector<unsigned int>& newIds2OldIdsInDimension);
  static vector<vector<unsigned int>> projectMetadata(const bool isReturningOld2New);

 private:
  static ostream outputStream;
  static ofstream outputFile;
  static string outputDimensionSeparator;
  static string outputElementSeparator;
  static string rssPrefix;
  static bool isPrintLambda;

  static AbstractRoughTensor* makeRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double densityThreshold, const double shift);
  static void orderDimensionsAndSetExternal2InternalDimensionOrderAndCardinalities();
  static unsigned long long getAreaFromIds2Labels();
};

#endif /*ABSTRACT_ROUGH_TENSOR_H_*/
