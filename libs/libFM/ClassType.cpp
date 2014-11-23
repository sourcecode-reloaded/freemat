#include "ClassType.hpp"
#include "AllTypes.hpp"
#include "Compiler.hpp"

using namespace FM;

ClassMetaData::ClassMetaData(ThreadContext *_ctxt) : m_name(_ctxt), m_defaults(_ctxt) {
  m_defaults = _ctxt->_list->empty();
}

Object ClassMetaType::getParens(const Object &meta, const Object &b) {
  const ClassMetaData *cmd = this->ro(meta);
  auto j = cmd->m_methods.find(cmd->m_name);
  if (j == cmd->m_methods.end())
    {
      // No constructor - just use default
      throw Exception("Cannot invoke constructor for class " + cmd->m_name.description() + " with arguments - only the default constructor is available.");
    }
  else
    {
      // There is a constructor - just invoke it (no arguments)
      // the constructor will be responsible for calling OP_CONSTRUCT to construct the class.
      std::cout << "Constructor found - invoking\n";
      return _ctxt->_function->getParens(j->second,b);
    }
}

Object ClassMetaType::construct(const Object &meta) {
  ClassData *cd = new ClassData(_ctxt);
  cd->metaClass = meta;
  cd->m_data = _ctxt->_list->makeScalar(this->ro(meta)->m_defaults);
  Data *q = new Data;
  q->refcnt = 0;
  q->ptr = cd;
  return Object(new ObjectBase(q,_ctxt->_class,0,Tuple(1,1),0,0)); // Capacity?
}

// FIXME - should have separate set of static methods... for now, 
// assume that constructor must be static by definition
Object ClassMetaType::deref(const Object &meta) {
  const ClassMetaData *cmd = this->ro(meta);
  auto j = cmd->m_methods.find(cmd->m_name);
  if (j == cmd->m_methods.end())
    {
      // No constructor - just use default
      std::cout << "No constructor found - using default constructor for " << cmd->m_name << "\n";
      return this->construct(meta);
    }
  else
    {
      // There is a constructor - just invoke it (no arguments)
      // the constructor will be responsible for calling OP_CONSTRUCT to construct the class.
      std::cout << "Constructor found - invoking\n";
      return _ctxt->_function->deref(j->second);
    }
}

FMString ClassMetaType::brief(const Object &a) {
  return "meta class for type " + _ctxt->_string->getString(this->ro(a)->m_name);
}

FMString ClassMetaType::describe(const Object &a) {
  const ClassMetaData *cmd = this->ro(a);
  FMString ret = "meta class " + _ctxt->_string->getString(cmd->m_name) + "\n";
  ret += "properties:\n";
  for (auto i=cmd->m_props.begin(); i != cmd->m_props.end(); ++i)
    ret += FMString(" ") + i->first.description() + "\n";
  return ret;
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
  cmd->m_props[name] = cpmd;
}

Object ClassMetaType::getField(const Object &meta, const Object &fieldname) {
  const ClassMetaData *cmd = this->ro(meta);
  auto i = cmd->m_props.find(fieldname);
  if (i == cmd->m_props.end())
    throw Exception("Property " + fieldname.description() + " not defined for class " + cmd->m_name.description());
  if (!i->second->m_constant)
    throw Exception("Property " + fieldname.description() + " is not constant for class " + cmd->m_name.description());
  return i->second->m_default;
}

void ClassMetaType::addMethod(Object &meta, const Object &name, const Object &definition) {
  ClassMetaData *cmd = this->rw(meta);
  FMString s_name = _ctxt->_string->getString(name);
  // if (s_name.startsWith("get."))
  //   cmd->m_getters.insert(std::pair<Object,Object>(_ctxt->_string->makeString(s_name.mid(4)),definition)); // TODO - Check that get.foo is a valid property name
  // else if (s_name.startsWith("set."))
  //   cmd->m_setters.insert(std::pair<Object,Object>(_ctxt->_string->makeString(s_name.mid(4)),definition)); // TODO - Check that set.foo is a valid property name
  // else
  cmd->m_methods.insert(std::pair<Object,Object>(name,definition));
}

bool ClassType::hasMethod(const Object &a, const Object &name, Object &ret) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto j = cmd->m_methods.find(name);
  if (j != cmd->m_methods.end()) 
    {
      ret = j->second;
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
  auto i = cmd->m_props.find(b);
  if (i != cmd->m_props.end()) 
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

Object ClassType::getField(const Object &a, const Object &b) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto k = cmd->m_props.find(b);
  if (k == cmd->m_props.end()) {
    auto j = cmd->m_methods.find(b);
    if (j == cmd->m_methods.end())
      throw Exception("Property " + b.description() + " is not defined for class " + _ctxt->_string->getString(cmd->m_name));
    return _ctxt->_bound->bindFunction(j->second,a);
  }
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
  auto k = cmd->m_props.find(args);
  if (k == cmd->m_props.end())
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
  auto i = cmd->m_props.find(args);
  if (i == cmd->m_props.end()) throw Exception("Property " + args.description() + " is not defined for class " + cmd->m_name.description());
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
    for (auto i = cmd->m_props.begin(); i != cmd->m_props.end(); ++i)
      {
	ClassPropertyMetaData *cpmd = i->second;
	ret += "    " + _ctxt->_string->getString(i->first) + ": ";
	if (cpmd->m_constant)
	  {
	    ret += cpmd->m_default.brief() + " (constant)\n";
	  }
	else if (cpmd->m_dependent)
	  {
	    ret += " (dependent)\n";
	  }
	else
	  {
	    ret += rd[cpmd->m_index].brief() + "\n";
	  }
      }
  } else {
    ret += " " + a.dims().toString() + " array\n";
    ret += "properties:\n";
    for (auto i = cmd->m_props.begin(); i != cmd->m_props.end(); ++i)
      ret += "    " + _ctxt->_string->getString(i->first) + "\n";
  }
  ret += "list of methods\n";
  for (auto i=cmd->m_methods.begin(); i != cmd->m_methods.end(); ++i)
    ret += "    " + _ctxt->_string->getString(i->first) + "\n";
  return ret;
}
