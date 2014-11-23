#include "Assembler.hpp"
#include "StructType.hpp"
#include "CellType.hpp"
#include "CodeType.hpp"
#include "BoolType.hpp"
#include "Compiler.hpp"
#include "TypeUtils.hpp"
#include "ModuleType.hpp"
#include "FunctionType.hpp"

std::string getOpCodeName(FM::op_t);

using namespace FM;

Object Assembler::run(Module *mod)
{
  Object module = _ctxt->_module->makeScalar();
  ModuleData *mp = _ctxt->_module->rw(module);
  mp->m_name = mod->_name;
  Object main_code = this->run(mod->_main);
  mp->is_class = mod->_isclass;
  if (!mp->is_class)
    {
      mp->m_main = _ctxt->_function->fromCode(main_code);
      for (auto i=mod->_locals.constBegin();
	   i != mod->_locals.constEnd();++i)
	{
	  Object co = this->run(i.value());
	  // Make into function object - no closures at the module level
	  Object fo = _ctxt->_function->fromCode(co);
	  mp->m_locals.insert(std::make_pair(_ctxt->_string->makeString(i.key()),fo));
	}
    }
  else
    {
      CodeData *cp = _ctxt->_code->rw(main_code);
      // Compile the methods
      for (auto i=mod->_locals.constBegin(); i!= mod->_locals.constEnd();++i)
	{
	  Object co = _ctxt->_function->fromCode(this->run(i.value()));
	  const Object &myName = _ctxt->_string->makeString("#" + i.key());
	  std::cout <<  "Need home for method code block: " << myName<< "\n";
	  Object *ip = _ctxt->_list->rw(cp->m_consts);
	  for (int j=0;j<cp->m_consts.count();j++)
	    if (ip[j] == myName)
	      {
		ip[j] = co;
		std::cout << "Home found!\n";
		break;
	      }
	}
      mp->m_main = _ctxt->_function->fromCode(main_code);
    }
  return module;
}

Object Assembler::run(CodeBlock *code)
{
  _code = code;
  _vm_codes.clear();
  _nested_codes.clear();
  for (int i=0;i<_postorder.size();i++) delete _postorder[i];
  _postorder.clear();
  depthFirstSearch(_code->_blocklist[0]);
  computeJumpOffsets();
  assemble();
  // Assemble the nested functions
  for (int i=0;i<code->_nested.size();i++)
    {
      Assembler sub(_ctxt);
      _nested_codes.push_back(sub.run(code->_nested[i]));
    }
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
      if (i.value().is_parameter()) param_count++;
      if (i.value().is_return()) return_count++;
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
      if (i.value().is_parameter())
	{
	  for (int j=0;j<cp->m_names.count();j++)
	    if (_ctxt->_string->getString(strings[j]) == i.key())
	      {
		if (!i.value().is_captured())
		  param_ptr[i.value().param_position] = j;
		else
		  param_ptr[i.value().param_position] = indexOfStringInList(_ctxt,_code->_capturedlist,i.key()) + 1000;
		if (i.key() == "varargin") varargin_ndx = j;
	      }
	}
      if (i.value().is_return())
	{
	  for (int j=0;j<cp->m_names.count();j++)
	    if (_ctxt->_string->getString(strings[j]) == i.key())
	      {
		if (!i.value().is_captured())
		  return_ptr[i.value().return_position] = j;
		else
		  return_ptr[i.value().return_position] = indexOfStringInList(_ctxt,_code->_capturedlist,i.key()) + 1000;
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
  // Mark up the constants to replace nested functions with their code blocks
  for (int i=0;i<_nested_codes.size();i++)
    {
      const Object &myName = _ctxt->_string->makeString("#" + _ctxt->_string->getString(_ctxt->_code->ro(_nested_codes[i])->m_name));
      std::cout << "Need home for code block: " << myName << "\n";
      Object *ip = _ctxt->_list->rw(cp->m_consts);
      for (int j=0;j<cp->m_consts.count();j++)
	if (ip[j] == myName)
	  {
	    ip[j] = _nested_codes[i];
	    break;
	  }
    }
  // For classes
  
  return code;
}
