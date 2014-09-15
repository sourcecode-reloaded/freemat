#ifndef __TypeUtils_hpp__
#define __TypeUtils_hpp__

// Utility functions for working with types
#include "CellType.hpp"
#include "StringType.hpp"
#include "ThreadContext.hpp"
#include "ListType.hpp"

namespace FM
{
  inline Object makeCellFromList(ThreadContext *ctxt, const Object &t) {
    Object p = ctxt->_cell->makeMatrix(1,t.elementCount());
    Object *q = ctxt->_cell->readWriteData(p);
    const Object *h = ctxt->_list->readOnlyData(t);
    for (size_t i=0;i<t.elementCount();i++)
      q[i] = h[i];
    return p;
  }

  inline Object makeCellFromStrings(ThreadContext *ctxt, const FMStringList &t) {
    Object p = ctxt->_cell->makeMatrix(1,t.size());
    Object *q = ctxt->_cell->readWriteData(p);
    for (size_t i=0;i<t.size();i++)
      q[i] = ctxt->_string->makeString(t[i]);
    return p;
  }

  inline FMStringList makeStringsFromCell(ThreadContext *ctxt, const Object &t) {
    assert(t.type()->code() == TypeCellArray);
    FMStringList ret;
    const Object *tptr = ctxt->_cell->readOnlyData(t);
    for (dim_t i=0;i<t.dims().elementCount();i++)
      ret << ctxt->_string->getString(tptr[i]);
    return ret;
  }

  inline void addIndexToList(ThreadContext *ctxt, Object &list, ndx_t val) {
    ctxt->_list->push(list,ctxt->_index->makeScalar(val));
  }

  inline void addStringToList(ThreadContext *ctxt, Object &list, const FMString &string) {
    ctxt->_list->push(list,ctxt->_string->makeString(string));
  }
  
  inline ndx_t indexOfStringInList(ThreadContext *ctxt, const Object &list, const FMString &string) {
    return ctxt->_list->indexOf(list,ctxt->_string->makeString(string));
  }
}

#endif
