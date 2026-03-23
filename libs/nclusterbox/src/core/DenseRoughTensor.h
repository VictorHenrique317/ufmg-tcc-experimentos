// Copyright 2018-2023 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#ifndef DENSE_ROUGH_TENSOR_H_
#define DENSE_ROUGH_TENSOR_H_

#include "AbstractDenseRoughTensor.h"

class DenseRoughTensor final : public AbstractDenseRoughTensor
{
 public:
  DenseRoughTensor(const char* tensorFileName, const char* inputDimensionSeparator, const char* inputElementSeparator, const bool isInput01, const bool isVerbose);
  DenseRoughTensor(vector<FuzzyTuple>& fuzzyTuples, const double constantShift);

  AbstractTrie* getTensor() const;
  VisitedPatterns* getEmptyVisitedPatterns(const float availableBytes, const unsigned int nbOfJobs, const bool isNoSelection) const;
  AbstractTrieWithPrediction* projectTensor();

 private:
  void init(vector<FuzzyTuple>& fuzzyTuples);
  double updateNullModelRSSAndElementMembershipsAndAdvance(vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<unsigned int, unsigned int>>>& elementPresences);
  double updateNullModelRSSAndElementMembershipsAndAdvance(vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, vector<vector<double>>& elementNegativeMemberships);
  void updateNullModelRSSAndElementMemberships(const vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<unsigned int, unsigned int>>>& elementPresences);
  void updateNullModelRSSAndElementMemberships(const vector<unsigned int>& tuple, const double shiftedMembership, vector<vector<pair<double, unsigned int>>>& elementPositiveMemberships, vector<vector<double>>& elementNegativeMemberships);
};

#endif /*DENSE_ROUGH_TENSOR_H_*/
