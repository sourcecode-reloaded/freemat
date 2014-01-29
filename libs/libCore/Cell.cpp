/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Array.hpp"
#include "Utils.hpp"

//@@Signature
//function cell CellFunction
//inputs varargin
//outputs y
//DOCBLOCK array_cell
ArrayVector CellFunction(int nargout, const ArrayVector& arg) {
  Array p(CellArray,ArrayVectorAsDimensions(arg));
  BasicArray<Array> &dp(p.real<Array>());
  for (index_t i=1;i<=dp.length();i++) {
    dp[i] = Array(Double);
  }
  return ArrayVector(p);
}

//@@Signature
//function cellslices CellSlicesFunction
//inputs varargin
//outputs y
ArrayVector CellSlicesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("cellslices expects three arguments: x, lb, ub - with an optional fourth argument dim");
  Array x = arg[0];
  Array lb = arg[1];
  Array ub = arg[2];
  index_t dim = 0;
  if (arg.size() > 3)
    {
      Array dimarg = arg[3].toClass(Index);
      dim = dimarg.realScalar<index_t>()-1;
    }
  if (lb.length() != ub.length())
    throw Exception("cellslices expects lb and ub to be the same length");
  BasicArray<index_t> lbdata = lb.toClass(Index).asDenseArray().constReal<index_t>();
  BasicArray<index_t> ubdata = ub.toClass(Index).asDenseArray().constReal<index_t>();
  int n = lbdata.length();
  Array ret(CellArray,NTuple(1,n));
  BasicArray<Array> &dp(ret.real<Array>());
  // Loop over the slices
  for (int i=0;i<n;i++)
    {
      // Calculate the size for this slice
      index_t lbval = lbdata[i+1];
      index_t ubval = ubdata[i+1];
      std::cout << "Slice from " << lbval << " to " << ubval << "\n";
      NTuple xdims = x.dimensions();
      xdims[dim] = std::max<index_t>((ubval-lbval)+1,0);
      if (xdims.count() == 0)
	dp[NTuple(1,i+1)] = Array(x.dataClass(),xdims);
      else
	{
	  // Have to do something.
	  // Build an index array
	  IndexArrayVector subset;
	  // Fill with colons
	  for (int j=0;j<NDims;j++)
	    subset.push_back(IndexRange(1,xdims[j]));
	  // Replace the dimension with a range
	  subset[dim] = IndexRange(lbval,ubval);
	  // Print them out
	  for (int j=0;j<NDims;j++)
	    {
	      std::cout << "Dim " << j << "\n";
	      subset[j].printMe(std::cout);
	    }
	  // Stuff it in
	  dp[NTuple(1,i+1)] = x.get(subset);
	}
    }
  return ArrayVector(ret);
}
