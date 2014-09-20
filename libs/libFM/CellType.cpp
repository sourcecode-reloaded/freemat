#include "CellType.hpp"
#include "ThreadContext.hpp"
#include "TermIF.hpp"
#include "TypeUtils.hpp"

using namespace FM;

void CellType::printElement(const Object &a, const ArrayFormatInfo &info, ndx_t ndx) {
  const Object *t = this->readOnlyData(a);
  _ctxt->_io->output("[" + t[ndx].description() + "]");
}

Object CellType::getBraces(const Object &a, const Object &args) {
  return makeListFromCell(_ctxt,getParens(a,args));
}

void CellType::setBraces(Object &a, const Object &args, const Object &b) {
  // A list for the RHS indicates multiple assignment
  if (!b.isList())
    setParens(a,args,this->makeScalar(b));
  else
    setParens(a,args,makeCellFromList(_ctxt,b)); // Need relief on oversizing?
}

