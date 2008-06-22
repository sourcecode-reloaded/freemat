/*
 * Copyright (c) 2002-2006 Samit Basu
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

#include "Utils.hpp"
#include "Exception.hpp"
#include "IEEEFP.hpp"
#include <math.h>


bool contains(StringVector& list, QString s, bool regexpmode) {
  QRegExp t;
  for (int i=0;i<list.size();i++) {
    if (regexpmode) {
      t = QRegExp(list[i]);
      if (t.exactMatch(s) &&
	  (t.matchedLength() == s.size())) return true;
    } else {
      if (list[i] == s) return true;
    }
  }
  return false;
}

NTuple ArrayVectorAsDimensions(const ArrayVector &arg) {
  NTuple dims;
  if (arg.size() == 0)
    return NTuple(1,1);
  // Case 1 - all of the entries are scalar
  bool allScalars;
  allScalars = true;
  for (int i=0;i<arg.size();i++)
    allScalars &= arg[i].isScalar();
  if (allScalars) {
    if (arg.size() == 1) {
      // If all scalars and only one argument - we want a square matrix
      dims.set(0,arg[0].asInteger());
      dims.set(1,arg[0].asInteger());
    } else {
      // If all scalars and and multiple arguments, we count dimensions
      for (int i=0;i<arg.size();i++) 
	dims.set(i,arg[i].asInteger());
    }
  } else {
    if (arg.size() > 1)
      throw Exception("Dimension arguments must be either all scalars or a single vector");
    Array t = arg[0].toClass(UInt32);
    const BasicArray<uint32> &td(t.constReal<uint32>());
    for (index_t i=1;i<=t.length();i++)
      dims.set(int(i-1),td.get(i));
  }
  return dims;
}

double ArrayRange(const Array& dp) {
  if ((dp.dataClass() != Float) && (dp.dataClass() != Double))
    throw Exception("Unsupported type for function");
  const BasicArray<double> &rp(dp.asDenseArray().toClass(Double).constReal<double>());
  if (rp.length() == 0) return 0;
  double result;
  bool init = false;
  for (index_t i=1;i!=rp.length();i++) {
    if (!IsNaN(rp[i])) {
      if (!init) {
	init = true;
	result = fabs(rp[i]);
      } else {
	if (fabs(rp[i]) > result) 
	  result = fabs(rp[i]);
      }
    }
  }
  if (!init) return NaN();
  return result;
}
  
