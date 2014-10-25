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

void ClassMetaType::addMethod(Object &meta, const Object &name, const Object &definition) {
  ClassMetaData *cmd = this->readWriteData(meta);
  cmd->m_methods.insert(std::pair<Object,Object>(name,definition));
}

bool ClassType::hasMethod(const Object &a, const Object &name, Object &ret) {
  const ClassData *cd = this->readOnlyData(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->readOnlyData(myMeta);
  auto j = cmd->m_methods.find(name);
  if (j != cmd->m_methods.end()) 
    {
      ret = j->second;
      return true;
    }
  return false;
}

Object ClassType::getField(const Object &a, const Object &b) {
  const ClassData *cd = this->readOnlyData(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->readOnlyData(myMeta);
  auto i = cmd->m_properties.find(b);
  if (i != cmd->m_properties.end()) 
    {
      int ndx = i->second;
      // This is copied from structtype...
      Object output = _ctxt->_list->makeMatrix(a.elementCount(),1);
      Object *op = _ctxt->_list->readWriteData(output);
      const Object *cp = _ctxt->_cell->readOnlyData(cd->m_data);
      for (dim_t i=0;i<a.elementCount();i++) {
	const Object *rp = _ctxt->_list->readOnlyData(cp[i]);
	op[i] = rp[ndx];
      }
      return output;
    }
  auto j = cmd->m_methods.find(b);
  if (j != cmd->m_methods.end()) return j->second; // FIXME - should bind function to the argument, i.e., func(obj,a,b) --> func_obj(a,b)
  throw Exception("Property " + b.description() + " is not defined for class " + cmd->m_name);
}

void ClassType::setField(Object &a, const Object &args, const Object &b) {
  ClassData *cd = this->readWriteData(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->readOnlyData(myMeta);
  auto i = cmd->m_properties.find(args);
  if (i == cmd->m_properties.end()) throw Exception("Property " + args.description() + " is not defined for class " + cmd->m_name);
  int ndx = i->second;
  Object *rp = _ctxt->_cell->readWriteData(cd->m_data);
  dim_t num_assignments = std::max<dim_t>(1,a.elementCount());
  const Object *bp = &b;
  if (b.isList())
    bp = _ctxt->_list->readOnlyData(b);
  if (b.isList() && (num_assignments > b.elementCount()))
    throw Exception("Mismatch in number of left and right hand sides in expression a.property = b");
  for (dim_t n=0;n<num_assignments;n++)
    {
      Object *rd = _ctxt->_list->readWriteData(rp[n]);
      rd[ndx] = *bp;
      if (b.isList()) bp++;
    }
}

FMString ClassType::describe(const Object &a) {
  // Get the meta class for this class
  const ClassData *cd = this->readOnlyData(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->readOnlyData(myMeta);
  FMString ret = cmd->m_name;
  if (a.isScalar()) {
    ret += "\n";
    const Object *rd = _ctxt->_list->readOnlyData(_ctxt->_cell->scalarValue(cd->m_data));
    ret += "properties:\n";
    for (auto i = cmd->m_properties.begin(); i != cmd->m_properties.end(); ++i)
      {
	ret += "    " + _ctxt->_string->getString(i->first) + ": ";
	ret += rd[i->second].description() + "\n";
      }
  } else {
    ret += " " + a.dims().toString() + " array\n";
    ret += "properties:\n";
    for (auto i = cmd->m_properties.begin(); i != cmd->m_properties.end(); ++i)
      ret += "    " + _ctxt->_string->getString(i->first) + "\n";
  }
  ret += "list of methods\n";
  for (auto i=cmd->m_methods.begin(); i != cmd->m_methods.end(); ++i)
    ret += "    " + _ctxt->_string->getString(i->first) + "\n";
  return ret;
}
