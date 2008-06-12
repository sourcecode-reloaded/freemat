/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <math.h>
#include <QtCore>
#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Math.hpp"
#include "Malloc.hpp"
#include "SingularValueDecompose.hpp"
#include "QRDecompose.hpp"
#include "System.hpp"
#include "Sparse.hpp"
#include "LUDecompose.hpp"
#include "Class.hpp"
#include "Print.hpp"
#include "MemPtr.hpp"
#include "Parser.hpp"
#include "List.hpp"

#include <algorithm>
#undef max
#undef min




//!
//@Module SIMKEYS Simulate Keypresses from the User
//@@Section FREEMAT
//@@Usage
//This routine simulates keystrokes from the user on FreeMat.
//The general syntax for its use is
//@[
//   otext = simkeys(text)
//@]
//where @|text| is a string to simulate as input to the console.
//The output of the commands are captured and returned in the 
//string @|otext|.  This is primarily used by the testing 
//infrastructure.
//!
ArrayVector SimKeysFunction(int nargout, const ArrayVector& arg,
			    Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("simkeys requires at least one argument (the cell array of strings to simulate)");
  eval->clearCaptureString();
  eval->setCaptureState(true);
  if (arg[0].dataClass() != FM_CELL_ARRAY)
    throw Exception("simkeys requires a cell array of strings");
  const Array *dp = (const Array *) arg[0].getDataPointer();
  for (int i=0;i<(int) arg[0].getLength();i++) {
    string txt(ArrayToString(dp[i]));
    if ((txt.size() > 0) && (txt[txt.size()-1] != '\n'))
      txt.push_back('\n');
    eval->ExecuteLine(txt);
  }
  eval->ExecuteLine("\nquit\n");
  try {
    while(1) 
      eval->evalCLI();
  } catch (InterpreterContinueException& e) {
  } catch (InterpreterBreakException& e) {
  } catch (InterpreterReturnException& e) {
  } catch (InterpreterRetallException& e) {
  } catch (InterpreterQuitException& e) {
  }
  eval->setCaptureState(false);
  return ArrayVector() << Array::stringConstructor(eval->getCaptureString());
}

//!
//@Module DIAG Diagonal Matrix Construction/Extraction
//@@Section ARRAY
//@@Usage
//The @|diag| function is used to either construct a 
//diagonal matrix from a vector, or return the diagonal
//elements of a matrix as a vector.  The general syntax
//for its use is
//@[
//  y = diag(x,n)
//@]
//If @|x| is a matrix, then @|y| returns the @|n|-th 
//diagonal.  If @|n| is omitted, it is assumed to be
//zero.  Conversely, if @|x| is a vector, then @|y|
//is a matrix with @|x| set to the @|n|-th diagonal.
//@@Examples
//Here is an example of @|diag| being used to extract
//a diagonal from a matrix.
//@<
//A = int32(10*rand(4,5))
//diag(A)
//diag(A,1)
//@>
//Here is an example of the second form of @|diag|, being
//used to construct a diagonal matrix.
//@<
//x = int32(10*rand(1,3))
//diag(x)
//diag(x,-1)
//@>
//@@Tests
//@{ test_diag1.m
//% Test the diagonal extraction function
//function test_val = test_diag1
//a = [1,2,3,4;5,6,7,8;9,10,11,12];
//b = diag(a);
//test_val = test(b == [1;6;11]);
//@}
//@{ test_diag2.m
//% Test the diagonal extraction function with a non-zero diagonal
//function test_val = test_diag2
//a = [1,2,3,4;5,6,7,8;9,10,11,12];
//b = diag(a,1);
//test_val = test(b == [2;7;12]);
//@}
//@{ test_diag3.m
//% Test the diagonal creation function
//function test_val = test_diag3
//a = [2,3];
//b = diag(a);
//test_val = test(b == [2,0;0,3]);
//@}
//@{ test_diag4.m
//% Test the diagonal creation function with a non-zero diagonal
//function test_val = test_diag4
//a = [2,3];
//b = diag(a,-1);
//test_val = test(b == [0,0,0;2,0,0;0,3,0]);
//@}
//@{ test_diag5.m
//% Test the diagonal creation function with no arguments (bug 1620051)
//function test_val = test_diag5
//test_val = 1;
//try
//  b = diag;
//catch
//  test_val = 1;
//end
//@}
//@@Tests
//@{ test_sparse74.m
//% Test sparse matrix array diagonal extraction
//function x = test_sparse74
//[yi1,zi1] = sparse_test_mat('int32',300,400);
//[yf1,zf1] = sparse_test_mat('float',300,400);
//[yd1,zd1] = sparse_test_mat('double',300,400);
//[yc1,zc1] = sparse_test_mat('complex',300,400);
//[yz1,zz1] = sparse_test_mat('dcomplex',300,400);
//x = testeq(diag(yi1,30),diag(zi1,30)) & testeq(diag(yf1,30),diag(zf1,30)) & testeq(diag(yd1,30),diag(zd1,30)) & testeq(diag(yc1,30),diag(zc1,30)) & testeq(diag(yz1,30),diag(zz1,30));
//@}
//!
ArrayVector DiagFunction(int nargout, const ArrayVector& arg) {
  Array a;
  Array b;
  Array c;
  ArrayVector retval;
  int32 *dp;
  int diagonalOrder;
  // First, get the diagonal order, and synthesize it if it was
  // not supplied
  if (arg.size() == 0)
    throw Exception("diag requires at least one argument.\n");
  if (arg.size() == 1) 
    diagonalOrder = 0;
  else {
    b = arg[1];
    if (!b.isScalar()) 
      throw Exception("second argument must be a scalar.\n");
    b.promoteType(FM_INT32);
    dp = (int32 *) b.getDataPointer();
    diagonalOrder = dp[0];
  }
  // Next, make sure the first argument is 2D
  a = arg[0];
  if (!a.is2D()) 
    throw Exception("First argument to 'diag' function must be 2D.\n");
  // Case 1 - if the number of columns in a is 1, then this is a diagonal
  // constructor call.
  if ((a.getDimensionLength(1) == 1) || (a.getDimensionLength(0) == 1))
    c = Array::diagonalConstructor(a,diagonalOrder);
  else
    c = a.getDiagonal(diagonalOrder);
  retval.push_back(c);
  return retval;
}


//!
//@Module SPONES Sparse Ones Function
//@@Section SPARSE
//@@Usage
//Returns a sparse @|float| matrix with ones where the argument
//matrix has nonzero values.  The general syntax for it is
//@[
//  y = spones(x)
//@]
//where @|x| is a matrix (it may be full or sparse).  The output
//matrix @|y| is the same size as @|x|, has type @|float|, and contains
//ones in the nonzero positions of @|x|.
//@@Examples
//Here are some examples of the @|spones| function
//@<
//a = [1,0,3,0,5;0,0,2,3,0;1,0,0,0,1]
//b = spones(a)
//full(b)
//@>
//!
ArrayVector SponesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("spones function requires a sparse matrix template argument");
  Array tmp(arg[0]);
  if (tmp.isEmpty())
    return SingleArrayVector(Array::emptyConstructor());
  if(tmp.isReferenceType())
    throw Exception("spones function requires a numeric sparse matrix argument");
  tmp.makeSparse();
  if (!tmp.sparse())
    throw Exception("spones function requires a sparse matrix template argument");
  return SingleArrayVector(Array::Array(FM_FLOAT,Dimensions(tmp.getDimensionLength(0),tmp.getDimensionLength(1)),SparseOnesFunc(tmp.dataClass(),tmp.getDimensionLength(0),tmp.getDimensionLength(1),tmp.getSparseDataPointer()),true));
}


//!
//@Module VERSION The Current Version Number
//@@Section FREEMAT
//@@Usage
//The @|version| function returns the current version number for
//FreeMat (as a string).  The general syntax for its use is
//@[
//    v = version
//@]
//@@Example
//The current version of FreeMat is
//@<
//version
//@>
//!
ArrayVector VersionFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector() << Array::stringConstructor(VERSION);
}


//!
//@Module VERSTRING The Current Version String
//@@Section FREEMAT
//@@Usage
//The @|verstring| function returns the current version string for
//FreeMat.  The general syntax for its use is
//@[
//    version = verstring
//@]
//@@Example
//The current version of FreeMat is
//@<
//verstring
//@>
//!
ArrayVector VerStringFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector() << Array::stringConstructor(Interpreter::getVersionString());
}


//!
//@Module DIARY Create a Log File of Console
//@@Section FREEMAT
//@@Usage
//The @|diary| function controls the creation of a log file that duplicates
//the text that would normally appear on the console.
//The simplest syntax for the command is simply:
//@[
//   diary
//@]
//which toggles the current state of the diary command.  You can also explicitly
//set the state of the diary command via the syntax
//@[
//   diary off
//@]
//or
//@[
//   diary on
//@]
//To specify a filename for the log (other than the default of @|diary|), you 
//can use the form:
//@[
//   diary filename
//@]
//or
//@[
//   diary('filename')
//@]
//which activates the diary with an output filename of @|filename|.  Note that the
//@|diary| command is thread specific, but that the output is appended to a given
//file.  That means that if you call @|diary| with the same filename on multiple 
//threads, their outputs will be intermingled in the log file (just as on the console).
//Because the @|diary| state is tied to individual threads, you cannot retrieve the
//current diary state using the @|get(0,'Diary')| syntax from MATLAB.  Instead, you
//must call the @|diary| function with no inputs and one output:
//@[
//   state = diary
//@]
//which returns a logical @|1| if the output of the current thread is currently going to
//a diary, and a logical @|0| if not.
//!
ArrayVector DiaryFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (nargout == 1) {
    if (arg.size() > 0)
      throw Exception("diary function with an assigned return value (i.e, 'x=diary') does not support any arguments");
    return ArrayVector() << Array::logicalConstructor(eval->getDiaryState());
  }
  if (arg.size() == 0) {
    eval->setDiaryState(!eval->getDiaryState());
    return ArrayVector();
  }
  string diaryString(ArrayToString(arg[0]));
  if (diaryString == "on")
    eval->setDiaryState(true);
  else if (diaryString == "off")
    eval->setDiaryState(false);
  else {
    eval->setDiaryFilename(diaryString);
    eval->setDiaryState(true);
  }
  return ArrayVector();
}



//!
//@Module QUIET Control the Verbosity of the Interpreter
//@@Section FREEMAT
//@@Usage
//The @|quiet| function controls how verbose the interpreter
//is when executing code.  The syntax for the function is
//@[
//   quiet flag
//@]
//where @|flag| is one of
//\begin{itemize}
//\item @|'normal'| - normal output from the interpreter
//\item @|'quiet'| - only intentional output (e.g. @|printf| calls and
//@|disp| calls) is printed.  The output of expressions that are not
//terminated in semicolons are not printed.
//\item @|'silent'| - nothing is printed to the output.
//\end{itemize}
//The @|quiet| command also returns the current quiet flag.
//!
ArrayVector QuietFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() > 0) {
    string qtype(arg[0].getContentsAsStringUpper());
    if (qtype == "NORMAL")
      eval->setQuietLevel(0);
    else if (qtype == "QUIET")
      eval->setQuietLevel(1);
    else if (qtype == "SILENT")
      eval->setQuietLevel(2);
    else
      throw Exception("quiet function takes one argument - the quiet level (normal, quiet, or silent) as a string");
  }
  string rtype;
  if (eval->getQuietLevel() == 0)
    rtype = "normal";
  else if (eval->getQuietLevel() == 1)
    rtype = "quiet";
  else if (eval->getQuietLevel() == 2)
    rtype = "silent";
  return ArrayVector() << Array::stringConstructor(rtype);
}

//!
//@Module SOURCE Execute an Arbitrary File
//@@Section FREEMAT
//@@Usage
//The @|source| function executes the contents of the given
//filename one line at a time (as if it had been typed at
//the @|-->| prompt).  The @|source| function syntax is
//@[
//  source(filename)
//@]
//where @|filename| is a @|string| containing the name of
//the file to process. 
//@@Example
//First, we write some commands to a file (note that it does
//not end in the usual @|.m| extension):
//@{ source_test
//a = 32;
//b = a;
//printf('a is %d and b is %d\n',a,b);
//@}
//Now we source the resulting file.
//@<
//clear a b
//source source_test
//@>
//@@Tests
//@{ source_test_script.m
//n = 1;
//n = n + 1;
//@}
//@{ test_source.m
//% Check that the source function does not double-execute the last line of the script
//function test_val = test_source
//myloc = which('test_source');
//[path,name,sfx] = fileparts(myloc);
//source([path '/source_test_script.m'])
//test_val = test(n == 2);
//@}
//!
ArrayVector SourceFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("source function takes exactly one argument - the filename of the script to execute");
  string filename = arg[0].getContentsAsString();
  QFile fp(QString::fromStdString(filename));
  if (!fp.open(QFile::ReadOnly))
    throw Exception(std::string("unable to open file ") + filename + " for reading");
  QTextStream fstr(&fp);
  QString scriptText(fstr.readAll());
  Scanner S(scriptText.toStdString(),filename);
  Parser P(S);
  CodeBlock pcode(P.process());
  if (pcode.tree()->is(TOK_FUNCTION_DEFS))
    throw Exception("only scripts can be source-ed, not functions");
  Tree *code = pcode.tree()->first();
  eval->block(code);
  return ArrayVector();
}


  

//!
//@Module LASTERR Retrieve Last Error Message
//@@Section FLOW
//@@Usage
//Either returns or sets the last error message.  The
//general syntax for its use is either
//@[
//  msg = lasterr
//@]
//which returns the last error message that occured, or
//@[
//  lasterr(msg)
//@]
//which sets the contents of the last error message.
//@@Example
//Here is an example of using the @|error| function to
//set the last error, and then retrieving it using
//lasterr.
//@<
//try; error('Test error message'); catch; end;
//lasterr
//@>
//Or equivalently, using the second form:
//@<
//lasterr('Test message');
//lasterr
//@>
//!
ArrayVector LasterrFunction(int nargout, const ArrayVector& arg,
			    Interpreter* eval) {
  ArrayVector retval;
  if (arg.size() == 0) {
    Array A = Array::stringConstructor(eval->getLastErrorString());
    retval.push_back(A);
  } else {
    Array tmp(arg[0]);
    eval->setLastErrorString(tmp.getContentsAsString());
  }
  return retval;
}


//!
//@Module ERRORCOUNT Retrieve the Error Counter for the Interpreter
//@@Section FREEMAT
//@@Usage
//This routine retrieves the internal counter for the interpreter,
//and resets it to zero.  The general syntax for its use is
//@[
//   count = errorcount
//@]
//!
ArrayVector ErrorCountFunction(int nargout, const ArrayVector& arg,
			       Interpreter* eval) {
  return ArrayVector() << Array::int32Constructor(eval->getErrorCount());
}

//!
//@Module WARNING Emits a Warning Message
//@@Section FLOW
//@@Usage
//The @|warning| function causes a warning message to be
//sent to the user.  The general syntax for its use is
//@[
//   warning(s)
//@]
//where @|s| is the string message containing the warning.
//!
ArrayVector WarningFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("Not enough inputs to warning function");
  if (!(arg[0].isString()))
    throw Exception("Input to error function must be a string");
  eval->warningMessage(arg[0].getContentsAsString());
  return ArrayVector();
}

//!
//@Module ERROR Causes an Error Condition Raised
//@@Section FLOW
//@@Usage
//The @|error| function causes an error condition (exception
//to be raised).  The general syntax for its use is
//@[
//   error(s),
//@]
//where @|s| is the string message describing the error.  The
//@|error| function is usually used in conjunction with @|try|
//and @|catch| to provide error handling.  If the string @|s|,
//then (to conform to the MATLAB API), @|error| does nothing.
//@@Example
//Here is a simple example of an @|error| being issued by a function
//@|evenoddtest|:
//@{ evenoddtest.m
//function evenoddtest(n)
//  if (n==0)
//    error('zero is neither even nor odd');
//  elseif ( n ~= fix(n) )
//    error('expecting integer argument');
//  end;
//  if (n==int32(n/2)*2)
//    printf('%d is even\n',n);
//  else
//    printf('%d is odd\n',n);
//  end
//@}
//The normal command line prompt @|-->| simply prints the error
//that occured.
//@<2
//evenoddtest(4)
//evenoddtest(5)
//evenoddtest(0)
//evenoddtest(pi)
//@>
//!
ArrayVector ErrorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    return ArrayVector();
  string etxt(ArrayToString(arg[0]));
  if (!etxt.empty()) 
    throw Exception(etxt);
  return ArrayVector();
}

//!
//@Module BUILTIN Evaulate Builtin Function
//@@Section FREEMAT
//@@Usage
//The @|builtin| function evaluates a built in function
//with the given name, bypassing any overloaded functions.
//The syntax of @|builtin| is
//@[
//  [y1,y2,...,yn] = builtin(fname,x1,x2,...,xm)
//@]
//where @|fname| is the name of the function to call.  Apart
//from the fact that @|fname| must be a string, and that @|builtin|
//always calls the non-overloaded method, it operates exactly like
//@|feval|.  Note that unlike MATLAB, @|builtin| does not force
//evaluation to an actual compiled function.  It simply subverts
//the activation of overloaded method calls.
//!
ArrayVector BuiltinFunction(int nargout, const ArrayVector& arg,Interpreter* eval){
  if (arg.size() == 0)
    throw Exception("builtin function requires at least one argument");
  if (!(arg[0].isString()))
    throw Exception("first argument to builtin must be the name of a function (i.e., a string)");
  FuncPtr funcDef;
  string fname = arg[0].getContentsAsString();
  Context *context = eval->getContext();
  if (!context->lookupFunction(fname,funcDef))
    throw Exception(std::string("function ") + fname + " undefined!");
  funcDef->updateCode(eval);
  if (funcDef->scriptFlag)
    throw Exception("cannot use feval on a script");
  ArrayVector newarg(arg);
  newarg.pop_front();
  bool flagsave = eval->getStopOverload();
  eval->setStopOverload(true);
  ArrayVector tmp(funcDef->evaluateFunction(eval,newarg,nargout));
  eval->setStopOverload(flagsave);
  return tmp;
}
  
  

//!
//@Module DOCLI Start a Command Line Interface
//@@Section FREEMAT
//@@Usage
//The @|docli| function is the main function that you interact with 
//when you run FreeMat.  I am not sure why you would want to use
//it, but hey - its there if you want to use it.
//!

//!
//@Module STARTUP Startup Script
//@@Section FREEMAT
//@@Usage
//Upon starting, FreeMat searches for a script names @|startup.m|, and
//if it finds it, it executes it.  This script can be in the current
//directory, or on the FreeMat path (set using @|setpath|).  The contents
//of startup.m must be a valid script (not a function).  
//!
ArrayVector DoCLIFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  Context *context = eval->getContext();
  FuncPtr funcDef;
  if (context->lookupFunction("startup",funcDef)) {
    funcDef->updateCode(eval);
    if (funcDef->scriptFlag) {
      try {
	eval->block(((MFunctionDef*)funcDef)->code.tree());
      } catch (Exception& e) {
	eval->errorMessage("Startup script error:\n" + e.getMessageCopy());
      }
    } else {
      eval->outputMessage(string("startup.m must be a script"));
    }
  }
  eval->doCLI();
  return ArrayVector();
}



//!
//@Module SYSTEM Call an External Program
//@@Section OS
//@@Usage
//The @|system| function allows you to call an external
//program from within FreeMat, and capture the output.
//The syntax of the @|system| function is
//@[
//  y = system(cmd)
//@]
//where @|cmd| is the command to execute.  The return
//array @|y| is of type @|cell-array|, where each entry
//in the array corresponds to a line from the output.
//@@Example
//Here is an example of calling the @|ls| function (the
//list files function under Un*x-like operating system).
//@<
//y = system('ls m*.m')
//y{1}
//@>
//!
ArrayVector SystemFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1) 
    throw Exception("System function takes one string argument");
  string systemArg = arg[0].getContentsAsString();
  ArrayVector retval;
  if (systemArg.size() == 0) 
    return retval;
  StringVector cp(DoSystemCallCaptured(systemArg));
  Array* np = new Array[cp.size()];
  for (int i=0;i<(int)cp.size();i++)
    np[i] = Array::stringConstructor(cp[i]);
  Dimensions dim(cp.size(),1);
  Array ret(FM_CELL_ARRAY,dim,np);
  retval.push_back(ret);
  return retval;
}





