#include "NCat.hpp"
#include "ListType.hpp"
#include "DoubleType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

/*
static DataClass ComputeCatType(const ArrayVector& pdata) {
  if (AnyOfType(pdata,CellArray))
    return CellArray;
  if (AnyOfType(pdata,Struct))
    return Struct;
  if (AnyOfType(pdata,StringArray))
    return StringArray;
  // Check for an integer type - choose the first one found.
  for (int i=0;i<pdata.size();i++) {
    if (pdata[i].dataClass() == Int8 ||
	pdata[i].dataClass() == UInt8 ||
	pdata[i].dataClass() == Int16 ||
	pdata[i].dataClass() == UInt16 ||
	pdata[i].dataClass() == Int32 ||
	pdata[i].dataClass() == UInt32 ||
	pdata[i].dataClass() == Int64 ||
	pdata[i].dataClass() == UInt64)
      return pdata[i].dataClass();
  }
  // So there are no integers, no cell, struct, strings
  // All that is left is logical, float, double.  Next we 
  // look for floats, then doubles, and last, bools
  if (AnyOfType(pdata,Float)) return Float;
  if (AnyOfType(pdata,Double)) return Double;
  return Bool;
}


// FIX it here... [0 0] [3 0 4] --> [3 0 4]
Array NCat(const ArrayVector& pdata, int catdim) {
  // Must have something to work with
  if (pdata.size() == 0) return EmptyConstructor();
  // First pass, compute the output class
  DataClass cls = ComputeCatType(pdata);
  // Check for the case of concatenation of user defined classes
  bool userClassCase = false;
  QString classname;
  if ((cls == Struct) && pdata[0].isUserClass()) {
    classname = pdata[0].className();
    for (int i=1;i<pdata.size();i++)
      if (!pdata[i].isUserClass() || pdata[i].className() != classname)
	throw Exception("Cannot concatenate user defined classes of different type");
    userClassCase = true;
  }
  // Next pass, if we are in the case of a cell array, we convert the arrays to cells
  // We also strip the empties
  ArrayVector data;
  for (int i=0;i<pdata.size();i++) {
    if ((pdata[i].dimensions() != NTuple(0,0)) &&
        (pdata[i].dimensions() != NTuple(1,0)) &&
        (pdata[i].dimensions() != NTuple(0,1))   ) {
      if (cls == CellArray) {
	if (pdata[i].dataClass() == CellArray)
	  data.push_back(pdata[i]);
	else
	  data.push_back(CellArrayFromArray(pdata[i]));
      } else 
	data.push_back(pdata[i]);
    }
  }
  // Next step, we compute the output dimensions
  NTuple outdims;
  // Compute the output dimensions and validate each of the elements
  bool foundNonzero = false;
  for (int i=0;i<data.size();i++) {
    NTuple eldims(data[i].dimensions());
    if (eldims != NTuple(0,0)) {
      if (!foundNonzero) {
	foundNonzero = true;
	outdims = eldims;
      } else {
	for (int j=0;j<NDims;j++)
	  if ((eldims[j] != outdims[j]) && (j != catdim))
	    throw Exception(QString("Mismatch in dimension %1 for elements being concatenated along dimension %2").arg(j+1).arg(catdim+1));
	outdims[catdim] += eldims[catdim];
      }
    }
  }
  if (!foundNonzero) outdims = NTuple(0,0);
  // Compute the output dataclass
  if (data.size() == 0) return Array(cls,outdims);
  Array retval(cls,outdims);
  ArrayIterator iter(&retval,catdim);
  while (iter.isValid()) {
    int selector = 0;
    index_t adjusted = 1;
    for (index_t i=1;i<=iter.size();i++) {
      while (adjusted > data[selector].dimensions()[catdim]) {
	adjusted -= data[selector].dimensions()[catdim];
	selector++;
      }
      iter.set(data[selector].get(iter.pos().replace(catdim,adjusted)).toClass(cls));
      adjusted++;
      iter.next();
    }
    iter.nextSlice();
  }
  if (userClassCase)
    retval.structPtr().setClassName(classname);
  return retval;
}
*/

Object FM::NCat(ThreadContext *ctxt, const Object &x, int dimension) {
  if (x.count() == 0) return ctxt->_double->empty();
  if (x.count() == 1) return ctxt->_list->first(x);
  Object ret = ctxt->_list->first(x).type()->NCat(x,dimension);
  return ret;
}
