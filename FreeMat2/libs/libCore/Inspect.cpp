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
#include "Malloc.hpp"
#include "PathSearch.hpp"
#include "IEEEFP.hpp"
#include "Sparse.hpp"
#include "helpwidget.hpp"
#include <algorithm>
#include "Editor.hpp"
#include "PathTool.hpp"
#include <QtCore>
#include <QtGui>
#include "Module.hpp"
#include "MemPtr.hpp"

void Tokenize(const std::string& str, StringVector& tokens,
	      const std::string& delimiters = " \n") {
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
    
  while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}


ArrayVector EndFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("End function requires 3 arguments, the array, the end index, and the number of subindexes");
  Dimensions t(arg[0].dimensions());
  int enddim(ArrayToInt32(arg[1]));
  int totalndxs(ArrayToInt32(arg[2]));
  if (totalndxs == 1)
    return SingleArrayVector(Array::int32Constructor(t.getElementCount()));
  return SingleArrayVector(Array::int32Constructor(t.get(enddim-1)));
}

//!
//@Module PATHTOOL Open Path Setting Tool
//@@Section FREEMAT
//@@Usage
//Brings up the pathtool dialog.  The @|pathtool| function takes
//no arguments:
//@[
//  pathtool
//@]
//!
ArrayVector PathToolFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  PathTool p;
  p.exec();
  QSettings settings("FreeMat","FreeMat");
  QStringList userPath = settings.value("interpreter/path").toStringList();
  eval->setUserPath(userPath);
  eval->rescanPath();
  return ArrayVector();
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
//!
ArrayVector WhoFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  int i;
  StringVector names;
  char buffer[1000];
  if (arg.size() == 0) {
    names = eval->getContext()->listAllVariables();
  } else {
    for (i=0;i<arg.size();i++) {
      Array varName(arg[i]);
      names.push_back(varName.getContentsAsString());
    }
  }
  std::sort(names.begin(),names.end());
  sprintf(buffer,"  Variable Name      Type   Flags             Size\n");
  eval->outputMessage(buffer);
  for (i=0;i<(int) names.size();i++) {
    Array lookup;
    ArrayReference ptr;
    sprintf(buffer,"%15s",names[i].c_str());
    eval->outputMessage(buffer);
    ptr = eval->getContext()->lookupVariable(names[i]);
    if (!ptr.valid())
      eval->outputMessage("   <undefined>");
    else {
      lookup = *ptr;
      Class t = lookup.dataClass();
      switch(t) {
      case FM_CELL_ARRAY:
	sprintf(buffer,"%10s","cell");
	break;
      case FM_STRUCT_ARRAY:
	if (lookup.isUserClass())
	  sprintf(buffer,"%10s",lookup.className().back().c_str());
	else
	  sprintf(buffer,"%10s","struct");
	break;
      case FM_LOGICAL:
	sprintf(buffer,"%10s","logical");
	break;
      case FM_UINT8:
	sprintf(buffer,"%10s","uint8");
	break;
      case FM_INT8:
	sprintf(buffer,"%10s","int8");
	break;
      case FM_UINT16:
	sprintf(buffer,"%10s","uint16");
	break;
      case FM_INT16:
	sprintf(buffer,"%10s","int16");
	break;
      case FM_UINT32:
	sprintf(buffer,"%10s","uint32");
	break;
      case FM_INT32:
	sprintf(buffer,"%10s","int32");
	break;
      case FM_UINT64:
	sprintf(buffer,"%10s","uint64");
	break;
      case FM_INT64:
	sprintf(buffer,"%10s","int64");
	break;
      case FM_FLOAT:
	sprintf(buffer,"%10s","float");
	break;
      case FM_DOUBLE:
	sprintf(buffer,"%10s","double");
	break;
      case FM_COMPLEX:
	sprintf(buffer,"%10s","complex");
	break;
      case FM_DCOMPLEX:
	sprintf(buffer,"%10s","dcomplex");
	break;
      case FM_STRING:
	sprintf(buffer,"%10s","string");
	break;
      case FM_FUNCPTR_ARRAY:
	sprintf(buffer,"%10s","func ptr");
	break;
      }
      eval->outputMessage(buffer);
      if (lookup.sparse())
	eval->outputMessage("   sparse");
      else
	eval->outputMessage("         ");	  
      if (eval->getContext()->isVariableGlobal(names[i])) {
	sprintf(buffer,"  global ");
	eval->outputMessage(buffer);
      } else if (eval->getContext()->isVariablePersistent(names[i])) {
	sprintf(buffer," persist ");
	eval->outputMessage(buffer);
      } else {
	sprintf(buffer,"         ");
	eval->outputMessage(buffer);
      }
      eval->outputMessage("  ");
      lookup.dimensions().printMe(eval);
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
//!
ArrayVector FieldNamesFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  if (arg.size() < 1)
    throw Exception("fieldnames function requires at least one argument");
  Array a(arg[0]);
  if (a.dataClass() != FM_STRUCT_ARRAY) {
    Array ret(Array::emptyConstructor());
    ret.promoteType(FM_CELL_ARRAY);
    return SingleArrayVector(ret);
  }
  StringVector names(a.fieldNames());
  ArrayMatrix m;
  for (int i=0;i<names.size();i++)
    m.push_back(SingleArrayVector(Array::stringConstructor(names.at(i))));
  return SingleArrayVector(Array::cellConstructor(m));
}

//!
//@Module SIZE Size of a Variable
//@@Section INSPECTION
//@@Usage
//Returns the size of a variable.  There are two syntaxes for its
//use.  The first syntax returns the size of the array as a vector
//of integers, one integer for each dimension
//@[
//  [d1,d2,...,dn] = size(x)
//@]
//The other format returns the size of @|x| along a particular
//dimension:
//@[
//  d = size(x,n)
//@]
//where @|n| is the dimension along which to return the size.
//@@Example
//@<
//a = randn(23,12,5);
//size(a)
//@>
//Here is an example of the second form of @|size|.
//@<
//size(a,2)
//@>
//@@Tests
//@{ test_size1.m
//% Check the size function with an n-dim argument & one output
//function test_val = test_size1
//a = [];
//a(3,7,2) = 1.0;
//c = size(a);
//test_val = test(c(1) == 3) & test(c(2) == 7) & test(c(3) == 2);
//@}
//@{ test_size2.m
//% Check the size function with an n-dim argument & multiple outputs
//function test_val = test_size2
//a = [1,2,3;4,5,6];
//[c1,c2,c3] = size(a);
//test_val = test(c1 == 2) & test(c2 == 3) & test(c3 == 1);
//@}
//@{ test_size3.m
//% Check the size function with two arguments and one output
//function test_val = test_size3
//a = [1,2,3;4,5,6];
//n = size(a,2);
//test_val = test(n == 3);
//@}
//@{ test_size4.m
//% Check the size function with two arguments and two outputs
//function test_val = test_size4
//a = [1,2,3;4,5,6];
//test_val = 0;
//[c,d] = size(a,2);
//test_val = (c == 3);
//@}
//!
ArrayVector SizeFunction(int nargout, const ArrayVector& arg) {
  ArrayVector retval;
  if (arg.size() < 1)
    throw Exception("size function requires either one or two arguments");
  Dimensions sze;
  sze = arg[0].dimensions();
  if (arg.size() == 1) {
    if (nargout > 1) {
      ArrayVector retval;
      for (int i=0;i<nargout;i++)
	retval.push_back(Array::uint32Constructor(sze.get(i)));
      return retval;
    } else {
      uint32 *dims = (uint32 *) Malloc(sizeof(uint32)*sze.getLength());
      for (int i=0;i<(int)sze.getLength();i++)
	dims[i] = sze.get(i);
      Array ret = Array(FM_UINT32,Dimensions(1,sze.getLength()),dims);
      retval.push_back(ret);
      return retval;
    } 
  }
  Array tmp(arg[1]);
  int dimval = tmp.getContentsAsIntegerScalar();
  if (dimval<1)
    throw Exception("illegal value for dimension argument in call to size function");
  retval.push_back(Array::uint32Constructor(sze.get(dimval-1)));
  return retval;
}

//   ArrayVector LengthFunction(int nargout, const ArrayVector& arg) {
//     Array A(Array::int32Constructor(arg[0].dimensions().getMax()));
//     ArrayVector retval;
//     retval.push_back(A);
//     return retval;
//   }


    
  

//!
//@Module NNZ Number of Nonzeros
//@@Section SPARSE
//@@Usage
//Returns the number of nonzero elements in a matrix.
//The general format for its use is
//@[
//   y = nnz(x)
//@]
//This function returns the number of nonzero elements
//in a matrix or array.  This function works for both
//sparse and non-sparse arrays.  For 
//@@Example
//@<
//a = [1,0,0,5;0,3,2,0]
//nnz(a)
//A = sparse(a)
//nnz(A)
//@>
//!
ArrayVector NNZFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("nnz function takes one argument - the array");
  Array tmp(arg[0]);
  return SingleArrayVector(Array::int32Constructor(tmp.nnz()));
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
//!
ArrayVector WhereFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  //    eval->stackTrace(false);
  eval->stackTrace(true);
  return ArrayVector();
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
//!
ArrayVector WhichFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("which function takes one string argument (the name of the function to look up)");
  string fname = arg[0].getContentsAsString();
  bool isFun;
  FuncPtr val;
  isFun = eval->lookupFunction(fname,val);
  char buffer[1000];
  Array ret(Array::emptyConstructor());
  if (isFun) {
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      mptr->updateCode(eval);
      if (mptr->pcodeFunction) {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, P-code script\n",fname.c_str());
	    eval->outputMessage(buffer);
	  }
	} else {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, P-code function\n",fname.c_str());
	    eval->outputMessage(buffer);
	  }
	}
      } else {
	if (mptr->scriptFlag) {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, M-File script in file '%s'\n",fname.c_str(),
		    mptr->fileName.c_str());
	    eval->outputMessage(buffer);
	  } else 
	    ret = Array::stringConstructor(mptr->fileName);
	} else {
	  if (nargout == 0) {
	    sprintf(buffer,"Function %s, M-File function in file '%s'\n",
		    fname.c_str(),mptr->fileName.c_str());
	    eval->outputMessage(buffer);
	  } else
	    ret = Array::stringConstructor(mptr->fileName);
	}
      }
    } else if ((val->type() == FM_BUILT_IN_FUNCTION) || (val->type() == FM_SPECIAL_FUNCTION) ) {
      if (nargout == 0) {
	sprintf(buffer,"Function %s is a built in function\n",fname.c_str());
	eval->outputMessage(buffer);
      }
    } else {
      if (nargout == 0) {
	sprintf(buffer,"Function %s is an imported function\n",fname.c_str());
	eval->outputMessage(buffer);
      }
    }
  } else {
    if (nargout == 0) {
      sprintf(buffer,"Function %s is unknown!\n",fname.c_str());
      eval->outputMessage(buffer);
    }
  }
  if (nargout > 0)
    return SingleArrayVector(ret);
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
//!

static std::string fname_only(std::string name) {
  int ndx;
  ndx = name.rfind("/");
  if (ndx>=0)
    name.erase(0,ndx+1);
  ndx = name.rfind(".");
  if (ndx>=0)
    name.erase(ndx,name.size());
  return name;
}

ArrayVector MFilenameFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return SingleArrayVector(Array::stringConstructor(fname_only(eval->getMFileName())));
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
//!
ArrayVector ComputerFunction(int nargout, const ArrayVector& arg) {
#ifdef WIN32
  return SingleArrayVector(Array::stringConstructor("PCWIN"));
#elif defined(__APPLE__)
  return SingleArrayVector(Array::stringConstructor("MAC"));
#else
  return SingleArrayVector(Array::stringConstructor("UNIX"));
#endif
}

