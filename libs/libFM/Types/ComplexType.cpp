#include "ComplexType.hpp"

DataCode ComplexProxyType::code() const {
  return _ctxt->_double->code();
}

const ComplexProxyType::FMString& name() const {
  return _ctxt->_double->name();
}

void ComplexProxyType::destroyObject(ObjectBase* p) {
  _ctxt->_double->destroyObject(p);
}

Data* ComplexProxyType::duplicateData(const ObjectBase * p, ndx_t &reserve) {
  return _ctxt->_double->duplicateData(p,reserve);
}

void ComplexProxyType::visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {
  return _ctxt->_double->visitContainedObjects(p,visitor);
}

Object ComplexProxyType::empty() {
  return _ctxt->_double->empty();
}

Object ComplexProxyType::import(const Object &foreign) {
  return _ctxt->_double->import(foreign);
}

Object ComplexProxyType::Add(const Object &a, const Object &b) {
  return _ctxt->_double->Add(a,b);
}

Object ComplexProxyType::Multiply(const Object &a, const Object &b) {
  return _ctxt->_double->Multiply(a,b);
}

Object ComplexProxyType::Power(const Object &a, const Object &b) {
  return _ctxt->_double->Power(a,b);
}

Object ComplexProxyType::LeftDivide(const Object &a, const Object &b) {
  return _ctxt->_double->LeftDivide(a,b);
}

Object ComplexProxyType::RightDivide(const Object &a, const Object &b) {
  return _ctxt->_double->RightDivide(a,b);
}

Object ComplexProxyType::DotLeftDivide(const Object &a, const Object &b) {
  return _ctxt->_double->DotLeftDivide(a,b);
}

Object ComplexProxyType::DotRightDivide(const Object &a, const Object &b) {
  return _ctxt->_double->DotRightDivide(a,b);
}

Object ComplexProxyType::DotMultiply(const Object &a, const Object &b) {
  return _ctxt->_double->DotMultiply(a,b);
}

Object ComplexProxyType::DotPower(const Object &a, const Object &b) {
  return _ctxt->_double->DotPower(a,b);
}

Object ComplexProxyType::Subtract(const Object &a, const Object &b) {
  return _ctxt->_double->Subtract(a,b);
}

Object ComplexProxyType::LessEquals(const Object &a, const Object &b) {
  return _ctxt->_double->LessEquals(a,b);
}

Object ComplexProxyType::LessThan(const Object &a, const Object &b) {
  return _ctxt->_double->LessThan(a,b);
}

Object ComplexProxyType::GreaterThan(const Object &a, const Object &b) {
  return _ctxt->_double->GreaterThan(a,b);
}

Object ComplexProxyType::GreaterEquals(const Object &a, const Object &b) {
  return _ctxt->_double->GreaterEquals(a,b);
}

Object ComplexProxyType::NotEquals(const Object &a, const Object &b) {
  return _ctxt->_double->NotEquals(a,b);
}

Object ComplexProxyType::Equals(const Object &a, const Object &b) {
  return _ctxt->_double->Equals(a,b);
}

Object ComplexProxyType::Or(const Object &a, const Object &b) {
  return _ctxt->_double->Or(a,b);
}

Object ComplexProxyType::And(const Object &a, const Object &b) {
  return _ctxt->_double->And(a,b);
}

Object ComplexProxyType::Neg(const Object &a) {
  return _ctxt->_double->Neg(a);
}

Object ComplexProxyType::Not(const Object &a) {
  return _ctxt->_double->Not(a);
}

Object ComplexProxyType::Plus(const Object &a) {
  return _ctxt->_double->Plus(a);
}

Object ComplexProxyType::Colon(const Object &a, const Object &b) {
  return _ctxt->_double->Colon(a,b);
}

Object ComplexProxyType::NCat(const Object &p, ndx_t dimension) {
  return _ctxt->_double->NCat(a,dimension);
}

Object ComplexProxyType::DoubleColon(const Object &a, const Object &b, const Object &c) {
  return _ctxt->_double->DoubleColon(a,b,c);
}

Object ComplexProxyType::Transpose(const Object &a) {
  return _ctxt->_double->Transpose(a);
}

Object ComplexProxyType::Hermitian(const Object &a) {
  return _ctxt->_double->Hermitian(a);
}

bool ComplexProxyType::equals(const Object &a, const Object &b) {
  return _ctxt->_double->equals(a,b);
}

FMString ComplexProxyType::describe(const Object &a) {
  return _ctxt->_double->describe(a);
}

FMString ComplexProxyType::brief(const Object &a) {
  return _ctxt->_double->brief(a);
}

Object ComplexProxyType::asIndex(const Object &a, ndx_t max) {
  return _ctxt->_double->asIndex(a,max);
}

Object ComplexProxyType::asIndexNoBoundsCheck(const Object &a) {
  return _ctxt->_double->asIndexNoBoundsCheck(a);
}

Object ComplexProxyType::asLogical(const Object &a) {
  return _ctxt->_double->asLogical(a);
}

Object ComplexProxyType::getParens(const Object &a, const Object &b) {
  return _ctxt->_double->getParens(a,b);
}

Object ComplexProxyType::getBraces(const Object &a, const Object &b) {
  return _ctxt->_double->getBraces(a,b);
}

Object ComplexProxyType::getField(const Object &a, const Object &b) {
  return _ctxt->_double->getField(a,b);
}

Object ComplexProxyType::getFieldNoGetters(const Object &a, const Object &b) {
  return _ctxt->_double->getFieldNoGetters(a,b);
}

Object ComplexProxyType::sliceColumn(const Object &a, ndx_t column) {
  return _ctxt->_double->sliceColumn(a,column);
}

Object ComplexProxyType::convert(const Object &a) {
  return _ctxt->_double->convert(a);
}

double ComplexProxyType::doubleValue(const Object &a) {
  return _ctxt->_double->doubleValue(a);
}

void ComplexProxyType::setParens(Object &a, const Object &args, const Object &b) {
  return _ctxt->_double->setParens(a,args,b);
}

void ComplexProxyType::setBraces(Object &a, const Object &args, const Object &b) {
  return _ctxt->_double->setBraces(a,args,b);
}

void ComplexProxyType::setField(Object &a, const Object &args, const Object &b) {
  return _ctxt->_double->setField(a,args,b);
}

void ComplexProxyType::setFieldNoSetters(Object &a, const Object &args, const Object &b) {
  return _ctxt->_double->setFieldNoSetters(a,args,b);
}

Object ComplexProxyType::get(const Object &a, const Object &b, bool invokeGetters) {
  return _ctxt->_double->get(a,b,invokeGetters);
}

void ComplexProxyType::set(Object &a, const Object &args, const Object &b, bool invokeSetters) {
  return _ctxt->_double->set(a,args,b,invokeSetters);
}

void ComplexProxyType::resize(Object &a, const Tuple &newsize) {
  return _ctxt->_double->resize(a,newsize);
}

void ComplexProxyType::print(const Object &a) {
  return _ctxt->_double->print(a);
}

Object ComplexProxyType::deref(const Object &a) {
  return _ctxt->_double->deref(a);
}

Object ComplexProxyType::call(const Object &a, const Object &args, ndx_t nargout) {
  return _ctxt->_double->call(a,args,nargout);
}

