#include "Assembler.hpp"
#include "StructType.hpp"
#include "CellType.hpp"
//#include "Algorithms.hpp"

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
  std::cout << "Jump Offset computation!\n";
  int total_size = 0;
  for (int i=0;i<_postorder.size();++i)
    {
      total_size += _postorder[i]->_insnlist.size();
      printf("Block %x - offset %d\n",_postorder[i],total_size);
      _postorder[i]->_offset = total_size;
    }
  // Update the jump offsets
  for (int i=0;i<_postorder.size();++i)
    {
      BasicBlock *b = _postorder[i];
      for (int j=0;j<b->_insnlist.size();j++)
	{
	  if (b->_insnlist[j]._target)
	    {
	      // Compute the address of the jump target
	      int target_address = total_size-b->_insnlist[j]._target->_offset;
	      b->_insnlist[j]._opcode |= (target_address << 16);
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
  int ip = 0;
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
  Object op = _b->_uint32->makeMatrix(_vm_codes.size(),1);
  memcpy(_b->_uint32->readWriteData(op),&(_vm_codes[0]),_vm_codes.size()*sizeof(uint32_t));
  _b->_struct->setScalar(qp,"code",op);
  _b->_struct->setScalar(qp,"captured",_b->makeCellFromStrings(_code->_capturedlist));
  _b->_struct->setScalar(qp,"free",_b->makeCellFromStrings(_code->_freelist));
  _b->_struct->setScalar(qp,"vars",_b->makeCellFromStrings(_code->_varlist));
  _b->_struct->setScalar(qp,"names",_b->makeCellFromStrings(_code->_namelist));
  op = _b->_cell->makeMatrix(1,_code->_constlist.size());
  Object* opp = _b->_cell->readWriteData(op);
  for (int i=0;i<_code->_constlist.size();i++)
    opp[i] = _code->_constlist[i];
  _b->_struct->setScalar(qp,"consts",op);
  _b->_struct->setScalar(qp,"locals",_b->_cell->makeMatrix(0,0));
  return qp;
}
