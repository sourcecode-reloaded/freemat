#ifndef __CodeType_hpp__
#define __CodeType_hpp__

#include "PODType.hpp"
#include "Object.hpp"
#include "AggregateType.hpp"

namespace FM
{
  class CodeData {
  public:
    Object m_name;      // Name of the code block
    Object m_module;    // Name of the code module
    Object m_code;      // Bytecodes themselves
    Object m_names;     // LIST of names used in the code
    Object m_consts;    // LIST of constants used in the code
    Object m_params;    // NDX_ARRAY of parameters as indices into names list
    Object m_returns;   // NDX_ARRAY of returns as indices into names list
    Object m_varargin;  // NDX scalar - -1 if not varargin case, set to index of varargin name otherwise
    Object m_varargout; // NDX scalar - -1 if not varargout case, set to index of varargout name otherwise
    Object m_captured;  // LIST of captured variables
    Object m_free;      // LIST of free variables
    Object m_lineno;    // RLE line numbers for the bytecodes - an array of int32's
    int    m_registers; // Number of registers used by the code
    CodeData(ThreadContext *ctxt) : 
      m_name(ctxt), m_module(ctxt), m_code(ctxt), m_names(ctxt), 
      m_consts(ctxt), m_params(ctxt), m_returns(ctxt), 
      m_varargin(ctxt), m_varargout(ctxt), m_captured(ctxt),
      m_free(ctxt), m_lineno(ctxt) {}
  };

  struct ThreadContext;

  class CodeType : public AggregateType<CodeData,ValueSemantics> {
  public:
    CodeType(ThreadContext *ctxt) : AggregateType<CodeData,ValueSemantics>(ctxt) {}
    virtual DataCode code() const {return TypeCode;}
    virtual const FMString& name() const {static FMString _name = "code"; return _name;}
    void visitContainedObjects(const ObjectBase *, ObjectVisitor &) const {}
    bool equals(const Object &, const Object &) {
      throw Exception("Code type doesn't support equals yet.");
    }
    virtual FMString describe(const Object &a);
    virtual FMString brief(const Object &a);
    Object import(const Object &foreign) {
      CodeType *them = foreign.asType<CodeType>();
      Object ret = this->makeScalar();
      CodeData *cd = this->rw(ret);
      const CodeData *ccd = them->ro(ret);
      cd->m_name = ccd->m_name.exportTo(_ctxt);
      cd->m_module = ccd->m_module.exportTo(_ctxt);
      cd->m_code = ccd->m_code.exportTo(_ctxt);
      cd->m_names = ccd->m_names.exportTo(_ctxt);
      cd->m_consts = ccd->m_consts.exportTo(_ctxt);
      cd->m_params = ccd->m_params.exportTo(_ctxt);
      cd->m_returns = ccd->m_returns.exportTo(_ctxt);
      cd->m_varargin = ccd->m_varargin.exportTo(_ctxt);
      cd->m_varargout = ccd->m_varargout.exportTo(_ctxt);
      cd->m_captured = ccd->m_captured.exportTo(_ctxt);
      cd->m_free = ccd->m_free.exportTo(_ctxt);
      cd->m_lineno = ccd->m_lineno.exportTo(_ctxt);
      cd->m_registers = ccd->m_registers;
      return ret;
    }
  };
}

#endif
