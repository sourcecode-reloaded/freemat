#include "NCat.hpp"
#include "ListType.hpp"
#include "DoubleType.hpp"
#include "ClassType.hpp"
#include "ThreadContext.hpp"
#include "Type.hpp"
#include "TypeUtils.hpp"

using namespace FM;

static DataCode ComputeCatType(ThreadContext *ctxt, const Object& pdata, bool &conversion_required) {
  const Object *x = ctxt->_list->ro(pdata);
  DataCode mincode = TypeAnonymous;
  DataCode maxcode = TypeInvalid;
  DataCode firstInt = TypeInvalid;
  bool anySingle = false;
  for (int i=0;i<pdata.count();i++) {
    DataCode xcode = x[i].typeCode();
    mincode = std::min<DataCode>(xcode,mincode);
    maxcode = std::max<DataCode>(xcode,maxcode);
    if ((xcode >= TypeInt8) && (xcode <= TypeUInt64) && (firstInt == TypeInvalid))
      firstInt = xcode;
    if (xcode == TypeSingle) anySingle = true;
  }
  if (maxcode == TypeClass) {
    conversion_required = true;
    return maxcode;
  }
  // Test for invalid cases
  if (maxcode >= TypeListArray)
    throw Exception("Cannot use NCat on objects of type handle, class, module, function, etc.  Use a cell array instead");
  // Test for case of all elements of the same type
  if (mincode == maxcode) {
    conversion_required = false;
    return mincode;
  }
  conversion_required = true;
  if (mincode < TypeBool) return mincode;
  if (firstInt != TypeInvalid) return firstInt;
  // So there are no integers, no cell, struct, strings
  // All that is left is logical, float, double.  Next we 
  // look for floats, then doubles, and last, bools
  if (anySingle) return TypeSingle;
  return maxcode;
}

Object FM::NCat(ThreadContext *ctxt, const Object &x, int dimension) {
  if (x.count() == 0) return ctxt->_double->empty();
  if (x.count() == 1) return ctxt->_list->first(x);
  bool conversion_required = false;
  DataCode finalType = ComputeCatType(ctxt,x,conversion_required);
  if (finalType == TypeClass) return ctxt->_class->NCat(x,dimension);
  if (!conversion_required)
    return ctxt->_list->first(x).type()->NCat(x,dimension);
  Object xconv = convertToCode(ctxt,x,finalType);
  return ctxt->_list->first(xconv).type()->NCat(xconv,dimension);
}
