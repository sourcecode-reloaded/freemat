#include "Type.hpp"
#include "Object.hpp"
#include "ListType.hpp"
#include "IntegerType.hpp"

using namespace FM;

Object Type::add(const Object &a, const Object &b)
{
  throw Exception("Add is unsupported for objects of type " + this->name());
}

Object Type::getParens(const Object &a, const Object &b) {
  throw Exception("() indexing is unsupported for objects of type " + this->name());
}

Object Type::getBraces(const Object &a, const Object &b) {
  throw Exception("{} indexing is unsupported for objects of type " + this->name());
}

Object Type::getField(const Object &a, const Object &b) {
  throw Exception(". indexing is unsupported for objects of type " + this->name());
}

Object Type::get(const Object &a, const Object &b) {
  Object c;
  int ptr = 0;
  const Object *bp = b.asType<ListType>()->readOnlyData(b);
  if ((b.elementCount() == 2) &&
      (bp[0].asType<Int32Type>()->scalarValue(bp[0]) == 0))
    return a.type()->getParens(a,bp[1]);
  c = a;
  while (ptr < b.elementCount())
    {
      int getType = bp[ptr].asType<Int32Type>()->scalarValue(bp[ptr]);
      switch (getType)
	{
	case 0:
	  c = c.type()->getParens(c,bp[ptr+1]);
	  break;
	case 1:
	  c = c.type()->getBraces(c,bp[ptr+1]);
	  break;
	case 2:
	  c = c.type()->getField(c,bp[ptr+1]);
	  break;
	}
      ptr += 2;
    }
  return c;
}

