#ifndef __ModuleType_hpp__
#define __ModuleType_hpp__

#include "Object.hpp"
#include "Type.hpp"
#include "HashMap.hpp"
#include "HandleType.hpp"

namespace FM
{
  struct ThreadContext;

  class ModuleData {
  public:
    HashMap<Object> m_locals; // Local functions for this module
    Object m_main; // Main function for this module
    FMString m_name;
    bool is_class;
    ModuleData(ThreadContext *_ctxt) : m_main(_ctxt) {}
  };

  class ModuleType : public AggregateType<ModuleData,HandleSemantics> {
  public:
    ModuleType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeModule;}
    virtual const FMString& name() const {static FMString _name = "module"; return _name;}
    virtual FMString describe(const Object &module) {
      FMString ret = "Module ";
      ret += this->ro(module)->m_name + "\n";
      ret += "  locals:\n";
      for (auto i=this->ro(module)->m_locals.begin();i!=this->ro(module)->m_locals.end();++i)
	{
	  ret += "    " + i->first.description() + "\n";
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
  };
};

#endif
