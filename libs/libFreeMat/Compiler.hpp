/*

Codeblock:
VariableNames : string list
Constants : array list
Locals: array list
Arguments:

TOK_REAL:
TOK_IMAG:
TOK_REALF:
TOK_IMAGF:
TOK_STRING:
LOAD_CONST <const_no> -->
   PUSH(GET_CONST(oparg))
TOK_VARIABLE:
// if global
   PUSH(GET_GLOBAL(oparg))
// if local
   PUSH(GET_LOCAL(oparg))
// 

BINARY_GET --> equivalent to X(Y)
  x = pop()
  y = pop()
  push x.get(y)
  
NARY_GET --> equivalent to 
  x = pop()
  n = pop()
  for i=1:n
    vec[i] = pop()
  push x.get(vec)

FIELD_GET
  x = pop()
  y = pop()
  push x.get(y.asString())

STORE_LOCAL id
  x = pop()
  local[id] = x

STORE_GLOBAL id
  x = pop()
  global[id] = x

CELL_UNPACK
  x = pop()
  y = array_from_cell_array(x)
  push(x)

END
  


binary op:

push A
push B
binop name

incr_prefix:
*/


struct OpCode
{
  int8_t opcode;
  int8_t opreg0;
  int8_t opreg1;
  int8_t opreg2;
};

const int8_t OP_NOP = 0;
const int8_t OP_LOAD_CONST = 1;
const int8_t OP_LOAD_LOCAL = 2;
const int8_t OP_LOAD_GLOBAL = 3;
const int8_t OP_BINARY_GET = 4;
const int8_t OP_NARY_GET = 5;
const int8_t OP_FIELD_GET = 6;
const int8_t OP_STORE_LOCAL = 7;
const int8_t OP_STORE_GLOBAL = 8;
const int8_t OP_CELL_UNPACK = 9;
const int8_t OP_CELL_PACK = 10;
const int8_t OP_BINARY_SET = 11;
const int8_t OP_NARY_SET = 12;
const int8_t OP_FIELD_SET = 13;
const int8_t OP_COLON = 14;
const int8_t OP_DOUBLE_COLON = 15;
const int8_t OP_ADD = 16;
const int8_t OP_MINUS = 17;
const int8_t OP_MTIMES = 18;
const int8_t OP_MRDIVIDE = 19;
const int8_t OP_MLDIVIDE = 20;
const int8_t OP_OR = 21;
const int8_t OP_AND = 22;
const int8_t OP_LT = 23;
const int8_t OP_LE = 24;
const int8_t OP_GT = 25;
const int8_t OP_GE = 26;
const int8_t OP_EQ = 27;
const int8_t OP_NE = 28;
const int8_t OP_TIMES = 29;
const int8_t OP_RDIVIDE = 30;
const int8_t OP_LDIVIDE = 31;
const int8_t OP_MINUS = 32;
const int8_t OP_PLUS = 33;
const int8_t OP_NOT = 34;
const int8_t OP_POWER = 35;
const int8_t OP_DOTPOWER = 36;
const int8_t OP_HERMITIAN = 37;
const int8_t OP_TRANSPOSE = 38;
const int8_t OP_VEC_TO_REG = 39;
const int8_t OP_VEC_TO_REG_SILENT = 40;

class Compiler
{
public:
  void assignment(const Tree &var, bool printIt);
  int8_t expression(const Tree &t);
  void block(const Tree & t);
  void statement(const Tree & t);
  void statementType(const Tree &t, bool printIt);
};

#endif
