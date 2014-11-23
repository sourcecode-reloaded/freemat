#include "CodeType.hpp"
#include "ThreadContext.hpp"
#include "VM.hpp"
#include "ListType.hpp"
#include "StringType.hpp"
#include "BoolType.hpp"
#include "IntegerType.hpp"
#include "Compiler.hpp"
#include "ClassType.hpp"

using namespace FM;

FMString CodeType::brief(const Object &a) {
  return "code named " + this->ro(a)->m_name.description();
}

FMString CodeType::describe(const Object &a) {
  if (a.isScalar()) {
    const CodeData* cp = this->ro(a);
    FMString ret = "code object\n";
    ret += "   name      : " + cp->m_name.description() + "\n";
    ret += "   code      : " + cp->m_code.description() + "\n";
    ret += "   names     : " + cp->m_names.description() + "\n";
    ret += "   consts    : " + cp->m_consts.description() + "\n";
    ret += "   params    : " + cp->m_params.description() + "\n";
    ret += "   returns   : " + cp->m_returns.description() + "\n";
    ret += "   varargin  : " + cp->m_varargin.description() + "\n";
    ret += "   varargout : " + cp->m_varargout.description() + "\n";
    return ret;
  } else {
    return a.dims().toString() + " code array\n";
  }
}

static inline uint64_t AsmReg(uint64_t code, uint64_t reg) {return code | reg << shift_reg1;}
static inline uint64_t AsmRegReg(uint64_t code, uint64_t reg, uint64_t reg2) {return code | reg << shift_reg1 | reg2 << shift_reg2;}
static inline uint64_t AsmRegRegReg(uint64_t code, uint64_t reg, uint64_t reg2, uint64_t reg3) {return code | reg << shift_reg1 | reg2 << shift_reg2 | reg3 << shift_reg3;}
static inline uint64_t AsmRegConst(uint64_t code, uint64_t reg, uint64_t con) {return code | reg << shift_reg1 | con << shift_constant;}


// TODO - handle multiple arguments, varargin, varargout?
Object CodeType::bindFunction(const Object &func, const Object &argument)
{
  // Create a code object that binds a function to an argument
  Object code = this->makeScalar();
  CodeData *cp = this->rw(code);
  const CodeData *fp = this->ro(func);
  cp->m_name = _ctxt->_string->makeString("Binding of " + fp->m_name.description() + " to " + _ctxt->_class->className(argument).description());
  // There are two constants... the code object (0) and the argument (1)
  cp->m_consts = _ctxt->_list->empty();
  _ctxt->_list->push(cp->m_consts,func); // const[0] = code
  _ctxt->_list->push(cp->m_consts,argument); // const[1] = object
  // For the arguments, we have to copy the arguments of the func block, and delete the first argument
  cp->m_params = _ctxt->_index->makeMatrix(fp->m_params.count()-1,1);
  cp->m_names = _ctxt->_list->empty();
  cp->m_returns = fp->m_returns;
  const Object *fp_names = _ctxt->_list->ro(fp->m_names);
  const ndx_t *fp_params = _ctxt->_index->ro(fp->m_params);
  ndx_t *cp_params = _ctxt->_index->rw(cp->m_params);
  for (int i=1;i<fp->m_params.count();++i) {
    cp_params[i-1] = i-1;
    _ctxt->_list->push(cp->m_names,fp_names[fp_params[i]]);
  }
  // Next handle the returns
  const ndx_t *fp_rets = _ctxt->_index->ro(fp->m_returns);
  ndx_t *cp_rets = _ctxt->_index->rw(cp->m_returns);
  for (int i=0;i<fp->m_returns.count();++i) {
    const Object & retname = fp_names[fp_rets[i]];
    // Check to see if retname is in the list of names alread
    if (!_ctxt->_list->has(cp->m_names,retname))
      _ctxt->_list->push(cp->m_names,retname);
    cp_rets[i] = _ctxt->_list->indexOf(cp->m_names,retname);
  }
  cp->m_code = _ctxt->_uint64->makeMatrix(10+2*cp->m_params.count()+2*cp->m_returns.count(),1);
  uint64_t *code_ip = _ctxt->_uint64->rw(cp->m_code);
  int ip = 0;
  code_ip[ip++] = AsmReg(OP_NEW_LIST,0);                                   // r0 <- new list
  code_ip[ip++] = AsmRegConst(OP_PUSH_INT,0,cp->m_returns.count()); // r0 += num_returns
  code_ip[ip++] = AsmReg(OP_NEW_LIST,1);                                   // r1 <- new list
  code_ip[ip++] = AsmRegConst(OP_LOAD_CONST,2,1);                          // r2 <- const[1] = object
  code_ip[ip++] = AsmRegReg(OP_PUSH,1,2);                                  // r1 += r2
  // Push the arguments
  for (int i=0;i<cp->m_params.count();i++)
    {
      code_ip[ip++] = AsmRegConst(OP_LOAD,2,cp_params[i]);                 // r2 <- name[0] = t
      code_ip[ip++] = AsmRegReg(OP_PUSH,1,2);                              // r1 += r2
    }
  code_ip[ip++] = AsmRegConst(OP_LOAD_CONST,2,0);                          // r2 <- const[0] = code
  code_ip[ip++] = AsmReg(OP_NEW_LIST,3);                                   // r3 <- new list
  code_ip[ip++] = AsmRegReg(OP_PUSH,0,1);                                  // r0 += r1
  code_ip[ip++] = AsmRegRegReg(OP_CALL,3,2,0);                             // r3 <- call(r2,r0)
  for (int i=0;i<cp->m_returns.count();i++)
    {
      code_ip[ip++] = AsmRegReg(OP_FIRST,4,3);
      code_ip[ip++] = AsmRegConst(OP_SAVE,4,cp_rets[i]);
    }
  code_ip[ip++] = OP_RETURN;
  cp->m_varargin = _ctxt->_index->makeScalar(-1);
  cp->m_varargout = _ctxt->_index->makeScalar(-1);
  Disassemble(_ctxt,code);
  return code;
}
