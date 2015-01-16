#include "ClassType.hpp"
#include "AllTypes.hpp"
#include "Compiler.hpp"

using namespace FM;

ClassMetaData::ClassMetaData(ThreadContext *_ctxt) : m_name(_ctxt), m_defaults(_ctxt), m_events(_ctxt), m_constructor(_ctxt) {
  m_defaults = _ctxt->_list->empty();
  m_events = _ctxt->_list->empty();
  m_constructor = _ctxt->_function->empty();
}

Object ClassMetaType::getField(const Object &meta, const Object &fieldname) {
  const ClassMetaData *cmd = this->ro(meta);
  auto j = cmd->m_methods.find(fieldname);
  if (j == cmd->m_methods.end())
    throw Exception("Class " + cmd->m_name.description() + " has no method named " + fieldname.description());
  // if (!j->second->m_static)
  //   throw Exception("Method " + fieldname.description() + " is not static for class " + cmd->m_name.description());
  return j->second->m_definition;
}

Object ClassMetaType::getParens(const Object &meta, const Object &b) {
  const ClassMetaData *cmd = this->ro(meta);
  // Construct the object
  Object obj = this->construct(meta);
  return this->invokeConstructor(meta,obj,b);
}

Object ClassMetaType::invokeConstructor(const Object &meta, const Object &self, const Object &args) {
  const ClassMetaData *cmd = this->ro(meta);
  return _ctxt->_function->methodCall(cmd->m_constructor,self,args);  
}

Object ClassMetaType::construct(const Object &meta) {
  ClassData *cd = new ClassData(_ctxt);
  cd->metaClass = meta;
  cd->m_data = _ctxt->_list->makeScalar(this->ro(meta)->m_defaults);
  Data *q = new Data;
  q->refcnt = 0;
  q->ptr = cd;
  std::cout << "Constructing class with handle flag " << this->ro(meta)->m_ishandle << "\n";
  return Object(new ObjectBase(q,_ctxt->_class,0,Tuple(1,1),0,0,this->ro(meta)->m_ishandle)); // Capacity?
}

Object ClassMetaType::deref(const Object &meta) {
  const ClassMetaData *cmd = this->ro(meta);
  Object obj = this->construct(meta);
  Object args = _ctxt->_list->empty();
  return this->invokeConstructor(meta,obj,args);
}

FMString ClassMetaType::brief(const Object &a) {
  return "meta class for type " + _ctxt->_string->getString(this->ro(a)->m_name);
}

FMString ClassMetaType::describe(const Object &a) {
  const ClassMetaData *cmd = this->ro(a);
  FMString ret = "meta class " + _ctxt->_string->getString(cmd->m_name) + "\n";
  ret += "properties:\n";
  for (auto i=cmd->m_properties.begin(); i != cmd->m_properties.end(); ++i)
    ret += FMString(" ") + i->first.description() + "\n";
  return ret;
}

void ClassMetaType::addEvent(Object &meta, const Object &event) {
  ClassMetaData *cmd = this->rw(meta);
  _ctxt->_list->push(cmd->m_events,event);
}

void ClassMetaType::addProperty(Object &meta, const Object &name, bool constant,
				bool dependent, const Object &default_value,
				const Object &getter, const Object &setter) {
  ClassMetaData *cmd = this->rw(meta);
  ClassPropertyMetaData *cpmd = new ClassPropertyMetaData(_ctxt);
  cpmd->m_constant = constant;
  cpmd->m_dependent = dependent;
  cpmd->m_default = default_value;
  cpmd->m_getter = getter;
  cpmd->m_setter = setter;
  if (!dependent && !constant)
    {
      cpmd->m_index = cmd->m_defaults.count();
      _ctxt->_list->push(cmd->m_defaults,default_value);
    }
  else
    cpmd->m_index = -1;
  if (constant && (name == _ctxt->_string->makeString("_ishandle")))
    cmd->m_ishandle = true;
  cmd->m_properties[name] = cpmd;
}

void ClassMetaType::addSuperClass(Object &meta, const Object &super) {
  ClassMetaData *cmd = this->rw(meta);
  const ClassMetaData *smd = this->ro(super);
  for (auto i=smd->m_properties.begin(); i != smd->m_properties.end(); ++i)
    // TODO - warn if conflict on properties
    this->addProperty(meta,i->first,i->second->m_constant,
		      i->second->m_dependent, i->second->m_default,
		      i->second->m_getter, i->second->m_setter);
  for (auto i=smd->m_methods.begin(); i != smd->m_methods.end(); ++i)
    {
      // Does this method not exist?
      if (cmd->m_methods.find(i->first) == cmd->m_methods.end())
	this->addMethod(meta,i->first,i->second->m_definition,i->second->m_static);
    }
  // TODO - warn if method with same name as constructor already is defined
  this->addMethod(meta,smd->m_name,smd->m_constructor,false);
}

void ClassMetaType::addMethod(Object &meta, const Object &name, const Object &definition, bool is_static) {
  ClassMetaData *cmd = this->rw(meta);
  FMString s_name = _ctxt->_string->getString(name);
  ClassMethodMetaData *cmmd = new ClassMethodMetaData(_ctxt);
  cmmd->m_definition = definition;
  cmmd->m_static = is_static;
  cmd->m_methods.insert(std::pair<Object,ClassMethodMetaData*>(name,cmmd));
}

bool ClassType::hasMethod(const Object &a, const Object &name, Object &ret) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto j = cmd->m_methods.find(name);
  if (j != cmd->m_methods.end()) 
    {
      ret = j->second->m_definition;
      return true;
    }
  return false;
}

// Originally, I had wanted to make the avoidance of recursion for getters detected at compile time.
// I.e., if we have:
//    function x = get.foo(obj)
//        obj.foo = y
//    end
// In this case, we can see that the call to obj.foo can cause recursion and should be avoided.  But 
// what about this case:
//    function x = get.foo(obj)
//        t = obj
//        t.foo = y
//    end
// In this case, t is an alias for obj.  Infinite recursion will occur in this case.  But maybe that's OK.

Object ClassType::getFieldNoGetters(const Object &a, const Object &b) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto i = cmd->m_properties.find(b);
  if (i != cmd->m_properties.end()) 
    {
      ClassPropertyMetaData *cpmd = i->second;
      // This is copied from structtype...
      Object output = _ctxt->_list->makeMatrix(a.count(),1);
      Object *op = _ctxt->_list->rw(output);
      const Object *cp = _ctxt->_list->ro(cd->m_data);
      for (dim_t i=0;i<a.count();i++) {
	if (!cpmd->m_constant && !cpmd->m_dependent)
	  {
	    const Object *rp = _ctxt->_list->ro(cp[i]);
	    op[i] = rp[cpmd->m_index];
	  }
	else if (cpmd->m_constant)
	  op[i] = cpmd->m_default;
	else
	  throw Exception("Dependent property " + b.description() + " can only defined by getters.");
      }
      return output;
    }
  throw Exception("Property " + b.description() + " is not defined for class " + _ctxt->_string->getString(cmd->m_name));
}

Object ClassType::getMethod(const Object &a, const Object &b) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto j = cmd->m_methods.find(b);
  if (j == cmd->m_methods.end())
    throw Exception("Method " + b.description() + " is not defined for class " + _ctxt->_string->getString(cmd->m_name));
  return _ctxt->_bound->bindFunction(j->second->m_definition,a);
}

Object ClassType::getField(const Object &a, const Object &b) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto k = cmd->m_properties.find(b);
  if (k == cmd->m_properties.end()) 
    return this->getMethod(a,b);
  ClassPropertyMetaData *cpmd = k->second;
  if (cpmd->m_getter.isEmpty())
    return getFieldNoGetters(a,b);
  else
    return _ctxt->_function->call(cpmd->m_getter,_ctxt->_list->makeScalar(a),1);
}


void ClassType::setField(Object &a, const Object &args, const Object &b) {
  ClassData *cd = this->rw(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto k = cmd->m_properties.find(args);
  if (k == cmd->m_properties.end())
    throw Exception("Property " + b.description() + " is not defined for class " + _ctxt->_string->getString(cmd->m_name));
  ClassPropertyMetaData *cpmd = k->second;
  if (cpmd->m_setter.isEmpty())
    setFieldNoSetters(a,args,b);
  else
    {
      Object func_args = _ctxt->_list->empty();
      _ctxt->_list->push(func_args,a);
      _ctxt->_list->push(func_args,b);
      Object fret = _ctxt->_function->call(cpmd->m_setter,func_args,1);
      if (fret.count() != 1) 
	throw Exception("Setter " + cpmd->m_setter.description() + " failed to return a value!");
      a = _ctxt->_list->first(fret);
      return;
    }
}

void ClassType::setFieldNoSetters(Object &a, const Object &args, const Object &b) {
  ClassData *cd = this->rw(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto i = cmd->m_properties.find(args);
  if (i == cmd->m_properties.end()) throw Exception("Property " + args.description() + " is not defined for class " + cmd->m_name.description());
  ClassPropertyMetaData *cpmd = i->second;
  if (cpmd->m_constant)
    throw Exception("Property " + args.description() + " is constant and cannot be changed for class " + cmd->m_name.description());
  if (cpmd->m_dependent)
    throw Exception("Property " + args.description() + " is dependant and cannot be changed directly without a setter for class " + cmd->m_name.description());
  Object *rp = _ctxt->_list->rw(cd->m_data);
  dim_t num_assignments = std::max<dim_t>(1,a.count());
  const Object *bp = &b;
  if (b.isList())
    bp = _ctxt->_list->ro(b);
  if (b.isList() && (num_assignments > b.count()))
    throw Exception("Mismatch in number of left and right hand sides in expression a.property = b");
  for (dim_t n=0;n<num_assignments;n++)
    {
      Object *rd = _ctxt->_list->rw(rp[n]);
      rd[cpmd->m_index] = *bp;
      if (b.isList()) bp++;
    }
}

FMString ClassType::brief(const Object &a) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  return _ctxt->_string->getString(cmd->m_name);
}

ClassType::ClassType(ThreadContext *ctxt) : m_deletefunc(ctxt->_string->makeString("delete")) {
  _ctxt = ctxt;
}

void ClassType::destroyObject(ObjectBase* p) {
  ClassData *cd = static_cast<ClassData *>(p->ptr());
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto j = cmd->m_methods.find(m_deletefunc);
  if (j != cmd->m_methods.end()) {
    // Hmm... scary -- this is a pickle.  We need to destroy the object,
    // without triggering a recursion.  Unfortunately, the original object has
    // already been destroyed (or is in the process of being destroyed).  So
    // we need to construct a proxy object that will hold the reference to
    // the ObjectBase pointer, without itself becoming destroyed when this
    // function ends.  This bit of dark magic is done using the swap function
    // of the Object.  We create a sacrificial object, swap in our ObjectBase pointer,
    // and then, before the object gets destroyed, swap it back out.  Not necessarily
    // the cleanest solution, but it does work.
    Object sacrificial(_ctxt);
    ObjectBase *save = sacrificial.swap(p);
    _ctxt->_function->call(j->second->m_definition,_ctxt->_list->makeScalar(sacrificial),1);
    sacrificial.swap(save);
  }
  AggregateType::destroyObject(p);
}

FMString ClassType::describe(const Object &a) {
  // Get the meta class for this class
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  FMString ret = _ctxt->_string->getString(cmd->m_name);
  if (a.isScalar()) {
    ret += "\n";
    const Object *rd = _ctxt->_list->ro(_ctxt->_list->scalarValue(cd->m_data));
    ret += "properties:\n";
    for (auto i = cmd->m_properties.begin(); i != cmd->m_properties.end(); ++i)
      {
	ClassPropertyMetaData *cpmd = i->second;
	ret += "    " + _ctxt->_string->getString(i->first) + ": ";
	if (cpmd->m_constant)
	  {
	    ret += cpmd->m_default.brief() + " (constant)\n";
	  }
	else if (cpmd->m_dependent)
	  {
	    ret += _ctxt->_function->getParens(cpmd->m_getter,_ctxt->_list->makeScalar(a)).brief() + " (dependent)\n";
	  }
	else
	  {
	    ret += rd[cpmd->m_index].brief() + "\n";
	  }
      }
  } else {
    ret += " " + a.dims().toString() + " array\n";
    ret += "properties:\n";
    for (auto i = cmd->m_properties.begin(); i != cmd->m_properties.end(); ++i)
      ret += "    " + _ctxt->_string->getString(i->first) + "\n";
  }
  ret += "list of events\n";
  const Object *op = _ctxt->_list->ro(cmd->m_events);
  for (int i=0;i<cmd->m_events.count();i++)
    ret += "   " + op[i].description() + "\n";
  ret += "list of methods\n";
  for (auto i=cmd->m_methods.begin(); i != cmd->m_methods.end(); ++i)
    {
      ret += "    " + _ctxt->_string->getString(i->first);
      if (i->second->m_static)
	ret += " (static)";
      ret += "\n";
    }
  ret += "refcount = " + Stringify(a.refcnt()) + "\n";
  return ret;
}
