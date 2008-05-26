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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include "Utils.hpp"
#include "IEEEFP.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include <math.h>
#include "Types.hpp"
#include <algorithm>
#include "SparseMatrix.hpp"
#include "Math.hpp"
#include "LAPACK.hpp"
#include "MemPtr.hpp"
#include <QtCore>


//!
//@Module PCODE Convert a Script or Function to P-Code
//@@Section FREEMAT
//@@Usage
//Writes out a script or function as a P-code function.
//The general syntax for its use is:
//@[
//   pcode fun1 fun2 ...
//@]
//The compiled functions are written to the current
//directory.
//!
ArrayVector PCodeFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  throw Exception("P-code functions currently unsupported");
}

template <class T>
static void Unique(const BasicArray<T> &data,
		   BasicArray<T> &uniqueValues,
		   BasicArray<index_t> &uniqueIndices,
		   BasicArray<index_t> &dataIndices) {
  
}
  

template <class T>
class UniqueEntryReal {
public:
  uint32 n;
  int len;
  uint32 stride;
  const T* data;
};

template <class T>
bool operator<(const UniqueEntryReal<T>& a, const UniqueEntryReal<T>& b) {
  int i;
  i = 0;
  while (i<a.len) {
    if (a.data[i*a.stride] < b.data[i*b.stride]) 
      return true; 
    else if (a.data[i*a.stride] > b.data[i*b.stride]) 
      return false; 
    i++;
  }
  return false;
}
  
template <class T>
bool operator==(const UniqueEntryReal<T>& a, const UniqueEntryReal<T>& b) {
  int i;
  i = 0;
  while (i<a.len) {
    if (a.data[i*a.stride] != b.data[i*b.stride]) return false;
    i++;
  }
  return true;
}
  
template <class T>
class UniqueEntryComplex {
public:
  uint32 n;
  int len;
  uint32 stride;
  const T* data;
};

template <class T>
bool operator<(const UniqueEntryComplex<T>& a, const UniqueEntryComplex<T>& b) {
  int i;
  T a_abs, b_abs;
  i = 0;
  while (i<a.len) {
    a_abs = complex_abs(a.data[2*i*a.stride],a.data[2*i*a.stride+1]);
    b_abs = complex_abs(b.data[2*i*b.stride],b.data[2*i*b.stride+1]);
    if (a_abs < b_abs) 
      return true;
    else if (a_abs > b_abs)
      return false;
    i++;
  }
  return false;
}
  
template <class T>
bool operator==(const UniqueEntryComplex<T>& a, const UniqueEntryComplex<T>& b) {
  int i;
  i = 0;
  while (i<a.len) {
    if ((a.data[2*i*a.stride] != b.data[2*i*b.stride]) || 
	(a.data[2*i*a.stride+1] != b.data[2*i*b.stride+1])) return false;
    i++;
  }
  return true;
}
  
  
//!
//@Module UNIQUE Unique
//@@Section ARRAY
//@@Usage
//Returns a vector containing the unique elements of an array.  The first
//form is simply
//@[
//   y = unique(x)
//@]
//where @|x| is either a numerical array or a cell-array of strings.  The 
//result is sorted in increasing order.  You can also retrieve two sets
//of index vectors
//@[
//   [y, m, n] = unique(x)
//@]
//such that @|y = x(m)| and @|x = y(n)|.  If the argument @|x| is a matrix,
//you can also indicate that FreeMat should look for unique rows in the
//matrix via
//@[
//   y = unique(x,'rows')
//@]
//and
//@[
//   [y, m, n] = unique(x,'rows')
//@]
//@@Example
//Here is an example in row mode
//@<
//A = randi(1,3*ones(15,3))
//unique(A,'rows')
//[b,m,n] = unique(A,'rows');
//b
//A(m,:)
//b(n,:)
//@>
//Here is an example in vector mode
//@<
//A = randi(1,5*ones(10,1))
//unique(A)
//[b,m,n] = unique(A,'rows');
//b
//A(m)
//b(n)
//@>
//For cell arrays of strings.
//@<
//A = {'hi','bye','good','tell','hi','bye'}
//unique(A)
//@>
//!

template <class T>
ArrayVector UniqueFunctionRowModeComplex(int nargout, Array& input) {
  const T* dp = (const T*) input.getDataPointer();
  int rows = input.getDimensionLength(0);
  int cols = input.getDimensionLength(1); 
  int len = rows;
  Class cls(input.dataClass());
  int i, j;
  int cnt;
  UniqueEntryComplex<T> *sp = new UniqueEntryComplex<T>[len];
  for (i=0;i<len;i++) {
    sp[i].n = i;
    sp[i].len = cols;
    sp[i].stride = rows;
    sp[i].data = dp + 2*i;
  }
  std::stable_sort(sp,sp+len);
  i = 1;
  cnt = 1;
  while (i < len) {
    if (!(sp[i] == sp[i-1]))
      cnt++;
    i++;
  }
  int tcnt = cnt;
  if (nargout <= 1) {
    T* op = (T*) Malloc(sizeof(T)*cnt*2*cols);
    for (j=0;j<cols;j++) {
      op[0+j*2*tcnt] = sp[0].data[0+j*2*rows];
      op[1+j*2*tcnt] = sp[0].data[1+j*2*rows];
    }
    i = 1;
    cnt = 1;
    while (i < len) {
      if (!(sp[i] == sp[i-1])) {
	for (j=0;j<cols;j++) {
	  op[2*cnt+j*2*tcnt] = sp[i].data[0+j*2*rows];
	  op[2*cnt+j*2*tcnt+1] = sp[i].data[1+j*2*rows];
	}
	cnt++;
      }
      i++;
    }
    delete[] sp;
    return SingleArrayVector(Array(cls,Dimensions(cnt,cols),op));
  } else {
    uint32* np = (uint32*) Malloc(sizeof(int32)*len);
    uint32* mp = (uint32*) Malloc(sizeof(int32)*cnt);
    T* op = (T*) Malloc(sizeof(T)*cnt*2*cols);
    for (j=0;j<cols;j++) {
      op[0+j*2*tcnt] = sp[0].data[0+j*2*rows];
      op[1+j*2*tcnt] = sp[0].data[1+j*2*rows];
    }
    i = 1;
    cnt = 1;
    np[sp[0].n] = 1;
    mp[0] = sp[0].n + 1;
    while (i < len) {
      if (!(sp[i] == sp[i-1])) {
	for (j=0;j<cols;j++) {
	  op[2*cnt+j*2*tcnt] = sp[i].data[0+j*2*rows];
	  op[2*cnt+j*2*tcnt+1] = sp[i].data[1+j*2*rows];
	}
	mp[cnt] = sp[i].n + 1;
	cnt++;
      }
      np[sp[i].n] = cnt;
      i++;
    }
    delete[] sp;
    ArrayVector retval;
    retval.push_back(Array(cls,Dimensions(cnt,cols),op));
    retval.push_back(Array(FM_UINT32,Dimensions(cnt,1),mp));
    retval.push_back(Array(FM_UINT32,Dimensions(len,1),np));
    return retval;
  }
}

template <class T>
ArrayVector UniqueFunctionRowModeReal(int nargout, Array& input) {
  const T* dp = (const T*) input.getDataPointer();
  int rows = input.getDimensionLength(0);
  int cols = input.getDimensionLength(1); 
  int len = rows;
  Class cls(input.dataClass());
  int i, j;
  int cnt;
  UniqueEntryReal<T> *sp = new UniqueEntryReal<T>[len];
  for (i=0;i<len;i++) {
    sp[i].n = i;
    sp[i].len = cols;
    sp[i].stride = rows;
    sp[i].data = dp + i;
  }
  std::stable_sort(sp,sp+len);
  i = 1;
  cnt = 1;
  while (i < len) {
    if (!(sp[i] == sp[i-1]))
      cnt++;
    i++;
  }
  int tcnt = cnt;
  if (nargout <= 1) {
    T* op = (T*) Malloc(sizeof(T)*cnt*cols);
    for (j=0;j<cols;j++)
      op[0+j*tcnt] = sp[0].data[0+j*rows];
    i = 1;
    cnt = 1;
    while (i < len) {
      if (!(sp[i] == sp[i-1])) {
	for (j=0;j<cols;j++)
	  op[cnt+j*tcnt] = sp[i].data[0+j*rows];
	cnt++;
      }
      i++;
    }
    delete[] sp;
    return SingleArrayVector(Array(cls,Dimensions(cnt,cols),op));
  } else {
    uint32* np = (uint32*) Malloc(sizeof(int32)*len);
    uint32* mp = (uint32*) Malloc(sizeof(int32)*cnt);
    T* op = (T*) Malloc(sizeof(T)*cnt*cols); 
    for (j=0;j<cols;j++)
      op[0+j*tcnt] = sp[0].data[0+j*rows];
    i = 1;
    cnt = 1;
    np[sp[0].n] = 1;
    mp[0] = sp[0].n + 1;
    while (i < len) {
      if (!(sp[i] == sp[i-1])) {
	for (j=0;j<cols;j++)
	  op[cnt+j*tcnt] = sp[i].data[0+j*rows];
	mp[cnt] = sp[i].n + 1;
	cnt++;
      }
      np[sp[i].n] = cnt;
      i++;
    }
    delete[] sp;
    ArrayVector retval;
    retval.push_back(Array(cls,Dimensions(cnt,cols),op));
    retval.push_back(Array(FM_UINT32,Dimensions(cnt,1),mp));
    retval.push_back(Array(FM_UINT32,Dimensions(len,1),np));
    return retval;
  }
}

ArrayVector UniqueFunctionString(int nargout, Array& input) {
  int len(input.getLength());
  if (!VerifyAllStrings((Array*) input.getDataPointer(),len))
    throw Exception("when 'unique' is applied to cell arrays, each cell must contain a string");
  XSEntry *buf = new XSEntry[len];
  Array *sp = (Array*) input.getDataPointer();
  int i;
  for (i=0;i<len;i++) {
    buf[i].x = sp[i].getContentsAsString();
    buf[i].n = i;
  }
  sortreverse = false;
  std::stable_sort(buf,buf+len);
  i = 1;
  int cnt = 1;
  while (i < len) {
    if (!(buf[i] == buf[i-1]))
      cnt++;
    i++;
  }
  if (nargout <= 1) {
    Array *op = new Array[cnt];
    op[0] = sp[buf[0].n];
    i = 1;
    cnt = 1;
    while (i < len) {
      if (!(buf[i] == buf[i-1])) {
	op[cnt] = sp[buf[i].n];
	cnt++;
      }
      i++;
    }
    delete[] buf;
    return SingleArrayVector(Array(FM_CELL_ARRAY,Dimensions(cnt,1),op));
  } else {
    uint32* np = (uint32*) Malloc(sizeof(int32)*len);
    uint32* mp = (uint32*) Malloc(sizeof(int32)*cnt);
    Array *op = new Array[cnt];
    op[0] = sp[buf[0].n];
    i = 1;
    cnt = 1;
    np[buf[0].n] = 1;
    mp[0] = buf[0].n + 1;
    while (i < len) {
      if (!(buf[i] == buf[i-1])) {
	op[cnt] = sp[buf[i].n];
	mp[cnt] = buf[i].n + 1;
	cnt++;
      }
      np[buf[i].n] = cnt;
      i++;
    }
    delete[] buf;
    ArrayVector retval;
    retval.push_back(Array(FM_CELL_ARRAY,Dimensions(cnt,1),op));
    retval.push_back(Array(FM_UINT32,Dimensions(cnt,1),mp));
    retval.push_back(Array(FM_UINT32,Dimensions(len,1),np));
    return retval;
  }
}
  
ArrayVector UniqueFunctionAux(int nargout, Array input, bool rowmode) {
  if ((input.dataClass() == FM_CELL_ARRAY) || (!rowmode)) {
    Dimensions newdim(input.getLength(),1);
    input.reshape(newdim);
  }
  Class argType(input.dataClass());
  switch (argType) {
  default: throw Exception("Unsupported type for unique operation");
  case FM_INT8: 
    return UniqueFunctionRowModeReal<int8>(nargout, input);
  case FM_UINT8:
    return UniqueFunctionRowModeReal<uint8>(nargout, input);
  case FM_INT16: 
    return UniqueFunctionRowModeReal<int16>(nargout, input);
  case FM_UINT16:
    return UniqueFunctionRowModeReal<uint16>(nargout, input);
  case FM_INT32: 
    return UniqueFunctionRowModeReal<int32>(nargout, input);
  case FM_UINT32:
    return UniqueFunctionRowModeReal<uint32>(nargout, input);
  case FM_INT64: 
    return UniqueFunctionRowModeReal<int64>(nargout, input);
  case FM_UINT64:
    return UniqueFunctionRowModeReal<uint64>(nargout, input);
  case FM_FLOAT: 
    return UniqueFunctionRowModeReal<float>(nargout, input);
  case FM_DOUBLE:
    return UniqueFunctionRowModeReal<double>(nargout, input);
  case FM_COMPLEX: 
    return UniqueFunctionRowModeComplex<float>(nargout, input);
  case FM_DCOMPLEX:
    return UniqueFunctionRowModeComplex<double>(nargout, input);
  case FM_CELL_ARRAY:
    return UniqueFunctionString(nargout, input);
  }
  throw Exception("Unsupported type in call to unique");
  return ArrayVector();
}

ArrayVector UniqueFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("unique function requires at least one argument");
  Array input(arg[0]);
  if (input.isEmpty()) {
    if (nargout <= 1)
      return SingleArrayVector(Array::emptyConstructor());
    else {
      ArrayVector retval;
      retval.push_back(Array::emptyConstructor());
      retval.push_back(Array::emptyConstructor());
      retval.push_back(Array::emptyConstructor());
      return retval;
    }
  }
  bool rowmode = false;
  if (arg.size() == 2) {
    Array Sdir(arg[1]);
    if (!Sdir.isString())
      throw Exception("second argument to unique must be 'rows'");
    const char *dp = (const char*) Sdir.getDataPointer();
    if ((dp[0] == 'r') || (dp[0] == 'R'))
      rowmode = true;
    else
      throw Exception("second argument to unique must be 'rows'");
  }
  // Get the input dimensions
  Dimensions inDim(input.dimensions());
  if (rowmode && (inDim.getLength() != 2))
    throw Exception("'rows' mode only works for matrix (2D) arguments");
  return UniqueFunctionAux(nargout, input, rowmode);
}


//!
//@Module XNRM2 BLAS Norm Calculation
//@@Section ARRAY
//@@Usage
//Calculates the 2-norm of a vector.  The syntax for its use
//is
//@[
//   y = xnrm2(A)
//@]
//where @|A| is the n-dimensional array to analyze.  This form
//uses the underlying BLAS implementation to compute the 2-norm.
//!
ArrayVector XNrm2Function(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("xnrm2 requires at least one argument");
  Array input(arg[0]);
  Class argType(input.dataClass());
  if (input.isReferenceType())
    throw Exception("xnrm2 does not apply to reference types");
  if ((argType < FM_FLOAT) || (argType == FM_STRING)) {
    input.promoteType(FM_DOUBLE);
    argType = input.dataClass();
  }
  switch (argType) {
  default: throw Exception("Unsupported type for xnrm2 operation");
  case FM_FLOAT: {
    float *ptr = (float*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::floatConstructor(snrm2_(&len,ptr,&one)));
  }
  case FM_DOUBLE:  {
    double *ptr = (double*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::doubleConstructor(dnrm2_(&len,ptr,&one)));
  }
  case FM_COMPLEX:  {
    float *ptr = (float*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::floatConstructor(scnrm2_(&len,ptr,&one)));
  }
  case FM_DCOMPLEX: {
    double *ptr = (double*) input.getDataPointer();
    int len = input.getLength();
    int one = 1;
    return SingleArrayVector(Array::doubleConstructor(dznrm2_(&len,ptr,&one)));
  }
  }
  return ArrayVector();
}

