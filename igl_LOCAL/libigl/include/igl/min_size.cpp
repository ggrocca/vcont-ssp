// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2013 Alec Jacobson <alecjacobson@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#include "min_size.h"

template <typename T>
IGL_INLINE int igl::min_size(const std::vector<T> & V)
{
  int min_size = -1;
  for(
    typename std::vector<T>::const_iterator iter = V.begin();
    iter != V.end(); 
    iter++)
  {
    int size = (int)iter->size();
    // have to handle base case
    if(min_size == -1)
    {
      min_size = size;
    }else{
      min_size = (min_size < size ? min_size : size);
    }
  }
  return min_size;
}



#ifndef IGL_HEADER_ONLY
// Explicit template specialization
// generated by autoexplicit.sh
template int igl::min_size<std::vector<int, std::allocator<int> > >(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > > const&);
// generated by autoexplicit.sh
template int igl::min_size<std::vector<double, std::allocator<double> > >(std::vector<std::vector<double, std::allocator<double> >, std::allocator<std::vector<double, std::allocator<double> > > > const&);
template int igl::min_size<std::vector<bool, std::allocator<bool> > >(std::vector<std::vector<bool, std::allocator<bool> >, std::allocator<std::vector<bool, std::allocator<bool> > > > const&);
#endif
