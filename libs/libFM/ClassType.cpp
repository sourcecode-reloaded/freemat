#include "ClassType.hpp"
#include "AllTypes.hpp"

using namespace FM;

ClassMetaData::ClassMetaData(ThreadContext *_ctxt) : m_defaults(_ctxt->_list->empty()) {}

Object ClassMetaType::construct(const Object &meta) {
  ClassData *cd = new ClassData(_ctxt);
  cd->metaClass = meta;
  cd->m_data = _ctxt->_list->makeScalar(this->readOnlyData(meta)->m_defaults);
  Data *q = new Data;
  q->refcnt = 0;
  q->ptr = cd;
  return Object(new ObjectBase(q,_ctxt->_class,0,Tuple(1,1),0,0)); // Capacity?
}

FMString ClassMetaType::describe(const Object &a) {
  return "meta class for type " + this->readOnlyData(a)->m_name + "\n";
}

void ClassMetaType::addProperty(Object &meta, const Object &name, const Object &default_value) {
  int next = this->readWriteData(meta)->m_properties.size();
  ClassMetaData *cmd = this->readWriteData(meta);
  cmd->m_properties[name] = next;
  _ctxt->_list->push(cmd->m_defaults,default_value);
}

FMString ClassType::describe(const Object &a) {
  // Get the meta class for this class
  const ClassData *cd = this->readOnlyData(a);
  const Object &myMeta = cd->metaClass;
  return "class of type " + _ctxt->_meta->readOnlyData(myMeta)->m_name + ": " + cd->m_data.description() + "\n";
}
