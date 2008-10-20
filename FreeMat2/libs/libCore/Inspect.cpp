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

#include "Array.hpp"
#include "Interpreter.hpp"
#include <QtCore>
#include "Algorithms.hpp"

//!
//@Module END End Function
//@@Section INSPECTION
//@@Usage
//Computes the size of a variable along a given dimension.  The syntax
//for its use is 
//@[
//   y = end(x,dim,subindexes)
//@]
//where @|x| is the array to be analyzed, @|dim| is the dimension along
//which to compute the end, and @|subindexes| indicates how many dimensions
//are involved in the @|end| calculation.
//@@Signature
//function p_end EndFunction
//inputs x dim subindexes
//outputs y
//!
ArrayVector EndFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("End function requires 3 arguments, the array, the end index, and the number of subindexes");
  NTuple t(arg[0].dimensions());
  index_t enddim(arg[1].asDouble());
  index_t totalndxs(arg[2].asDouble());
  if (totalndxs == 1)
    return ArrayVector(Array(index_t(arg[0].length())));
  return ArrayVector(Array(index_t(t[int(enddim-1)])));
}

//!
//@Module WHO Describe Currently Defined Variables
//@@Section INSPECTION
//@@Usage
//Reports information on either all variables in the current context
//or on a specified set of variables.  For each variable, the @|who|
//function indicates the size and type of the variable as well as 
//if it is a global or persistent.  There are two formats for the 
//function call.  The first is the explicit form, in which a list
//of variables are provided:
//@[
//  who a1 a2 ...
//@]
//In the second form
//@[
//  who
//@]
//the @|who| function lists all variables defined in the current 
//context (as well as global and persistent variables). Note that
//there are two alternate forms for calling the @|who| function:
//@[
//  who 'a1' 'a2' ...
//@]
//and
//@[
//  who('a1','a2',...)
//@]
//@@Example
//Here is an example of the general use of @|who|, which lists all of the variables defined.
//@<
//c = [1,2,3];
//f = 'hello';
//p = randn(1,256);
//who
//@>
//In the second case, we examine only a specific variable:
//@<
//who c
//who('c')
//@>
//@@Signature
//sfunction who WhoFunction
//inputs varargin
//outputs none
//!
ArrayVector WhoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector names;
  if (arg.size() == 0)
    names = eval->getContext()->listAllVariables();
  else
    for (int i=0;i<arg.size();i++)
      names.push_back(arg[i].asString());
  qSort(names.begin(),names.end());
  eval->outputMessage("  Variable Name       Type   Flags             Size\n");
  for (int i=0;i<names.size();i++) {
    Array lookup;
    ArrayReference ptr;
    eval->outputMessage(names[i].rightJustified(15,' ',false));
    ptr = eval->getContext()->lookupVariable(names[i]);
    if (!ptr.valid())
      eval->outputMessage("   <undefined>");
    else {
      lookup = *ptr;
      eval->outputMessage(lookup.className().rightJustified(10,' ',false));
      if (lookup.isSparse())
	eval->outputMessage("   sparse");
      else
	eval->outputMessage("         ");	  
      if (eval->getContext()->isVariableGlobal(names[i])) {
	eval->outputMessage("  global ");
      } else if (eval->getContext()->isVariablePersistent(names[i])) {
	eval->outputMessage(" persist ");
      } else {
	eval->outputMessage("         ");
      }
      eval->outputMessage(QString("  [") + 
			  lookup.dimensions().toString() + 
			  QString("]"));
    }
    eval->outputMessage("\n");
  }
  return ArrayVector();
}

//!
//@Module WHOS Describe Currently Defined Variables With Memory Usage
//@@Section INSPECTION
//@@Usage
//Reports information on either all variables in the current context
//or on a specified set of variables.  For each variable, the @|who|
//function indicates the size and type of the variable as well as 
//if it is a global or persistent.  There are two formats for the 
//function call.  The first is the explicit form, in which a list
//of variables are provided:
//@[
//  whos a1 a2 ...
//@]
//In the second form
//@[
//  whos
//@]
//the @|whos| function lists all variables defined in the current 
//context (as well as global and persistent variables). Note that
//there are two alternate forms for calling the @|whos| function:
//@[
//  whos 'a1' 'a2' ...
//@]
//and
//@[
//  whos('a1','a2',...)
//@]
//@@Signature
//sfunction whos WhosFunction
//inputs varargin
//outputs none
//!
ArrayVector WhosFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  StringVector names;
  if (arg.size() == 0)
    names = eval->getContext()->listAllVariables();
  else
    for (int i=0;i<arg.size();i++)
      names.push_back(arg[i].asString());
  qSort(names.begin(),names.end());
  eval->outputMessage("  Variable Name       Type   Flags             Size       Bytes\n");
  for (int i=0;i<names.size();i++) {
    Array lookup;
    ArrayReference ptr;
    eval->outputMessage(names[i].rightJustified(15,' ',false));
    ptr = eval->getContext()->lookupVariable(names[i]);
    if (!ptr.valid())
      eval->outputMessage("   <undefined>");
    else {
      lookup = *ptr;
      eval->outputMessage(lookup.className().rightJustified(10,' ',false));
      if (lookup.isSparse())
	eval->outputMessage("   sparse");
      else
	eval->outputMessage("         ");	  
      if (eval->getContext()->isVariableGlobal(names[i])) {
	eval->outputMessage("  global ");
      } else if (eval->getContext()->isVariablePersistent(names[i])) {
	eval->outputMessage(" persist ");
      } else {
	eval->outputMessage("         ");
      }
      QString txt(QString("  [") + 
		  lookup.dimensions().toString() + 
		  QString("]"));
      eval->outputMessage(txt.leftJustified(15,' ',false));
      eval->outputMessage(QString("   %1").arg(lookup.bytes()));
    }
    eval->outputMessage("\n");
  }
  return ArrayVector();
}


//!
//@Module FIELDNAMES Fieldnames of a Structure
//@@Section INSPECTION
//@@Usage
//Returns a cell array containing the names of the fields in
//a structure array.  The syntax for its use is
//@[
//   x = fieldnames(y)
//@]
//where @|y| is a structure array of object array.  The result
//is a cell array, with one entry per field in @|y|.
//@@Example
//We define a simple structure array:
//@<
//y.foo = 3; y.goo = 'hello';
//x = fieldnames(y)
//@>
//@@Signature
//function fieldnames FieldNamesFunction
//inputs y
//outputs x
//!
ArrayVector FieldNamesFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("fieldnames function requires at least one argument");
  if (arg[0].dataClass() != Struct)
    return ArrayVector(Array(CellArray,NTuple(0,0)));
  StringVector names(FieldNames(arg[0]));
  ArrayMatrix m;
  for (int i=0;i<names.size();i++)
    m.push_back(ArrayVector(Array(names.at(i))));
  return ArrayVector(CellConstructor(m));
}


//!
//@Module WHERE Get Information on Program Stack
//@@Section INSPECTION
//@@Usage
//Returns information on the current stack.  The usage is
//@[
//   where
//@]
//The result is a kind of stack trace that indicates the state
//of the current call stack, and where you are relative to the
//stack.
//@@Example
//Suppose we have the following chain of functions.
//@{ chain1.m
//function chain1
//  a = 32;
//  b = a + 5;
//  chain2(b)
//@}
//@{ chain2.m
//function chain2(d)
//  d = d + 5;
//  chain3
//@}
//@{ chain3.m
//function chain3
//  g = 54;
//  f = g + 1;
//  keyboard
//@}
//The execution of the @|where| command shows the stack trace.
//@<
//chain1
//where
//@>
//@@Signature
//sfunction where WhereFunction
//inputs none
//outputs none
//!
ArrayVector WhereFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  eval->stackTrace(true);
  return ArrayVector();
}

ArrayVector AddrFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(arg[0].address()));
}

//!
//@Module WHICH Get Information on Function
//@@Section INSPECTION
//@@Usage
//Returns information on a function (if defined).  The usage is
//@[
//   which(fname)
//@]
//where @|fname| is a @|string| argument that contains the name of the 
//function.  For functions and scripts defined
//via @|.m| files, the @|which| command returns the location of the source
//file:
//@[
//   y = which(fname)
//@]
//will return the filename for the @|.m| file corresponding to the given
//function, and an empty string otherwise.
//@@Example
//First, we apply the @|which| command to a built in function.
//@<
//which fft
//@>
//Next, we apply it to a function defined via a @|.m| file.
//@<
//which fliplr
//@>
//@@Signature
//sfunction which WhichFunction
//inputs functionname
//outputs location
//!
ArrayVector WhichFunction(int nargout, const ArrayVector& arg, 
			  Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("which function takes one string argument (the name of the function to look up)");
  QString fname = arg[0].asString();
  bool isFun;
  FuncPtr val;
  isFun = eval->lookupFunction(fname,val);
  Array ret(Double,NTuple(0,0));
  if (isFun) {
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      mptr->updateCode(eval);
      if (mptr->pcodeFunction) {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", P-code script\n");
	  }
	} else {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", P-code function\n");
	  }
	}
      } else {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", M-File script in file '"+mptr->fileName+"'\n");
	  } else 
	    ret = Array(mptr->fileName);
	} else {
	  if (nargout == 0) {
	    eval->outputMessage("Function "+fname+", M-File function in file '"+mptr->fileName+"'\n");
	  } else
	    ret = Array(mptr->fileName);
	}
      }
    } else if ((val->type() == FM_BUILT_IN_FUNCTION) || 
	       (val->type() == FM_SPECIAL_FUNCTION) ) {
      if (nargout == 0) {
	eval->outputMessage("Function "+fname+" is a built in function\n");
      }
    } else {
      if (nargout == 0) {
	eval->outputMessage("Function "+fname+" is an imported function\n");
      }
    }
  } else {
    if (nargout == 0) {
      eval->outputMessage("Function "+fname+" is unknown!\n");
    }
  }
  if (nargout > 0)
    return ArrayVector(ret);
  else
    return ArrayVector();
}


//!
//@Module MFILENAME Name of Current Function
//@@Section FreeMat
//@@Usage
//Returns a string describing the name of the current function.  For M-files
//this string will be the complete filename of the function.  This is true even
//for subfunctions.  The syntax for its use is
//@[
//    y = mfilename
//@]
//@@Signature
//sfunction mfilename MFilenameFunction
//inputs none
//outputs filename
//!
ArrayVector MFilenameFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(QFileInfo(eval->getMFileName()).fileName()));
}

//!
//@Module COMPUTER Computer System FreeMat is Running On
//@@Section FreeMat
//@@Usage
//Returns a string describing the name of the system FreeMat is running on.
//The exact value of this string is subject to change, although the @|'MAC'|
//and @|'PCWIN'| values are probably fixed.
//@[
//  str = computer
//@]
//Currently, the following return values are defined
//\begin{itemize}
//  \item @|'PCWIN'| - MS Windows
//  \item @|'MAC'| - Mac OS X
//  \item @|'UNIX'| - All others
//\end{itemize}
//@@Signature
//function computer ComputerFunction
//inputs none
//outputs str
//!
ArrayVector ComputerFunction(int nargout, const ArrayVector& arg) {
#ifdef WIN32
  return ArrayVector(Array(QString("PCWIN")));
#elif defined(__APPLE__)
  return ArrayVector(Array(QString("MAC")));
#else
  return ArrayVector(Array(QString("UNIX")));
#endif
}

