#include "ClassType.hpp"

using namespace FM;

Object ClassMetaType::construct(const Object &meta) {
  ClassData *cd = new ClassData(_ctxt);
  cd->metaClass = meta;
  // Get the property count
  int num_props = 
}

FMString ClassMetaType::describe(const Object &a) {
  return "meta class for type " + this->readOnlyData(a)->m_name + "\n";
}

FMString ClassType::describe(const Object &a) {
  // Get the meta class for this class
  const Object &myMeta = this->readOnlyData(a)->metaClass;
  return "class of type " + _ctxt->_meta->readOnlyData(myMeta)->m_name + "\n";
}
