#include "Assembler.hpp"
#include "Struct.hpp"
#include "Algorithms.hpp"

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

Array Assembler::codeObject()
{
  // Compute the codeObject for the code block
  StructArray qp;
  qp.insert("name",Array(_code->_name));
  BasicArray<uint32_t> opcodes(NTuple(_vm_codes.size(),1));
  memcpy(opcodes.data(),&(_vm_codes[0]),_vm_codes.size()*sizeof(uint32_t));
  qp.insert("code",Array(opcodes));
  qp.insert("captured",CellArrayFromStringVector(_code->_capturedlist));
  qp.insert("free",CellArrayFromStringVector(_code->_freelist));
  qp.insert("vars",CellArrayFromStringVector(_code->_varlist));
  qp.insert("names",CellArrayFromStringVector(_code->_namelist));
  Array consts(CellArray,NTuple(1,_code->_constlist.size()));
  BasicArray<Array> &rp(consts.real<Array>());
  for (index_t i=1;i<=_code->_constlist.size();i++)
    rp.set(i,_code->_constlist[i-1]);
  qp.insert("consts",consts);
  qp.insert("locals",EmptyConstructor());
  qp.updateDims();
  Array ret(qp);
  ret.structPtr().setClassName("code_object");
  return ret;
}
