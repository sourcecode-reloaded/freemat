opcodemode getOpCodeMode(op_t opcode)
{
  switch (opcode)
  {
    case OP_NOP: return no_arguments;
    case OP_RETURN: return no_arguments;
    case OP_PUSH: return one_register;
    case OP_POP: return one_register;
    case OP_CALL: return constant;
    case OP_LOAD_FREE: return register_free;
    case OP_SAVE_FREE: return register_free;
    case OP_LOAD_CAPTURED: return register_captured;
    case OP_SAVE_CAPTURED: return register_captured;
    case OP_SAVE: return register_variable;
    case OP_DCOLON: return one_register;
    case OP_SUBSASGN: return register_variable;
    case OP_LOAD_CONST: return register_constant;
    case OP_LOAD: return register_variable;
    case OP_START_LIST: return one_register;
    case OP_END_LIST: return one_register;
    case OP_LOAD_STACK: return register_constant;
    case OP_SUBSREF: return one_register;
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
    case OP_LHSCOUNT: return register_variable;
    case OP_SUBSASGNM: return register_variable;
    case OP_ZERO: return one_register;
    case OP_CELLROWDEF: return no_arguments;
    case OP_HCAT: return no_arguments;
    case OP_VCAT: return one_register;
    case OP_LOAD_GLOBAL: return register_name;
    case OP_LOAD_PERSIST: return register_name;
    case OP_LOAD_DYNAMIC: return register_name;
    case OP_SAVE_GLOBAL: return register_name;
    case OP_SAVE_DYNAMIC: return register_name;
    case OP_SAVE_PERSIST: return register_name;
    case OP_JUMP_ZERO: return register_offset;
    case OP_JUMP: return offset;
    case OP_TRY_BEGIN: return register_offset;
    case OP_TRY_END: return no_arguments;
    case OP_THROW: return one_register;
    case OP_PRINT: return one_register;
    case OP_DEREF: return two_registers;
    case OP_SUBSASGN_GLOBAL: return register_name;
    case OP_SUBSASGN_PERSIST: return register_name;
    case OP_SUBSASGN_CAPTURED: return register_captured;
    case OP_SUBSASGN_FREE: return register_free;
    case OP_SUBSASGN_DYNAMIC: return register_name;
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
    case OP_POP: return "POP";
    case OP_CALL: return "CALL";
    case OP_LOAD_FREE: return "LOAD_FREE";
    case OP_SAVE_FREE: return "SAVE_FREE";
    case OP_LOAD_CAPTURED: return "LOAD_CAPTURED";
    case OP_SAVE_CAPTURED: return "SAVE_CAPTURED";
    case OP_SAVE: return "SAVE";
    case OP_DCOLON: return "DCOLON";
    case OP_SUBSASGN: return "SUBSASGN";
    case OP_LOAD_CONST: return "LOAD_CONST";
    case OP_LOAD: return "LOAD";
    case OP_START_LIST: return "START_LIST";
    case OP_END_LIST: return "END_LIST";
    case OP_LOAD_STACK: return "LOAD_STACK";
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
    case OP_LHSCOUNT: return "LHSCOUNT";
    case OP_SUBSASGNM: return "SUBSASGNM";
    case OP_ZERO: return "ZERO";
    case OP_CELLROWDEF: return "CELLROWDEF";
    case OP_HCAT: return "HCAT";
    case OP_VCAT: return "VCAT";
    case OP_LOAD_GLOBAL: return "LOAD_GLOBAL";
    case OP_LOAD_PERSIST: return "LOAD_PERSIST";
    case OP_LOAD_DYNAMIC: return "LOAD_DYNAMIC";
    case OP_SAVE_GLOBAL: return "SAVE_GLOBAL";
    case OP_SAVE_DYNAMIC: return "SAVE_DYNAMIC";
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
    case OP_SUBSASGN_CAPTURED: return "SUBSASGN_CAPTURED";
    case OP_SUBSASGN_FREE: return "SUBSASGN_FREE";
    case OP_SUBSASGN_DYNAMIC: return "SUBSASGN_DYNAMIC";
  }
}
