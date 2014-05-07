#ifndef __Code_hpp__
#define __Code_hpp__

#include <stdint.h>
#include <vector>
#include "FMLib.hpp"
#include "Array.hpp"
#include "Symbol.hpp"

struct OpCode
{
  int8_t opcode;
  int8_t opreg0;
  int8_t opreg1;
  int8_t opreg2;
};

typedef uint8_t op_t;
typedef int16_t const_t;

#include "OpCodes.h"

enum opcodemode
  {
    no_arguments,
    one_register,
    two_registers,
    three_registers,
    register_constant,
    register_variable,
    register_name,
    register_offset,
    register_captured,
    register_free,
    constant,
    offset
  };


struct Instruction
{
  int32_t _opcode;
  struct BasicBlock *_target;
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
  FMStringList _capturedlist;
  FMStringList _freelist;
  FMStringList _varlist;
  FMStringList _namelist;
  FMVector<Array> _constlist;
};

struct Module
{
  FMString _name;
  CodeBlock *_main;
  FMMap<FMString,CodeBlock*> _locals;
};

#endif
