#ifndef __ModuleType_hpp__
#define __ModuleType_hpp__

#include "Object.hpp"
#include "Type.hpp"
#include "HashMap.hpp"
#include "HandleType.hpp"
#include "Code.hpp"

namespace FM
{
  struct ThreadContext;

  typedef Object (*fncptr)(const Object &args, int nargout, ThreadContext *ctxt);  
  
  class ModuleData {
  public:
    HashMap<Object> m_locals; // Local functions for this module
    Object m_main; // Main function for this module
    FMString m_name;
    ModuleTypeCode m_modtype;
    Object m_dependencies; // Super classes if encountered
    fncptr m_ptr; // Function pointer for builtin types
    ModuleData(ThreadContext *_ctxt) : m_main(_ctxt), m_dependencies(_ctxt) {}
  };

  class ModuleType : public AggregateType<ModuleData,HandleSemantics> {
  public:
    ModuleType(ThreadContext *ctxt) : AggregateType<ModuleData,HandleSemantics> (ctxt) {}
    void visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {}
    virtual DataCode code() const {return TypeModule;}
    virtual const FMString& name() const {static FMString _name = "module"; return _name;}
    Object import(const Object& foreign) {
      ModuleType *them = foreign.asType<ModuleType>();
      if (this == them) return foreign;
      Object ret = this->makeScalar();
      ModuleData *md = this->rw(ret);
      const ModuleData *cmd = them->ro(ret);
      for (auto i : cmd->m_locals)
	md->m_locals.insert(std::make_pair(i.first.exportTo(_ctxt),i.second.exportTo(_ctxt)));
      md->m_name = cmd->m_name;
      md->m_modtype = cmd->m_modtype;
      md->m_dependencies = cmd->m_dependencies.exportTo(_ctxt);
      return ret;
    }
    virtual FMString describe(const Object &module) {
      FMString ret = "Module ";
      ret += this->ro(module)->m_name + "\n";
      ret += "  type: " + ModuleTypeCodeToString(this->ro(module)->m_modtype) + "\n";
      ret += "  dependencies: " + this->ro(module)->m_dependencies.description() + "\n";
      ret += "  locals:\n";
      for (auto i : this->ro(module)->m_locals)
	{
	  ret += "    " + i.first.description() + "\n";
	}
      return ret;
    }
    virtual FMString brief(const Object &module) {return FMString("Module ") + this->ro(module)->m_name;}
    virtual bool equals(const Object &a, const Object &b)
    {
      return false;
    }
    virtual Object getParens(const Object &module, const Object &args);
    virtual Object call(const Object &module, const Object &args, int nargout);
    virtual Object deref(const Object &a);
    virtual Object builtin(const FMString &name, fncptr addr);
    virtual Object pass();
  };
};

#endif
