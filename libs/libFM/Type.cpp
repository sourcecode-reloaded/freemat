#include "Type.hpp"
#include "Object.hpp"
#include "ListType.hpp"
#include "IntegerType.hpp"
#include "BaseTypes.hpp"

using namespace FM;

Object Type::asLogical(const Object &a)
{
  throw Exception("asLogical(a) is unsupported for objects of type " + this->name());
}

Object Type::asIndex(const Object &a, dim_t max)
{
  throw Exception("object of type " + this->name() + " cannot be used as an index");
}

Object Type::asIndexNoBoundsCheck(const Object &a)
{
  throw Exception("object of type " + this->name() + " cannot be used as an index");  
}

#define NoSupportBinOp(x) \
  Object Type::x(const Object &a, const Object &b) {throw Exception(#x " is unsupported for objects of type " + this->name());}

#define NoSupportUnaryOp(x) \
  Object Type::x(const Object &a) {throw Exception(#x " is unsupported for objects of type " + this->name());}

NoSupportBinOp(LessEquals);
NoSupportBinOp(Add);
NoSupportBinOp(LessThan);
NoSupportBinOp(DotMultiply);
NoSupportBinOp(Subtract);
NoSupportBinOp(Colon);
NoSupportBinOp(GreaterThan);
NoSupportBinOp(GreaterEquals);
NoSupportBinOp(Equals);
NoSupportBinOp(NotEquals);
NoSupportBinOp(Or);
NoSupportBinOp(And);
NoSupportUnaryOp(Neg);
NoSupportUnaryOp(Plus);

Object Type::DoubleColon(const Object &a, const Object &b, const Object &c)
{
  throw Exception("Colon(a,b,c) is unsupported for objects of type " + this->name());
}

void Type::setParens(Object &a, const Object &args, const Object &b) {
  throw Exception("() indexing is unsupported for objects of type " + this->name());
}

void Type::setBraces(Object &a, const Object &args, const Object &b) {
  throw Exception("{} indexing is unsupported for objects of type " + this->name());
}

void Type::setField(Object &a, const Object &args, const Object &b) {
  throw Exception(". indexing is unsupported for objects of type " + this->name());
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

void Type::resize(Object &a, const Tuple &newsize) {
  throw Exception("resize is unsupported for objects of type " + this->name());
}

void Type::print(const Object &a, TermIF &io) {
  io.output(a.description());
}

double Type::doubleValue(const Object &a) {
  throw Exception("Type cannot be converted to double scalar");
}

Object Type::get(const Object &a, const Object &b) {
  Object c;
  int ptr = 0;
  const Object *bp = b.asType<ListType>()->readOnlyData(b);
  if ((b.elementCount() == 2) &&
      (bp[0].asType<Int32Type>()->scalarValue(bp[0]) == 0))
    return b.asType<ListType>()->makeScalar(a.type()->getParens(a,bp[1]));
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
  return b.asType<ListType>()->makeScalar(c);
}

// The usual unwinding algorithm
//
// A(1,3).foo = 7
//
// set(A,(1,3), set(get(A,(1,3)),.foo,7))
//
// if args.elementcount == 2, do a straightforward set
//
// else
//
//  asub = get(A,args.first()) || empty
//
//  set(asub,args.but_first(),b)
//  set(A,args.first(),asub)
//


void Type::set(Object &a, const Object &args, const Object &b) {
  const Object *argp = args.asType<ListType>()->readOnlyData(args);
  if (args.elementCount() == 2)
    {
      int setType = argp[0].asType<Int32Type>()->scalarValue(argp[0]);
      switch (setType)
	{
	case 0:
	  a.type()->setParens(a,argp[1],b);
	  break;
	case 1:
	  if (a.isEmpty() && (a.type()->code() != TypeCellArray))
	    a = a.type()->_base->_cell->zeroArrayOfSize(a.dims(),false);
	  a.type()->setBraces(a,argp[1],b);
	  break;
	case 2:
	  if (a.isEmpty() && (a.type()->code() != TypeStruct))
	    {
	      // FIXME	      a = a.type()->_base->_struct->makeStruct(Tuple(1,1),FMStringList(argp[1]
	    }
	  a.type()->setField(a,argp[1],b);
	  break;
	}
    }
  else
    {
      dim_t arg_count = args.elementCount();
      Object args_first = args.asType<ListType>()->makeMatrix(2,1);
      Object args_rest = args.asType<ListType>()->makeMatrix(arg_count-2,1);
      Object asub = a.type()->get(a,args_first);
      set(asub,args_rest,b);
      set(a,args_first,asub);
    }
}
