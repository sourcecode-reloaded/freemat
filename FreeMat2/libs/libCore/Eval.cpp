#include "Array.hpp"
#include "Interpreter.hpp"
#include "Algorithms.hpp"

//!
//@Module EVAL Evaluate a String
//@@Section FREEMAT
//@@Usage
//The @|eval| function evaluates a string.  The general syntax
//for its use is
//@[
//   eval(s)
//@]
//where @|s| is the string to evaluate.  If @|s| is an expression
//(instead of a set of statements), you can assign the output
//of the @|eval| call to one or more variables, via
//@[
//   x = eval(s)
//   [x,y,z] = eval(s)
//@]
//
//Another form of @|eval| allows you to specify an expression or
//set of statements to execute if an error occurs.  In this 
//form, the syntax for @|eval| is
//@[
//   eval(try_clause,catch_clause),
//@]
//or with return values,
//@[
//   x = eval(try_clause,catch_clause)
//   [x,y,z] = eval(try_clause,catch_clause)
//@]
//These later forms are useful for specifying defaults.  Note that
//both the @|try_clause| and @|catch_clause| must be expressions,
//as the equivalent code is
//@[
//  try
//    [x,y,z] = try_clause
//  catch
//    [x,y,z] = catch_clause
//  end
//@]
//so that the assignment must make sense in both cases.
//@@Example
//Here are some examples of @|eval| being used.
//@<
//eval('a = 32')
//b = eval('a')
//@>
//The primary use of the @|eval| statement is to enable construction
//of expressions at run time.
//@<
//s = ['b = a' ' + 2']
//eval(s)
//@>
//Here we demonstrate the use of the catch-clause to provide a 
//default value
//@<
//a = 32
//b = eval('a','1')
//b = eval('z','a+1')
//@>
//Note that in the second case, @|b| takes the value of 33, indicating
//that the evaluation of the first expression failed (because @|z| is
//not defined).
//@@Signature
//sfunction eval EvalFunction
//inputs try_clause catch_clause
//outputs varargout
//!
static QString PrePendCallVars(QString line, int nargout) {
  QString gp;
  if (nargout > 1)
    gp += "[";
  for (int i=0;i<nargout-1;i++)
    gp += QString("t___%1,").arg(i);
  gp += QString("t___%1").arg(nargout-1);
  if (nargout > 1)
    gp += "] = " + line + ";\n";
  else
    gp += " = " + line + ";\n";
  return gp;
}

static ArrayVector RetrieveCallVars(Interpreter *eval, int nargout) {
  ArrayVector retval;
  for (int i=0;i<nargout;i++) {
    QString tname = QString("t___%1").arg(i);
    Array tval;
    ArrayReference ptr = eval->getContext()->lookupVariable(tname);
    if (!ptr.valid())
      tval = EmptyConstructor();
    else
      tval = *ptr;
    eval->getContext()->deleteVariable(tname);
    retval.push_back(tval);
  }
  return retval;
}

static ArrayVector EvalTryFunction(int nargout, const ArrayVector& arg, Interpreter* eval, int popSpec) {
  if (nargout > 0) {
    QString try_line = arg[0].asString();
    QString try_buf = PrePendCallVars(try_line,nargout);
    QString catch_line = arg[1].asString();
    QString catch_buf = PrePendCallVars(catch_line,nargout);
    ArrayVector retval;
    bool autostop;
    autostop = eval->AutoStop();
    eval->setAutoStop(false);
    try {
      eval->getContext()->bypassScope(popSpec);
      eval->evaluateString(try_buf,true);
      retval = RetrieveCallVars(eval,nargout);
      eval->getContext()->restoreBypassedScopes();
    } catch (Exception &e) {
      eval->getContext()->restoreBypassedScopes();
      eval->evaluateString(catch_buf,false);
      retval = RetrieveCallVars(eval,nargout);
    }
    eval->setAutoStop(autostop);
    return retval;
  } else {
    QString try_line = arg[0].asString();
    QString catch_line = arg[1].asString();
    QString try_buf = try_line + "\n";
    QString catch_buf = catch_line + "\n";
    bool autostop;
    autostop = eval->AutoStop();
    eval->setAutoStop(false);
    try {
      eval->getContext()->bypassScope(popSpec);
      eval->evaluateString(try_buf,true);
      eval->getContext()->restoreBypassedScopes();
    } catch (Exception &e) {
      eval->getContext()->restoreBypassedScopes();
      eval->evaluateString(catch_buf,false);
    }
    eval->setAutoStop(autostop);
    return ArrayVector();
  }
}

static ArrayVector EvalNoTryFunction(int nargout, const ArrayVector& arg, Interpreter* eval, int popSpec) {
  if (nargout > 0) {
    QString line = arg[0].asString();
    QString buf = PrePendCallVars(line,nargout);
    eval->getContext()->bypassScope(popSpec);
    eval->evaluateString(buf);
    ArrayVector retval(RetrieveCallVars(eval,nargout));
    eval->getContext()->restoreBypassedScopes();
    return retval;
  } else {
    QString line = arg[0].asString();
    QString buf = line + "\n";
    eval->getContext()->bypassScope(popSpec);
    eval->evaluateString(buf);
    eval->getContext()->restoreBypassedScopes();
    return ArrayVector();
  }
}

ArrayVector EvalFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("eval function takes at least one argument - the string to execute");
  if (arg.size() == 2)
    return EvalTryFunction(nargout, arg, eval, 0);
  return EvalNoTryFunction(nargout, arg, eval, 0);
}

//!
//@Module EVALIN Evaluate a String in Workspace
//@@Section FREEMAT
//@@Usage
//The @|evalin| function is similar to the @|eval| function, with an additional
//argument up front that indicates the workspace that the expressions are to 
//be evaluated in.  The various syntaxes for @|evalin| are:
//@[
//   evalin(workspace,expression)
//   x = evalin(workspace,expression)
//   [x,y,z] = evalin(workspace,expression)
//   evalin(workspace,try_clause,catch_clause)
//   x = evalin(workspace,try_clause,catch_clause)
//   [x,y,z] = evalin(workspace,try_clause,catch_clause)
//@]
//The argument @|workspace| must be either 'caller' or 'base'.  If it is
//'caller', then the expression is evaluated in the caller's work space.
//That does not mean the caller of @|evalin|, but the caller of the current
//function or script.  On the other hand if the argument is 'base', then
//the expression is evaluated in the base work space.   See @|eval| for
//details on the use of each variation.
//@@Signature
//sfunction evalin EvalInFunction
//inputs workspace expression
//outputs x y z
//!
ArrayVector EvalInFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 2)
    throw Exception("evalin function requires a workspace (scope) specifier (either 'caller' or 'base') and an expression to evaluate");
  QString spec_str = arg[0].asString();
  int popspec = 0;
  if (spec_str=="base")
    popspec = -1;
  else if (spec_str=="caller")
    popspec = 1;
  else
    throw Exception("evalin function requires the first argument to be either 'caller' or 'base'");
  ArrayVector argcopy(arg);
  argcopy.pop_front();
  if (arg.size() == 3)
    return EvalTryFunction(nargout,argcopy,eval,popspec);
  else
    return EvalNoTryFunction(nargout,argcopy,eval,popspec);
}

//!
//@Module ASSIGNIN Assign Variable in Workspace
//@@Section FREEMAT
//@@Usage
//The @|assignin| function allows you to assign a value to a variable
//in either the callers work space or the base work space.  The syntax
//for @|assignin| is
//@[
//   assignin(workspace,variablename,value)
//@]
//The argument @|workspace| must be either 'caller' or 'base'.  If it is
//'caller' then the variable is assigned in the caller's work space.
//That does not mean the caller of @|assignin|, but the caller of the
//current function or script.  On the other hand if the argument is 'base',
//then the assignment is done in the base work space.  Note that the
//variable is created if it does not already exist.
//@@Signature
//sfunction assignin AssignInFunction
//inputs workspace variablename value
//outputs none
//!
ArrayVector AssignInFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() < 3)
    throw Exception("assignin function requires a workspace (scope) specifier (either 'caller' or 'base') a variable name and a value to assign");
  QString spec_str = arg[0].asString();
  int popspec = 0;
  if (spec_str=="base")
    popspec = -1;
  else if (spec_str=="caller") 
    popspec = 1;
  else
    throw Exception("evalin function requires the first argument to be either 'caller' or 'base'");
  QString varname = arg[1].asString();
  Array varvalue = arg[2];
  eval->getContext()->bypassScope(popspec);
  eval->getContext()->insertVariable(varname,varvalue);
  eval->getContext()->restoreBypassedScopes();
  return ArrayVector();
}

//!
//@Module FEVAL Evaluate a Function
//@@Section FREEMAT
//@@Usage
//The @|feval| function executes a function using its name.
//The syntax of @|feval| is
//@[
//  [y1,y2,...,yn] = feval(f,x1,x2,...,xm)
//@]
//where @|f| is the name of the function to evaluate, and
//@|xi| are the arguments to the function, and @|yi| are the
//return values.
//
//Alternately, @|f| can be a function handle to a function
//(see the section on @|function handles| for more information).
//
//Finally, FreeMat also supports @|f| being a user defined class
//in which case it will atttempt to invoke the @|subsref| method
//of the class.
//@@Example
//Here is an example of using @|feval| to call the @|cos| 
//function indirectly.
//@<
//feval('cos',pi/4)
//@>
//Now, we call it through a function handle
//@<
//c = @cos
//feval(c,pi/4)
//@>
//Here we construct an inline object (which is a user-defined class)
//and use @|feval| to call it
//@<
//afunc = inline('cos(t)+sin(t)','t')
//feval(afunc,pi)
//afunc(pi)
//@>
//In both cases, (the @|feval| call and the direct invokation), FreeMat
//calls the @|subsref| method of the class, which computes the requested 
//function.
//@@Tests
//@$y1 = feval(@cos,x1)
//@$y1 = feval(inline('cos(t)'),x1)
//@{ test_feval1.m
//function test_val = test_feval1
//y = 0;
//test_val = feval('test_feval1_local_func',y);
//
//function z = test_feval1_local_func(x)
//z = 1;
//@}
//@@Signature
//sfunction feval FevalFunction
//inputs varargin
//outputs varargout
//!
ArrayVector FevalFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("feval function requires at least one argument");
  if (!arg[0].isString())
    throw Exception("first argument to feval must be the name of a function (i.e., a string) a function handle, or a user defined class");
  FuncPtr funcDef;
  if (arg[0].isString()) {
    QString fname = arg[0].asString();
    eval->popDebug();
    if (!eval->lookupFunction(fname,funcDef)) {
      eval->pushDebug("feval","feval");
      throw Exception(QString("function ") + fname + " undefined!");
    }
    eval->pushDebug("feval","feval");
  } else 
    throw Exception("argument to feval must be a string");
  funcDef->updateCode(eval);
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  return(funcDef->evaluateFunction(eval,newarg,nargout));
}
