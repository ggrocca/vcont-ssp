// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "face_occurences.h"

#include <map>
#include "sort.h"
#include <cassert>

template <typename IntegerF, typename IntegerC>
IGL_INLINE void igl::face_occurences(
  const std::vector<std::vector<IntegerF> > & F,
  std::vector<IntegerC> & C)
{
  using namespace std;

  // Get a list of sorted faces
  vector<vector<IntegerF> > sortedF = F;
  for(int i = 0; i < (int)F.size();i++)
  {
    sort(sortedF[i].begin(),sortedF[i].end());
  }
  // Count how many times each sorted face occurs
  map<vector<IntegerF>,int> counts;
  for(int i = 0; i < (int)sortedF.size();i++)
  {
    if(counts.find(sortedF[i]) == counts.end())
    {
      // initialize to count of 1
      counts[sortedF[i]] = 1;
    }else
    {
      // increment count
      counts[sortedF[i]]++;
      assert(counts[sortedF[i]] == 2);
    }
  }

  // Resize output to fit number of ones
  C.resize(F.size());
  for(int i = 0;i< (int)F.size();i++)
  {
    // sorted face should definitely be in counts map
    assert(counts.find(sortedF[i]) != counts.end());
    C[i] = counts[sortedF[i]];
  }
}

#ifndef IGL_HEADER_ONLY
// Explicit template specialization
template void igl::face_occurences<int, int>(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&, std::vector<int, std::allocator<int> >&);
#endif
