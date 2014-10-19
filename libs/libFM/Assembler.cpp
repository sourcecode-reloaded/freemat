#include "Assembler.hpp"
#include "StructType.hpp"
#include "CellType.hpp"
#include "CodeType.hpp"
#include "Compiler.hpp"
#include "TypeUtils.hpp"

std::string getOpCodeName(FM::op_t);

using namespace FM;

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
  Object code = _ctxt->_code->empty();
  CodeData *cp = _ctxt->_code->readWriteData(code);
  cp->m_name = _ctxt->_string->makeString(_code->_name);
  Object op = _ctxt->_uint64->makeMatrix(_vm_codes.size(),1);
  memcpy(_ctxt->_uint64->readWriteData(op),&(_vm_codes[0]),_vm_codes.size()*sizeof(uint64_t));
  cp->m_code = op;
  cp->m_names = _code->_namelist;
  cp->m_consts = _code->_constlist;
  // Walk the symbol table and collect up the list of arguments
  const Object *strings = _ctxt->_list->readOnlyData(cp->m_names);
  Object param_list = _ctxt->_list->empty();
  Object return_list = _ctxt->_list->empty();
  for (FMMap<FMString,int>::const_iterator i=_code->_syms->syms.constBegin();i != _code->_syms->syms.constEnd(); ++i)
    {
      // TODO: This should be more efficient
      if (IS_PARAMETER(i.value())) 
	{
	  for (int j=0;j<cp->m_names.elementCount();j++)
	    if (_ctxt->_string->getString(strings[j]) == i.key())
	      addIndexToList(_ctxt,param_list,j);
	}
      if (IS_RETURN(i.value())) 
	{
	  for (int j=0;j<cp->m_names.elementCount();j++)
	    if (_ctxt->_string->getString(strings[j]) == i.key())
	      addIndexToList(_ctxt,return_list,j);
	}
    }
  cp->m_params = param_list;
  cp->m_returns = return_list;
  return code;
}
