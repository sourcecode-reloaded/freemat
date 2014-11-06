#if 0
#define __BaseTypes_hpp__

#include "BoolType.hpp"
#include "IntegerType.hpp"
#include "DoubleType.hpp"
#include "SingleType.hpp"
#include "StringType.hpp"
#include "StructType.hpp"
#include "ListType.hpp"
#include "CellType.hpp"

namespace FM
{
  struct BaseTypes
  {
    BoolType *_bool;
    DoubleType *_double;
    SingleType *_single;
    StringType *_string;
    IndexType *_index;
    Int32Type *_int32;
    Int64Type *_int64;
    UInt32Type *_uint32;
    UInt64Type *_uint64;
    StructType *_struct;
    CellType *_cell;
    ListType *_list;

    BaseTypes() {
      _bool = new BoolType(this);
      _double = new DoubleType(this);
      _single = new SingleType(this);
      _string = new StringType(this);
      _index = new IndexType(this);
      _int32 = new Int32Type(this);
      _uint32 = new UInt32Type(this);
      _int64 = new Int64Type(this);
      _uint64 = new UInt64Type(this);
      _cell = new CellType(this);
      _struct = new StructType(this);
      _list = new ListType(this);
    }

    Object makeCellFromStrings(const FMStringList &t) {
      Object p = _cell->makeMatrix(1,t.size());
      Object *q = _cell->rw(p);
      for (size_t i=0;i<t.size();i++)
	q[i] = _string->makeString(t[i]);
      return p;
    }

    FMStringList makeStringsFromCell(const Object &t) {
      assert(t.type()->code() == TypeCellArray);
      FMStringList ret;
      const Object *tptr = _cell->ro(t);
      for (dim_t i=0;i<t.dims().elementCount();i++)
	ret << _string->getString(tptr[i]);
      return ret;
    }

    void addStringToList(Object &list, const FMString &string) {
      _list->push(list,_string->makeString(string));
    }

    ndx_t indexOfStringInList(const Object &list, const FMString &string) {
      return _list->indexOf(list,_string->makeString(string));
    }
  };
}


#endif
