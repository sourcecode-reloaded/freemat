#include "Type.hpp"
#include "Object.hpp"
#include "ListType.hpp"
#include "IntegerType.hpp"
#include "CellType.hpp"
#include "StructType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

Object Type::call(const Object &a, const Object &args, int nargout)
{
  throw Exception("call is unsupported for objects of type " + this->name());
}

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

Object Type::NCat(const Object &a, int dimension)
{
  throw Exception("object of type " + this->name() + " does not support vcat/hcat");
}

#define NoSupportBinOp(x) \
  Object Type::x(const Object &a, const Object &b) {throw Exception(#x " is unsupported for objects of type " + this->name());}

#define NoSupportUnaryOp(x) \
  Object Type::x(const Object &a) {throw Exception(#x " is unsupported for objects of type " + this->name());}

NoSupportBinOp(LessEquals);
NoSupportBinOp(Add);
NoSupportBinOp(LessThan);
NoSupportBinOp(DotMultiply);
NoSupportBinOp(Multiply);
NoSupportBinOp(DotLeftDivide);
NoSupportBinOp(DotRightDivide);
NoSupportBinOp(LeftDivide);
NoSupportBinOp(RightDivide);
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
NoSupportUnaryOp(Transpose);
NoSupportUnaryOp(Hermitian);

Object Type::convert(const Object &a) {
  throw Exception("Cannot convert objects to type " + this->name());
}

Object Type::DoubleColon(const Object &a, const Object &b, const Object &c)
{
  throw Exception("Colon(a,b,c) is unsupported for objects of type " + this->name());
}

void Type::setParens(Object &a, const Object &args, const Object &b) {
  throw Exception("() assignment is unsupported for objects of type " + this->name());
}

void Type::setBraces(Object &a, const Object &args, const Object &b) {
  throw Exception("{} assignment is unsupported for objects of type " + this->name());
}

void Type::setField(Object &a, const Object &args, const Object &b) {
  throw Exception(". (field) assignment is unsupported for objects of type " + this->name());
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

void Type::print(const Object &a) {
  _ctxt->_io->output(a.description());
}

double Type::doubleValue(const Object &a) {
  throw Exception("Type " + this->name() + " cannot be converted to double scalar");
}

Object Type::get(const Object &a, const Object &b) {
  // std::cout << "get arguments: " << a.description() << "\n";
  // std::cout << "   b: " << b.description() << "\n";
  int ptr = 0;
  const Object *bp = b.asType<ListType>()->readOnlyData(b);
  if ((b.elementCount() == 2) &&
      (bp[0].asDouble() == 0))
    return b.asType<ListType>()->makeScalar(a.type()->getParens(a,bp[1]));
  Object c = a;
  while (ptr < b.elementCount())
    {
      double getType = bp[ptr].asDouble();
      switch (int(getType))
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
      double setType = argp[0].asDouble();
      switch (int(setType))
	{
	case 0:
	  // TODO - not sure about the case:
	  // a = int8([])  - typed empty
	  // a(3) = 5      - what type is it now?
	  if (a.isEmpty()) a = b.type()->empty();
	  a.type()->setParens(a,argp[1],b);
	  break;
	case 1:
	  if (a.isEmpty() && (a.type()->code() != TypeCellArray))
	    a = _ctxt->_cell->zeroArrayOfSize(a.dims(),false);
	  a.type()->setBraces(a,argp[1],b);
	  break;
	case 2:
	  if (a.isEmpty() && (a.type()->code() != TypeStruct))
	    a = _ctxt->_struct->empty();
	  a.type()->setField(a,argp[1],b);
	  break;
	}
    }
  else
    {
      dim_t arg_count = args.elementCount();
      Object args_rest = args;
      Object args_first = _ctxt->_list->empty();
      _ctxt->_list->push(args_first,_ctxt->_list->first(args_rest));
      _ctxt->_list->pop(args_rest);
      _ctxt->_list->push(args_first,_ctxt->_list->first(args_rest));
      _ctxt->_list->pop(args_rest);
      // TODO - is an exception here bad? If so, remove it
      Object asub(_ctxt);
      try {
	asub = a.type()->get(a,args_first);
      } catch (Exception &e) {
	asub = _ctxt->_list->makeScalar(a.type()->empty());
      }
	if (!asub.isScalar()) 
	  throw Exception("In complex indexing expressions (e.g., A(...).foo = x), the sub expressions (such as A(...)) must be single valued");
      asub = _ctxt->_list->first(asub);
      set(asub,args_rest,b);
      set(a,args_first,asub);
    }
}

Object Type::deref(const Object &a) {
  return a;
}
