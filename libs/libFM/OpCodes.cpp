opcodemode getOpCodeMode(op_t opcode)
{
  switch (opcode)
  {
    case OP_NOP: return no_arguments;
    case OP_RETURN: return no_arguments;
    case OP_PUSH: return two_registers;
    case OP_FIRST: return two_registers;
    case OP_CALL: return three_registers;
    case OP_SAVE: return register_name;
    case OP_DCOLON: return two_registers;
    case OP_SUBSASGN: return register_register_name;
    case OP_LOAD_CONST: return register_constant;
    case OP_LOAD: return register_name;
    case OP_NEW_LIST: return one_register;
    case OP_SUBSREF: return three_registers;
    case OP_COLON: return three_registers;
    case OP_ADD: return three_registers;
    case OP_MINUS: return three_registers;
    case OP_MTIMES: return three_registers;
    case OP_MRDIVIDE: return three_registers;
    case OP_MLDIVIDE: return three_registers;
    case OP_OR: return three_registers;
    case OP_AND: return three_registers;
    case OP_LT: return three_registers;
    case OP_LE: return three_registers;
    case OP_GT: return three_registers;
    case OP_GE: return three_registers;
    case OP_EQ: return three_registers;
    case OP_NE: return three_registers;
    case OP_TIMES: return three_registers;
    case OP_RDIVIDE: return three_registers;
    case OP_CASE: return three_registers;
    case OP_COLUMN: return three_registers;
    case OP_LDIVIDE: return three_registers;
    case OP_NUMCOLS: return two_registers;
    case OP_NEG: return two_registers;
    case OP_PLUS: return two_registers;
    case OP_NOT: return two_registers;
    case OP_POWER: return three_registers;
    case OP_DOTPOWER: return three_registers;
    case OP_HERMITIAN: return two_registers;
    case OP_TRANSPOSE: return two_registers;
    case OP_INCR: return one_register;
    case OP_ZERO: return one_register;
    case OP_CELLROWDEF: return no_arguments;
    case OP_HCAT: return two_registers;
    case OP_VCAT: return two_registers;
    case OP_LOAD_GLOBAL: return register_name;
    case OP_LOAD_PERSIST: return register_name;
    case OP_SAVE_GLOBAL: return register_name;
    case OP_SAVE_PERSIST: return register_name;
    case OP_JUMP_ZERO: return register_offset;
    case OP_JUMP: return offset;
    case OP_TRY_BEGIN: return register_offset;
    case OP_TRY_END: return no_arguments;
    case OP_THROW: return one_register;
    case OP_PRINT: return one_register;
    case OP_DEREF: return two_registers;
    case OP_SUBSASGN_GLOBAL: return register_register_name;
    case OP_SUBSASGN_PERSIST: return register_register_name;
    case OP_LOOPCOUNT: return one_register;
    case OP_PUSH_INT: return register_int;
    case OP_LENGTH: return two_registers;
    case OP_POP: return two_registers;
    case OP_END: return three_registers;
    case OP_LOOKUP: return register_register_name;
    case OP_LOAD_CELL: return register_cell;
    case OP_SAVE_CELL: return register_cell;
    case OP_MAKE_CLOSURE: return three_registers;
    case OP_PUSH_CELL: return register_cell;
    default:
      return three_registers;
  }
}


std::string getOpCodeName(op_t opcode)
{
  switch(opcode)
  {
    case OP_NOP: return "NOP";
    case OP_RETURN: return "RETURN";
    case OP_PUSH: return "PUSH";
    case OP_FIRST: return "FIRST";
    case OP_CALL: return "CALL";
    case OP_SAVE: return "SAVE";
    case OP_DCOLON: return "DCOLON";
    case OP_SUBSASGN: return "SUBSASGN";
    case OP_LOAD_CONST: return "LOAD_CONST";
    case OP_LOAD: return "LOAD";
    case OP_NEW_LIST: return "NEW_LIST";
    case OP_SUBSREF: return "SUBSREF";
    case OP_COLON: return "COLON";
    case OP_ADD: return "ADD";
    case OP_MINUS: return "MINUS";
    case OP_MTIMES: return "MTIMES";
    case OP_MRDIVIDE: return "MRDIVIDE";
    case OP_MLDIVIDE: return "MLDIVIDE";
    case OP_OR: return "OR";
    case OP_AND: return "AND";
    case OP_LT: return "LT";
    case OP_LE: return "LE";
    case OP_GT: return "GT";
    case OP_GE: return "GE";
    case OP_EQ: return "EQ";
    case OP_NE: return "NE";
    case OP_TIMES: return "TIMES";
    case OP_RDIVIDE: return "RDIVIDE";
    case OP_CASE: return "CASE";
    case OP_COLUMN: return "COLUMN";
    case OP_LDIVIDE: return "LDIVIDE";
    case OP_NUMCOLS: return "NUMCOLS";
    case OP_NEG: return "NEG";
    case OP_PLUS: return "PLUS";
    case OP_NOT: return "NOT";
    case OP_POWER: return "POWER";
    case OP_DOTPOWER: return "DOTPOWER";
    case OP_HERMITIAN: return "HERMITIAN";
    case OP_TRANSPOSE: return "TRANSPOSE";
    case OP_INCR: return "INCR";
    case OP_ZERO: return "ZERO";
    case OP_CELLROWDEF: return "CELLROWDEF";
    case OP_HCAT: return "HCAT";
    case OP_VCAT: return "VCAT";
    case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
    case OP_LOAD_PERSIST: return "LOAD_PERSIST";
    case OP_SAVE_GLOBAL: return "SAVE_GLOBAL";
    case OP_SAVE_PERSIST: return "SAVE_PERSIST";
    case OP_JUMP_ZERO: return "JUMP_ZERO";
    case OP_JUMP: return "JUMP";
    case OP_TRY_BEGIN: return "TRY_BEGIN";
    case OP_TRY_END: return "TRY_END";
    case OP_THROW: return "THROW";
    case OP_PRINT: return "PRINT";
    case OP_DEREF: return "DEREF";
    case OP_SUBSASGN_GLOBAL: return "SUBSASGN_GLOBAL";
    case OP_SUBSASGN_PERSIST: return "SUBSASGN_PERSIST";
    case OP_LOOPCOUNT: return "LOOPCOUNT";
    case OP_PUSH_INT: return "PUSH_INT";
    case OP_LENGTH: return "LENGTH";
    case OP_POP: return "POP";
    case OP_END: return "END";
    case OP_LOOKUP: return "LOOKUP";
    case OP_LOAD_CELL: return "LOAD_CELL";
    case OP_SAVE_CELL: return "SAVE_CELL";
    case OP_MAKE_CLOSURE: return "MAKE_CLOSURE";
    case OP_PUSH_CELL: return "PUSH_CELL";
    default: return "UNK";
  }
}
