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

struct VMInstruction
{
  unsigned opcode : 8;
  unsigned reg1 : 10;
  unsigned reg2 : 10;
  unsigned reg3 : 10;
  int constant : 26;  
};

  /*
  struct OpCode
  {
    int8_t opcode;
    int8_t opreg0;
    int8_t opreg1;
    int8_t opreg2;
    };*/

  typedef uint8_t op_t;
  typedef int16_t const_t;
  typedef int16_t idx_t;

const int opcode_width = 8;
const int reg_width = 7;
const int const_width = 16;
const int idx_width = 10;
const int opcode_mask = (1 << opcode_width) - 1;
const int reg_mask = (1 << reg_width) - 1;
const int const_mask = (1 << const_width) - 1;
const int idx_mask = (1 << idx_width) - 1;

const int shift_reg1 = opcode_width;
const int shift_reg2 = opcode_width+reg_width;
const int shift_reg3 = opcode_width+reg_width*2;

#define opcode(x) ((x) & opcode_mask)
#define reg1(x) (((x) >> shift_reg1) & reg_mask)
#define reg2(x) (((x) >> shift_reg2) & reg_mask)
#define reg3(x) (((x) >> shift_reg3) & reg_mask)
#define idx3(x) (((x) >> shift_reg3) & idx_mask)
#define constant(x) (((x) >> shift_reg2) & const_mask)

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
      constant,
      offset
    };


  struct Instruction
  {
    int32_t _opcode;
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
    Object _capturedlist;
    Object _freelist;
    Object _varlist;
    Object _namelist;
    Object _constlist; 
  };

  struct Module
  {
    FMString _name;
    CodeBlock *_main;
    FMMap<FMString,CodeBlock*> _locals;
  };
}

#endif
