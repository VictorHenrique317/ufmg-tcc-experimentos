// Copyright 2024 Loïc Cerf (lcerf@dcc.ufmg.br)

// This file is part of nclusterbox.

// nclusterbox is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

// nclusterbox is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along with nclusterbox.  If not, see <https://www.gnu.org/licenses/>.

#include "Tube.h"

Tube::~Tube()
{
}

bool Tube::isFullSparseTube(const unsigned int size) const
{
  return false;
}

void Tube::set(const unsigned int element)
{
  // Never called
}

void Tube::set(const unsigned int element, const int membership)
{
  // Never called
}

void Tube::sortTubes()
{
}
