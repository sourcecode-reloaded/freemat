#include "Assembler.hpp"
#include "StructType.hpp"
#include "CellType.hpp"
#include "CodeType.hpp"
#include "BoolType.hpp"
#include "Compiler.hpp"
#include "TypeUtils.hpp"
#include "ModuleType.hpp"

std::string getOpCodeName(FM::op_t);

using namespace FM;

Object Assembler::run(Module *mod)
{
  Object module = _ctxt->_module->makeScalar();
  ModuleData *mp = _ctxt->_module->rw(module);
  mp->m_name = mod->_name;
  mp->m_main = this->run(mod->_main);
  for (auto i=mod->_locals.constBegin();
       i != mod->_locals.constEnd();++i)
    mp->m_locals.insert(std::make_pair(_ctxt->_string->makeString(i.key()),
					 this->run(i.value())));
  return module;
}

Object Assembler::run(CodeBlock *code)
{
  _code = code;
  _vm_codes.clear();
  for (int i=0;i<_postorder.size();i++) delete _postorder[i];
  _postorder.clear();
  depthFirstSearch(_code->_blocklist[0]);
  computeJumpOffsets();
  assemble();
  return codeObject();
}

void Assembler::computeJumpOffsets()
{
  int total_size = 0;
  for (int i=0;i<_postorder.size();++i)
    {
      total_size += _postorder[i]->_insnlist.size();
      _postorder[i]->_offset = total_size;
    }
  for (int i=0;i<_postorder.size();++i)
    _postorder[i]->_offset = total_size - _postorder[i]->_offset;
  // Update the jump offsets
  for (int i=0;i<_postorder.size();++i)
    {
      BasicBlock *b = _postorder[i];
      for (int j=0;j<b->_insnlist.size();j++)
	{
	  if (b->_insnlist[j]._target)
	    {
	      // Compute the address of the jump target
	      insn_t target_address = b->_insnlist[j]._target->_offset;
	      b->_insnlist[j]._opcode |= (target_address << shift_constant);
	    }
	}
    }
}

void Assembler::depthFirstSearch(BasicBlock *b) 
{
  if (b->_seen) return;
  b->_seen = 1;
  for (int i=0;i<b->_insnlist.size();++i)
    {
      if (b->_insnlist[i]._target)
	depthFirstSearch(b->_insnlist[i]._target);
    }
  _postorder.push_back(b);
}

void Assembler::assemble()
{
  for (int i=0;i<_postorder.size();i++)
    {
      BasicBlock *b = _postorder[_postorder.size()-1-i];
      for (int j=0;j<b->_insnlist.size();j++)
	_vm_codes.push_back(b->_insnlist[j]._opcode);
    }
}

Object Assembler::codeObject()
{
  Object code = _ctxt->_code->makeScalar();
  CodeData *cp = _ctxt->_code->rw(code);
  cp->m_name = _ctxt->_string->makeString(_code->_name);
  Object op = _ctxt->_uint64->makeMatrix(_vm_codes.size(),1);
  memcpy(_ctxt->_uint64->rw(op),&(_vm_codes[0]),_vm_codes.size()*sizeof(uint64_t));
  cp->m_code = op;
  cp->m_names = _code->_namelist;
  cp->m_consts = _code->_constlist;
  // Walk the symbol table and collect up the list of arguments
  const Object *strings = _ctxt->_list->ro(cp->m_names);
  // Count number of parametrs and returns
  int param_count = 0;
  int return_count = 0;
  for (auto i=_code->_syms->syms.constBegin(); i != _code->_syms->syms.constEnd(); ++i)
    {
      if (IS_PARAMETER(i.value())) param_count++;
      if (IS_RETURN(i.value())) return_count++;
    }
  Object param_list = _ctxt->_index->makeMatrix(param_count,1);
  Object return_list = _ctxt->_index->makeMatrix(return_count,1);
  ndx_t * param_ptr = _ctxt->_index->rw(param_list);
  ndx_t * return_ptr = _ctxt->_index->rw(return_list);
  ndx_t varargin_ndx = -1;
  ndx_t varargout_ndx = -1;
  for (auto i=_code->_syms->syms.constBegin();i != _code->_syms->syms.constEnd(); ++i)
    {
      // TODO: This should be more efficient
      if (IS_PARAMETER(i.value())) 
	{
	  for (int j=0;j<cp->m_names.elementCount();j++)
	    if (_ctxt->_string->getString(strings[j]) == i.key())
	      {
		param_ptr[SYM_PARAM_POSITION(i.value())] = j;
		if (i.key() == "varargin") varargin_ndx = j;
	      }
	}
      if (IS_RETURN(i.value())) 
	{
	  for (int j=0;j<cp->m_names.elementCount();j++)
	    if (_ctxt->_string->getString(strings[j]) == i.key())
	      {
		return_ptr[SYM_RETURN_POSITION(i.value())] = j;
		if (i.key() == "varargout") varargout_ndx = j;
	      }
	}
    }
  cp->m_params = param_list;
  cp->m_returns = return_list;
  cp->m_free = _code->_freelist;
  cp->m_captured = _code->_capturedlist;
  cp->m_varargin = _ctxt->_index->makeScalar(varargin_ndx);
  cp->m_varargout = _ctxt->_index->makeScalar(varargout_ndx);
  return code;
}
