#include "ClassType.hpp"
#include "AllTypes.hpp"
#include "Compiler.hpp"
#include "TypeUtils.hpp"

using namespace FM;

ClassMetaData::ClassMetaData(ThreadContext *_ctxt) : m_name(_ctxt), m_constructor(_ctxt), m_events(_ctxt), m_defaults(_ctxt)  {
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
  // Construct the object
  Object obj = this->construct(meta);
  return this->invokeConstructor(meta,obj,b);
}

Object ClassMetaType::invokeConstructor(const Object &meta, const Object &self, const Object &args) {
  const ClassMetaData *cmd = this->ro(meta);
  Object olist = _ctxt->_list->makeScalar(self);
  _ctxt->_list->merge(olist,args);
  Object p = cmd->m_constructor.type()->call(cmd->m_constructor,olist,1);
  if (p.count() == 0) throw Exception("Constructor must return object being constructed!");
  return _ctxt->_list->first(p);
}

bool ClassType::isHandle(const Object &a) {
  const ClassData *cd = this->ro(a);
  const ClassMetaData *cmd = _ctxt->_meta->ro(cd->metaClass);
  return cmd->m_ishandle;
}


Object ClassMetaType::construct(const Object &meta) {
  ClassData *cd = new ClassData(_ctxt);
  cd->metaClass = meta;
  cd->m_data = _ctxt->_list->makeScalar(this->ro(meta)->m_defaults);
  Data *q = new Data;
  q->refcnt = 0;
  q->ptr = cd;
  ObjectBase *p = new ObjectBase(q,_ctxt->_class,0,Tuple(1,1),0,0,this->ro(meta)->m_ishandle);
  std::cout << "Constructing class with handle flag " << this->ro(meta)->m_ishandle << " at address " << p << "\n";
  if (this->ro(meta)->m_ishandle)
    _ctxt->_handles.insert(p); // Add a reference to the global list
  return Object(p); // Capacity?
}

Object ClassMetaType::deref(const Object &meta) {
  Object obj = this->construct(meta);
  Object args = _ctxt->_list->empty();
  return this->invokeConstructor(meta,obj,args);
}

FMString ClassMetaType::brief(const Object &a) {
  return "meta class for type " + _ctxt->_string->str(this->ro(a)->m_name);
}

FMString ClassMetaType::describe(const Object &a) {
  const ClassMetaData *cmd = this->ro(a);
  FMString ret = "meta class " + _ctxt->_string->str(cmd->m_name) + "\n";
  ret += "properties:\n";
  for (auto i : cmd->m_properties)
    ret += FMString(" ") + i.first.description() + "\n";
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
      cpmd->m_index = int(cmd->m_defaults.count());
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
  for (auto i : smd->m_properties)
    // TODO - warn if conflict on properties
    this->addProperty(meta,i.first,i.second->m_constant,
		      i.second->m_dependent, i.second->m_default,
		      i.second->m_getter, i.second->m_setter);
  for (auto i : smd->m_methods)
    {
      // Does this method not exist?
      if (cmd->m_methods.find(i.first) == cmd->m_methods.end())
	this->addMethod(meta,i.first,i.second->m_definition,i.second->m_static);
    }
  // TODO - warn if method with same name as constructor already is defined
  this->addMethod(meta,smd->m_name,smd->m_constructor,false);
}

void ClassMetaType::addMethod(Object &meta, const Object &name, const Object &definition, bool is_static) {
  ClassMetaData *cmd = this->rw(meta);
  FMString s_name = _ctxt->_string->str(name);
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
      for (ndx_t i=0;i<a.count();i++) {
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
  auto k = cmd->m_methods.find(b);
  if (k != cmd->m_methods.end())
    return _ctxt->_bound->bindFunction(k->second->m_definition,a);
  throw Exception("Property " + b.description() + " is not defined for class " + _ctxt->_string->str(cmd->m_name));
}

Object ClassType::getMethod(const Object &a, const Object &b) {
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto j = cmd->m_methods.find(b);
  if (j == cmd->m_methods.end())
    throw Exception("Method " + b.description() + " is not defined for class " + _ctxt->_string->str(cmd->m_name));
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
    throw Exception("Property " + b.description() + " is not defined for class " + _ctxt->_string->str(cmd->m_name));
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
  ndx_t num_assignments = std::max<ndx_t>(1,a.count());
  const Object *bp = &b;
  if (b.isList())
    bp = _ctxt->_list->ro(b);
  if (b.isList() && (num_assignments > b.count()))
    throw Exception("Mismatch in number of left and right hand sides in expression a.property = b");
  for (ndx_t n=0;n<num_assignments;n++)
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
  return _ctxt->_string->str(cmd->m_name);
}

//TODO Add power functions
ClassType::ClassType(ThreadContext *ctxt) :
  AggregateType<FM::ClassData,ValueSemantics>(ctxt),
  m_deletefunc_name(ctxt->_string->makeString("delete")),
  m_plusfunc_name(ctxt->_string->makeString("plus")),
  m_mtimes_name(ctxt->_string->makeString("mtimes")),
  m_mldivide_name(ctxt->_string->makeString("mldivide")),
  m_mrdivide_name(ctxt->_string->makeString("mrdivide")),
  m_ldivide_name(ctxt->_string->makeString("ldivide")),
  m_rdivide_name(ctxt->_string->makeString("rdivide")),
  m_times_name(ctxt->_string->makeString("times")),
  m_subtract_name(ctxt->_string->makeString("subtract")),
  m_le_name(ctxt->_string->makeString("le")),
  m_lt_name(ctxt->_string->makeString("lt")),
  m_gt_name(ctxt->_string->makeString("gt")),
  m_ge_name(ctxt->_string->makeString("ge")),
  m_ne_name(ctxt->_string->makeString("ne")),
  m_eq_name(ctxt->_string->makeString("eq")),
  m_or_name(ctxt->_string->makeString("or")),
  m_and_name(ctxt->_string->makeString("and")),
  m_neg_name(ctxt->_string->makeString("neg")),
  m_uplus_name(ctxt->_string->makeString("uplus")),
  m_ctranspose_name(ctxt->_string->makeString("ctranspose")),
  m_transpose_name(ctxt->_string->makeString("transpose")),
  m_colon_name(ctxt->_string->makeString("colon")),
  m_vertcat_name(ctxt->_string->makeString("vertcat")),
  m_horzcat_name(ctxt->_string->makeString("horzcat")),
  m_subsref_name(ctxt->_string->makeString("subsref")),
  m_subsasgn_name(ctxt->_string->makeString("subsasgn")),
  m_type_name(ctxt->_string->makeString("type")),
  m_subs_name(ctxt->_string->makeString("subs")),
  m_subsindex_name(ctxt->_string->makeString("subsindex"))
{
}

void ClassType::destroyObject(ObjectBase* p) {
  ClassData *cd = static_cast<ClassData *>(p->ptr());
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  auto j = cmd->m_methods.find(m_deletefunc_name);
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
    p->refcnt++;
    std::cout << "Destroying object with address " << p << "\n";
    _ctxt->_function->call(j->second->m_definition,_ctxt->_list->makeScalar(sacrificial),1);
    p->refcnt--;
    _ctxt->_handles.erase(p);
    sacrificial.swap(save);
  }
  AggregateType::destroyObject(p);
}

Object ClassType::ufunc(const Object &a, const Object &funcname) {
  Object func(_ctxt);
  if (!hasMethod(a,funcname,func))
    throw Exception("No function " + funcname.description() + " defined for class ");
  return _ctxt->_function->call(func,_ctxt->_list->makeScalar(a),1);
}

Object ClassType::bifunc(const Object &a, const Object &b, const Object &funcname) {
  Object func(_ctxt);
  if (!hasMethod(a,funcname,func))
    throw Exception("No function " + funcname.description() + " defined for class ");
  Object args = _ctxt->_list->empty();
  _ctxt->_list->push(args,a);
  _ctxt->_list->push(args,b);
  return _ctxt->_function->call(func,args,1);
}

Object ClassType::DoubleColon(const Object &a, const Object &b, const Object &c) {
  Object func(_ctxt);
  if (!hasMethod(a,m_colon_name,func))
    throw Exception("No function colon defined for class ");
  Object args = _ctxt->_list->empty();
  _ctxt->_list->push(args,a);
  _ctxt->_list->push(args,b);
  _ctxt->_list->push(args,c);
  return _ctxt->_function->call(func,args,1);
}

Object ClassType::NCat(const Object &a, ndx_t dimension) {
  // First, find the dominant class object
  const Object *ap = _ctxt->_list->ro(a);
  const Object *root = nullptr;
  for (ndx_t i=0;i<a.count();i++)
    if (ap[i].isClass()) {
      root = ap+i;
      break;
    }
  if (!root) throw Exception("NCat called on class type, but no classes found!");
  Object func(_ctxt);
  if ((dimension == 0) && !hasMethod(*root,m_vertcat_name,func))
    throw Exception("No vertcat method found for class");
  if ((dimension == 1) && !hasMethod(*root,m_horzcat_name,func))
    throw Exception("No horzcat method found for class");
  return _ctxt->_function->call(func,a,1);
}

bool ClassType::hasSubsasgn(const Object &a) {
  Object def(_ctxt);
  return this->hasMethod(a,m_subsasgn_name,def);
}

bool ClassType::hasSubsref(const Object &a) {
  Object def(_ctxt);
  return this->hasMethod(a,m_subsref_name,def);
}

Object ClassType::asIndex(const Object &a, ndx_t)
{
  Object def(_ctxt);
  if (!this->hasMethod(a,m_subsindex_name,def))
    throw Exception("This class does not support being used as an index - you must overload subsindex to provde that functionality");
  return _ctxt->_index->convert(_ctxt->_list->first(_ctxt->_function->call(def,_ctxt->_list->makeScalar(a),1)));
}

Object ClassType::makeSubstruct(const Object &b) {
  Object s(_ctxt->_struct->empty());
  StructData *sd = _ctxt->_struct->rw(s);
  sd->m_fields.insert(std::make_pair(m_type_name,0));
  sd->m_fields.insert(std::make_pair(m_subs_name,1));
  sd->m_data = _ctxt->_cell->makeMatrix(b.count()/2,1);
  Object *dp = _ctxt->_cell->rw(sd->m_data);
  const Object *bp = _ctxt->_list->ro(b);
  for (int i=0;i<b.count()/2;i++) {
    Object entry = _ctxt->_list->makeMatrix(2,1);
    Object *ep = _ctxt->_list->rw(entry);
    switch(int(bp[2*i].asDouble())) {
    case 0:
      ep[0] = _ctxt->_string->makeString("()");
      ep[1] = makeCellFromList(_ctxt,bp[2*i+1]);
      break;
    case 1:
      ep[0] = _ctxt->_string->makeString("{}");
      ep[1] = makeCellFromList(_ctxt,bp[2*i+1]);
      break;
    case 2:
      ep[0] = _ctxt->_string->makeString(".");
      ep[1] = bp[2*i+1];
      break;
    }
    dp[i] = entry;
  }
  _ctxt->_struct->updateDims(s);
  return s;
}

Object ClassType::subsref(const Object &a, const Object &b) {
  Object subsref_func(_ctxt);
  if (!hasMethod(a,m_subsref_name,subsref_func)) throw Exception("overloaded subsref called, but no method defined!");
  return _ctxt->_function->call(subsref_func,_ctxt->_list->makePair(a,makeSubstruct(b)),1);
}

void ClassType::subsasgn(Object &a, const Object &args, const Object &b) {
  Object subsasgn_func(_ctxt);
  if (!hasMethod(a,m_subsasgn_name,subsasgn_func)) throw Exception("overloaded subsasgn called, but no method defined");
  a = _ctxt->_list->first(_ctxt->_function->call(subsasgn_func,_ctxt->_list->makeTriple(a,makeSubstruct(args),b),1));
}

FMString ClassType::describe(const Object &a) {
  // Get the meta class for this class
  const ClassData *cd = this->ro(a);
  const Object &myMeta = cd->metaClass;
  const ClassMetaData *cmd = _ctxt->_meta->ro(myMeta);
  FMString ret = _ctxt->_string->str(cmd->m_name);
  if (a.isScalar()) {
    ret += "\n";
    const Object *rd = _ctxt->_list->ro(_ctxt->_list->scalarValue(cd->m_data));
    ret += "properties:\n";
    for (auto i : cmd->m_properties)
      {
	ClassPropertyMetaData *cpmd = i.second;
	ret += "    " + _ctxt->_string->str(i.first) + ": ";
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
    for (auto i : cmd->m_properties)
      ret += "    " + _ctxt->_string->str(i.first) + "\n";
  }
  ret += "list of events\n";
  const Object *op = _ctxt->_list->ro(cmd->m_events);
  for (int i=0;i<cmd->m_events.count();i++)
    ret += "   " + op[i].description() + "\n";
  ret += "list of methods\n";
  for (auto i : cmd->m_methods)
    {
      ret += "    " + _ctxt->_string->str(i.first);
      if (i.second->m_static)
	ret += " (static)";
      ret += "\n";
    }
  ret += "refcount = " + Stringify(a.refcnt()) + "\n";
  return ret;
}
