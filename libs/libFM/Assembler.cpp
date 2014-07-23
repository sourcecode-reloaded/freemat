#include "Assembler.hpp"
#include "StructType.hpp"
#include "CellType.hpp"
//#include "Algorithms.hpp"
#include "Compiler.hpp"

std::string getOpCodeName(FM::op_t);

using namespace FM;

Assembler::Assembler(CodeBlock *code) : _code(code)
{
}

void Assembler::run()
{
  depthFirstSearch(_code->_blocklist[0]);
  computeJumpOffsets();
  assemble();
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
	      int target_address = b->_insnlist[j]._target->_offset;
	      b->_insnlist[j]._opcode |= (target_address << (opcode_width + reg_width));
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

Object Assembler::codeObject(BaseTypes *_b)
{
  FMStringList fields;
  fields << "name" << "code" << "captured" << "free" << "vars" << "names" << "consts" << "locals";
  Object qp = _b->_struct->makeScalarStruct(fields);
  _b->_struct->setScalar(qp,"name",_b->_string->makeString(_code->_name));
  Object op = _b->_uint64->makeMatrix(_vm_codes.size(),1);
  memcpy(_b->_uint64->readWriteData(op),&(_vm_codes[0]),_vm_codes.size()*sizeof(uint64_t));
  _b->_struct->setScalar(qp,"code",op);
  _b->_struct->setScalar(qp,"captured",_code->_capturedlist);
  _b->_struct->setScalar(qp,"free",_code->_freelist);
  _b->_struct->setScalar(qp,"vars",_code->_varlist);
  _b->_struct->setScalar(qp,"names",_code->_namelist);
  _b->_struct->setScalar(qp,"consts",_code->_constlist);
  _b->_struct->setScalar(qp,"locals",_b->_cell->makeMatrix(0,0));
  return qp;
}
