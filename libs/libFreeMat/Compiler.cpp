#include "Compiler.hpp"

// Two types of registers.  Scalar registers and vector registers.

// Remove the end references with AST rewriting...
//
// Calls a function and then returns a pointer to the base
// of the return args on the stack
reg_t Compiler::doFunctionExpression(const Tree &t,
				      int narg_out,
				      bool outputOptional) {
  reg_t sp = getStackRegister();
  FMString funcname = t.first().text();
  if ((t.numChildren() >= 2) && t.second().is(TOK_PARENS)) {
    const Tree &s(t.second());
    for (int p=0;p<s.numChildren();p++)
      multiexpr(s.child(p));
  }
  emit(OP_CALL_GLOBAL,sp,getConstantID(funcname));
  return sp;
}

void Compiler::pushAndFree(reg_t x)
{
  emit(OP_PUSH,x);
  freeRegister(x);
}

reg_t Compiler::popStack()
{
  reg_t x = getRegister();
  emit(OP_POP,x);
  return x;
}

// Invoke a function, but we expect a scalar argument
// -- expressionStatement, rhs
reg_t Compiler::doScalarFunctionExpression(const Tree &t, bool optional_output) {
  reg_t sp = doFunctionExpression(t,(optional_output ? 0 : 1),optional_output);
  emit(OP_RESTORE_SP,sp,1);
  freeRegister(sp);
  return popStack();
}

reg_t Compiler::doBinaryOperator(const Tree &t, op_t opcode) {
  reg_t r1 = expression(t.first());
  reg_t r2 = expression(t.second());
  reg_t r0 = getRegister();
  emit(opcode,r0,r1,r2);
  freeRegister(r1);
  freeRegister(r2);
  return r0;
}

reg_t Compiler::doUnaryOperator(const Tree &t, op_t opcode) {
  reg_t r1 = expression(t.first());
  reg_t r0 = getRegister();
  emit(opcode,r0,r1);
  freeRegister(r1);
  return r0;
}

reg_t Compiler::doubleColon(const Tree &t) {
  pushAndFree(expression(t.first().first()));
  pushAndFree(expression(t.first().second()));
  pushAndFree(expression(t.second()));
  reg_t r0 = getRegister();
  emit(OP_DOUBLE_COLON,r0); // Pops args 0,1,2 off of top of stack
  return r0;
}

void Compiler::assignment(const Tree &t, bool printIt, reg_t b) {
  FMString varname = t.first().text();
  if (!is_global_variable(varname))
    createLocalVariable(varname);
  if (t.numChildren() == 1)
    {
      if (is_local_variable(varname))
	emit(OP_SAVE_LOCAL,b,getLocalVariableId(varname));
      else
	emit(OP_SAVE_GLOBAL,b,getGlobalVariableId(varname));
      freeRegister(b);
      return;
    }
  reg_t sp = getStackRegister();
  flattenDereferenceTreeToStack(t,1);
  if (is_local_variable(varname))
    emit(OP_SUBSASGN_LOCAL,b,sp,getLocalVariableId(varname));
  else
    emit(OP_SUBSASGN_GLOBAL,b,sp,getGlobalVariableId(varname));
  freeRegister(sp);
  freeRegister(b);
}

// We want to map a dereference tree to the stack.
void Compiler::flattenDereferenceTreeToStack(const Tree &s, int first_child) {
  for (int index = first_child; index < t.numChildren();index++)
    deref(s.child(index));
}

void Compiler::deref(const Tree &s) {
  switch (s.token())
    {
    case TOK_PARENS:
      {
	doPushConstantString("()");
	for (int p=0;p<s.numChildren();p++)
	  multiexpr(s.child(p));
	break;
      }
    case TOK_BRACES:
      {
	doPushConstantString("{}");
	for (int p=0;p<s.numChildren();p++)
	  multiexpr(s.child(p));
	break;
      }
    case '.':
      {
	doPushConstantString(".");
	doPushConstantString(s.first().text());
	break;
      }
    case TOK_DYN:
      {
	doPushConstantString(".");
	reg_t p = expression(s.first());
	emit(OP_PUSH,p);
	freeRegister(p);
      }
    default:
      throw Exception("Unknown expression!");
    }
}

reg_t Compiler::fetchConstantString(const FMString &constant)
{
  reg_t r0 = getRegister();
  emit(OP_LOAD_CONST,r0,getConstantID(Array(constant)));
  return r0;
}

reg_t Compiler::fetchGlobalVariable(const FMString &varname)
{
  reg_t x = getRegister();
  emit(OP_LOAD_GLOBAL,x,getGlobalVariableId(varname));
  return x;
}

reg_t Compiler::fetchLocalVariable(const FMString &varname)
{
  reg_t x = getRegister();
  emit(OP_LOAD_LOCAL,x,getLocalVariableId(varname));
  return x;
}

reg_t Compiler::fetchVariable(const FMString &varname)
{
  if (is_local_variable(varname))
    return fetchLocalVariable(varname);
  else
    return fetchGlobalVariable(varname);
}

reg_t Compiler::getStackRegister()
{
  reg_t x = getRegister();
  emit(OP_SAVE_STACK,x);
  return x;
}

void Compiler::restoreStack(reg_t x, int offset=0)
{
  emit(OP_RESTORE_STACK,x,offset);
  freeRegister(x);
}

// Takes an expression like A(...).foo{32} and
// expands it (possibly into multiple values).  They
// are left on the stack.  Caller is responsible
// for removing them.
void Compiler::rhs(const Tree &t) {
  FMString varname = t.first().text();
  int min_children_to_reindex = 1;
  reg_t x;
  if (!symbol_table_contains(varname))
    {
      x = doScalarFunctionExpression(t,false);
      min_children_to_reindex = 2;
    }
  else
    {
      x = fetchVariable(varname);
      min_children_to_reindex = 1;
    }
  if (t.numChildren() > min_children_to_reindex)
    {
      reg_t sp = getStackRegister();
      flattenDereferenceTreeToStack(t,min_children_to_reindex);
      emit(OP_SUBSREF,x,sp);
      freeRegister(x);
      return y;
    }
  else
    pushAndFree(x);
}

void Compiler::expressionStatement(const Tree &t, bool printIt) {
  
}


void Compiler::multiexpr(const Tree &t) {
  if (t.is(TOK_VARIABLE))
    rhs(t);
  else if (!t.is(TOK_KEYWORD))
    emit(OP_PUSH,expression(t));
}

reg_t Compiler::expression(const Tree &t) {
  switch(t.token()) {
  case TOK_VARIABLE:
    {
      reg_t sp = getStackRegister();
      rhs(t);
      restoreStack(sp,1);
      return popStack();
    }
  case TOK_REAL:
  case TOK_IMAG:
  case TOK_REALF:
  case TOK_IMAGF:
  case TOK_STRING:
    {
      reg_t r0 = getRegister();
      emit(OP_LOAD_CONST,r0,getConstantID(t.array()));
      return r0;
    }
  case TOK_END:
    // Rewrite the tree to remove TOK_END...
    // if (!endRef.valid())
    //   throw Exception("END keyword not allowed for undefined variables");
    // if (endTotal == 1)
    //   return Array(double(endRef->length()));
    // else
    //   return Array(double(endRef->dimensions()[endCount]));
    break;
  case ':':
    if (t.numChildren() == 0) {
      return fetchConstantString(":");
    } else if (t.first().is(':')) {
      return doubleColon(t);
    } else {
      return doBinaryOperator(t,OP_COLON);
    }
    break;
  case TOK_MATDEF:
    //    return matrixDefinition(t);
    break;
  case TOK_CELLDEF:
    //    return cellDefinition(t);
    break;
  case '+':
    return doBinaryOperator(t,OP_ADD);
    break;
  case '-':
    return doBinaryOperator(t,OP_MINUS);
    break;
  case '*':
    return doBinaryOperator(t,OP_MTIMES);
    break;
  case '/':
    return doBinaryOperator(t,OP_MRDIVIDE);
    break;
  case '\\':
    return doBinaryOperator(t,OP_MLDIVIDE);
    break;
  case TOK_SOR:
    return ShortCutOr(t);
    break;
  case '|':
    return doBinaryOperator(t,OP_OR);
    break;
  case TOK_SAND:
    return ShortCutAnd(t);
    break;
  case '&':
    return doBinaryOperator(t,OP_AND);
  case '<':
    return doBinaryOperator(t,OP_LT);
    break;
  case TOK_LE:
    return doBinaryOperator(t,OP_LE);
    break;
  case '>':
    return doBinaryOperator(t,OP_GT);
    break;
  case TOK_GE:
    return doBinaryOperator(t,OP_GE);
    break;
  case TOK_EQ:
    return doBinaryOperator(t,OP_EQ);
    break;
  case TOK_NE:
    return doBinaryOperator(t,OP_NE);
    break;
  case TOK_DOTTIMES:
    return doBinaryOperator(t,OP_TIMES);
    break;
  case TOK_DOTRDIV:
    return doBinaryOperator(t,OP_RDIVIDE);
    break;
  case TOK_DOTLDIV:
    return doBinaryOperator(t,OP_LDIVIDE);
    break;
  case TOK_UNARY_MINUS:
    return doUnaryOperator(t,OP_MINUS);
    break;
  case TOK_UNARY_PLUS:
    return doUnaryOperator(t,OP_PLUS);
    break;
  case '~':
    return doUnaryOperator(t,OP_NOT);
    break;
  case '^':
    return doBinaryOperator(t,OP_POWER);
    break;
  case TOK_DOTPOWER:
    return doBinaryOperator(t,OP_DOTPOWER);
    break;
  case '\'':
    return doUnaryOperator(t,OP_HERMITIAN);
    break;
  case TOK_DOTTRANSPOSE:
    return doUnaryOperator(t,OP_TRANSPOSE);
    break;
  case '@':
    //    return FunctionPointer(t);
  default:
    throw Exception("Unrecognized expression!");
  }  
}

void Compiler::multiFunctionCall(const Tree & t, bool printIt) {
  if (!t.first().is(TOK_BRACKETS))
    throw Exception("Illegal left hand side in multifunction expression");
  TreeList s = t.first().children();
  reg_t lhsCount = getRegister();
  for (int ind=0;ind<s.size();ind++)
    {
      const Tree &t = s[ind];
      FMString varname = t.first().text();
      if (!is_global_variable(varname))
	createLocalVariable(varname);
      if (t.numChildren() == 1 || t.last.is(TOK_PARENS))
	emit(OP_INCR,lhsCount);
      else
	{
	  reg_t sp = getStackRegister();
	  flattenDereferenceTreeToStack(t,1);
	  if (is_local_variable(varname))
	    emit(OP_LHSCOUNT_LOCAL,lhsCount,sp,getLocalVariableId(varname));
	  else
	    emit(OP_LHSCOUNT_GLOBAL,lhsCount,sp,getGlobalVariableId(varname));
	}
    }
  // Make the function call here

}


void Compiler::statementType(const Tree &t) {
  switch(t.token()) {
  case '=':
    {
      reg_t r0 = expression(t.second());
      assignment(t.first(),printIt,r0);
      freeRegister(r0);
    }
    break;
  case TOK_MULTI:
    multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    //    specialFunctionCall(t,printIt);
    break;
  case TOK_FOR:
    //    forStatement(t);
    break;
  case TOK_WHILE:
    //    whileStatement(t);
    break;
  case TOK_IF:
    //    ifStatement(t);
    break;
  case TOK_BREAK:
    //    breakStatement();
    break;
  case TOK_CONTINUE:
    //    continueStatement();
    break;
  case TOK_DBSTEP:
    //    dbstepStatement(t);
    break;
  case TOK_DBTRACE:
    //    dbtraceStatement(t);
    break;
  case TOK_DBUP:
    // dbupStatement(t);
    break;
  case TOK_DBDOWN:
    //dbdown(t);
    break;
  case TOK_RETURN:
    //returnStatement();
    break;
  case TOK_SWITCH:
    //switchStatement(t);
    break;
  case TOK_TRY:
    //tryStatement(t);
    break;
  case TOK_QUIT:
    //quitStatement();
    break;
  case TOK_RETALL:
    //retallStatement();
    break;
  case TOK_KEYBOARD:
    //doDebugCycle();
    break;
  case TOK_GLOBAL:
    //globalStatement(t);
    break;
  case TOK_PERSISTENT:
    //persistentStatement(t);
    break;
  case TOK_EXPR:
    //expressionStatement(t,printIt);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }  
}

void Compiler::statement(const Tree &t) {
  if (t.is(TOK_QSTATEMENT))
    statementType(t.first(),false);
  else if (t.is(TOK_STATEMENT))
    statementType(t.first(),true);
}

void Compiler::block(const Tree &t) {
  const TreeList statements(t.children());
  for (TreeList::const_iterator i=statements.begin(); i!= statements.end(); ++i)
    statement(*i);
}
