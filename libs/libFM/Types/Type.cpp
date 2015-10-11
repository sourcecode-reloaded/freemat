#include "Type.hpp"
#include "Object.hpp"
#include "ListType.hpp"
#include "ClassType.hpp"
#include "IntegerType.hpp"
#include "CellType.hpp"
#include "StructType.hpp"
#include "ThreadContext.hpp"

using namespace FM;

Object Type::call(const Object &, const Object &, ndx_t )
{
  throw Exception("call is unsupported for objects of type " + this->name());
}

Object Type::asLogical(const Object &)
{
  throw Exception("asLogical(a) is unsupported for objects of type " + this->name());
}

Object Type::asIndex(const Object &, ndx_t)
{
  throw Exception("object of type " + this->name() + " cannot be used as an index");
}

Object Type::asIndexNoBoundsCheck(const Object &)
{
  throw Exception("object of type " + this->name() + " cannot be used as an index");  
}

Object Type::NCat(const Object &, ndx_t)
{
  throw Exception("object of type " + this->name() + " does not support vcat/hcat");
}

#define NoSupportBinOp(x) \
  Object Type::x(const Object &, const Object &) {throw Exception(#x " is unsupported for objects of type " + this->name());}

#define NoSupportUnaryOp(x) \
  Object Type::x(const Object &) {throw Exception(#x " is unsupported for objects of type " + this->name());}

NoSupportBinOp(LessEquals);
NoSupportBinOp(Add);
NoSupportBinOp(LessThan);
NoSupportBinOp(DotMultiply);
NoSupportBinOp(Multiply);
NoSupportBinOp(Power);
NoSupportBinOp(DotPower);
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
NoSupportUnaryOp(Not);
NoSupportUnaryOp(Neg);
NoSupportUnaryOp(Plus);
NoSupportUnaryOp(Transpose);
NoSupportUnaryOp(Hermitian);

Object Type::sliceColumn(const Object &, ndx_t) {
  throw Exception("Cannot use objects of type " + this->name() + " in expressions of the type for i=<object>");
}

Object Type::convert(const Object &) {
  throw Exception("Cannot convert objects to type " + this->name());
}

Object Type::DoubleColon(const Object &, const Object &, const Object &)
{
  throw Exception("Colon(a,b,c) is unsupported for objects of type " + this->name());
}

void Type::setParens(Object &, const Object &, const Object &) {
  throw Exception("() assignment is unsupported for objects of type " + this->name());
}

void Type::setBraces(Object &, const Object &, const Object &) {
  throw Exception("{} assignment is unsupported for objects of type " + this->name());
}

void Type::setField(Object &, const Object &, const Object &) {
  throw Exception(". (field) assignment is unsupported for objects of type " + this->name());
}

void Type::setFieldNoSetters(Object &, const Object &, const Object &) {
  throw Exception(". (field) assignment (no setters called) is unsupported for objects of type " + this->name());
}

Object Type::getParens(const Object &, const Object &) {
  throw Exception("() indexing is unsupported for objects of type " + this->name());
}

Object Type::getBraces(const Object &, const Object &) {
  throw Exception("{} indexing is unsupported for objects of type " + this->name());
}

Object Type::getField(const Object &, const Object &) {
  throw Exception(". indexing is unsupported for objects of type " + this->name());
}

Object Type::getFieldNoGetters(const Object &, const Object &) {
  throw Exception(". indexing (no getter calls) is unsupported for objects of type " + this->name());
}

void Type::resize(Object &, const Tuple &) {
  throw Exception("resize is unsupported for objects of type " + this->name());
}

void Type::print(const Object &a) {
  _ctxt->_io->output(a.description());
}

double Type::doubleValue(const Object &) {
  throw Exception("Type " + this->name() + " cannot be converted to double scalar");
}

Object Type::get(const Object &a, const Object &b, bool invokeGetters) {
  //  std::cout << "get arguments: " << a.description() << "\n";
  //  std::cout << "   b: " << b.description() << "\n";
  ndx_t ptr = 0;
  // Allow overload of get using "subsref" method
  if (a.isClass() && _ctxt->_class->hasSubsref(a) && invokeGetters)
      return _ctxt->_class->subsref(a,b);
  const Object *bp = b.asType<ListType>()->ro(b);
  if ((b.count() == 2) &&
      (bp[0].asDouble() == 0))
    return b.asType<ListType>()->makeScalar(a.type()->getParens(a,bp[1]));
  Object c = a;
  while (ptr < b.count())
    {
      double getType = bp[ptr].asDouble();
      if (c.isList()) {
	if (c.count() > 1) throw Exception("Cannot apply indexing to multi-valued expressions (e.g. a.foo.bar means a.foo must be scalar valued)");
	c = c.asType<ListType>()->first(c); //TODO a.foo.goo ? should throw error if a.foo is multi-valued
      }
      switch (int(getType))
	{
	case 0:
	  c = c.type()->getParens(c,bp[ptr+1]);
	  break;
	case 1:
	  c = c.type()->getBraces(c,bp[ptr+1]);
	  break;
	case 2:
	  if (invokeGetters)
	    c = c.type()->getField(c,bp[ptr+1]);
	  else
	    c = c.type()->getFieldNoGetters(c,bp[ptr+1]);
	  break;
	}
      ptr += 2;
    }
  if (c.isList()) return c;
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


void Type::set(Object &a, const Object &args, const Object &b, bool invokeSetters) {
  if (a.isClass() && _ctxt->_class->hasSubsasgn(a) && invokeSetters)
    {
      _ctxt->_class->subsasgn(a,args,b);
      return;
    }
  if (b.type()->code() != a.type()->code()) {
    if (b.type()->isComplexType() && !a.type()->isComplexType()) {
      a = ThreadContext::GetComplexTypeForCode(_ctxt,a.type()->code())->convert(a);
      return set(a,args,b,invokeSetters);
    }
    Object bconv = a.type()->convert(b);
    return set(a,args,bconv,invokeSetters);
  }
  const Object *argp = args.asType<ListType>()->ro(args);
  if (args.count() == 2)
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
	    a = _ctxt->_cell->zeroArrayOfSize(a.dims());
	  a.type()->setBraces(a,argp[1],b);
	  break;
	case 2:
	  if (a.isEmpty() && (a.type()->code() != TypeStruct))
	    a = _ctxt->_struct->empty();
	  if (invokeSetters)
	    a.type()->setField(a,argp[1],b);
	  else
	    a.type()->setFieldNoSetters(a,argp[1],b);
	  break;
	}
    }
  else
    {
      Object args_rest = args;
      Object args_first = _ctxt->_list->empty();
      _ctxt->_list->push(args_first,_ctxt->_list->first(args_rest));
      args_rest = _ctxt->_list->pop(args_rest);
      _ctxt->_list->push(args_first,_ctxt->_list->first(args_rest));
      args_rest = _ctxt->_list->pop(args_rest);
      // TODO - is an exception here bad? If so, remove it
      Object asub(_ctxt);
      try {
	asub = a.type()->get(a,args_first,true);
      } catch (Exception &) {
	asub = _ctxt->_list->makeScalar(a.type()->empty());
      }
	if (!asub.isScalar()) 
	  throw Exception("In complex indexing expressions (e.g., A(...).foo = x), the sub expressions (such as A(...)) must be single valued");
      asub = _ctxt->_list->first(asub);
      set(asub,args_rest,b,invokeSetters);
      set(a,args_first,asub,invokeSetters);
    }
}

Object Type::deref(const Object &a) {
  return a;
}

