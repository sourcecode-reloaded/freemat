#ifndef __Code_hpp__
#define __Code_hpp__

#include <stdint.h>
#include <vector>
#include "FMLib.hpp"
#include "Object.hpp"
#include "Symbol.hpp"


//
// If we bump out to 64 bits, the performance penalty is small 
// in real code.  That gives us more room.  In that case we can
// do
//    <opcode> <reg0> <reg1> <reg2> <constant>
// where opcode, reg0, reg1 and reg2 are each 10 bits, and constant is
// 26 bits.
//
// Want to be able to handle
// opcode reg reg const
//
//
// Let's assume 7 bits is sufficient for the opcode, and
// take 7 bits for the registers.  That makes 21 bits for
// opcode + 2 regs, leaving 11 bits for the constant.
//
// That might be better.  It allows up to 2K variables.
//
// Other options are to pair up opcodes for the extended
// case.  So that we have two opcodes for a subsref and 
// a subsasgn.  We could have
//
//  emit(OPCODE,reg0,reg1,name)
//
// turn into 
//
//   OPCODE, reg0, reg1
//       EX, name
//
//
// Is this worse than
//
//   emit(OPLOAD,reg0,name)
//   emit(OPSUBSREF,reg1,reg0,args) // reg1 = reg0(args)
//
// No... but consider subsasgn
//
//   emit(OPSUBSASGN,reg0,name)     // name(reg0.butlast) = reg0.last

namespace FM
{

  // A VM instruction is a 64 bit integer laid out as
  // [ flags(4 bits) | constant(20 bits) | reg3(10 bits) | reg2(10 bits) | reg1(10 bits) | opcode(8 bits) ]
  typedef uint64_t insn_t;

  typedef uint8_t op_t;
  typedef int32_t const_t;

  const int opcode_width = 8;
  const int reg_width = 10;
  const int const_width = 20;
  const int opcode_mask = (1 << opcode_width) - 1;
  const int reg_mask = (1 << reg_width) - 1;
  const int const_mask = (1 << const_width) - 1;

  const int shift_reg1 = opcode_width;
  const int shift_reg2 = opcode_width+reg_width;
  const int shift_reg3 = opcode_width+reg_width*2;
  const int shift_constant = opcode_width+reg_width*3;

  //#define opcode(x) ((x) & opcode_mask)
  //#define reg1(x) (((x) >> shift_reg1) & reg_mask)
  //#define reg2(x) (((x) >> shift_reg2) & reg_mask)
  //#define reg3(x) (((x) >> shift_reg3) & reg_mask)
  //#define idx3(x) (((x) >> shift_reg3) & idx_mask)
  //#define constant(x) (((x) >> shift_reg2) & const_mask)
  
  inline int32_t get_constant(insn_t x) {return int32_t((x >> shift_constant) & const_mask);}
  inline uint16_t reg1(insn_t x) {return uint16_t((x >> shift_reg1) & reg_mask);}
  inline uint16_t reg2(insn_t x) {return uint16_t((x >> shift_reg2) & reg_mask);}
  inline uint16_t reg3(insn_t x) {return uint16_t((x >> shift_reg3) & reg_mask);}
  inline uint8_t opcode(insn_t x) {return uint8_t(x & opcode_mask);}
#include "OpCodes.h"

  enum opcodemode
    {
      no_arguments,
      one_register,
      two_registers,
      three_registers,
      register_register_variable,
      register_register_name,
      register_register_captured,
      register_register_free,
      register_constant,
      register_variable,
      register_name,
      register_offset,
      register_captured,
      register_free,
      register_int,
      register_cell,
      constant,
      offset
    };


  struct Instruction
  {
    insn_t _opcode;
    class BasicBlock *_target;
    int32_t _position;
  };

  class BasicBlock
  {
  public:
    std::vector<Instruction> _insnlist;
    int32_t _offset;
    bool _seen;
    BasicBlock() : _insnlist(), _offset(0), _seen(0) {}
  };


  struct CodeBlock
  {
    FMString _name;
    SymbolTable *_syms;
    std::vector<BasicBlock*> _blocklist;
    BasicBlock* _currentBlock;
    Object _namelist;
    Object _constlist; 
    Object _freelist;
    Object _capturedlist;
    std::vector<CodeBlock*> _nested;
    CodeBlock(ThreadContext*ctxt) : _namelist(ctxt), _constlist(ctxt), 
				    _freelist(ctxt), _capturedlist(ctxt) {}
  };

  struct Module
  {
    FMString _name;
    CodeBlock *_main;
    FMMap<FMString,CodeBlock*> _locals;
  };
}

#endif
