/*
 * Copyright (c) 2009 Samit Basu
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

#include "Interpreter.hpp"
#include "Array.hpp"
#include "PathSearch.hpp"

extern FMString GetRootPath();

//Signature
//sfunction help HelpFunction
//inputs function
//outputs none
//DOCBLOCK freemat_help
ArrayVector HelpFunction(int nargout, const ArrayVector& arg, Interpreter* eval)
{
  PathSearcher psearch(GetRootPath()+"/help/text");

  if (arg.size() != 1)
    throw Exception("help function requires a single argument (the function or script name)");
  FMString fname = arg[0].asString();
  bool mdcexists = !(psearch.ResolvePath(fname+".mdc").size() == 0);
  bool isFun;
  FuncPtr val;
  isFun = eval->getContext()->lookupFunction(fname,val);
  if (isFun && (val->type() == FM_M_FUNCTION) && !mdcexists) {
    MFunctionDef *mptr;
    mptr = (MFunctionDef *) val;
    mptr->updateCode(eval);
    for (int i=0;i<(int)mptr->helpText.size();i++)
      eval->outputMessage(mptr->helpText[i]);
    return ArrayVector();
  } else {
    // Check for a mdc file with the given name
    FMString mdcname = fname + ".mdc";
    mdcname = psearch.ResolvePath(mdcname);
    if( mdcname.size() == 0 )
      throw Exception("no help available on " + fname);
    
    FMFile fp(mdcname);
    if (!fp.open("r"))
      throw Exception(FMString("No help available on ") + fname);
    while (!fp.atEnd()) {
      FMString cp = fp.readLine();
      eval->outputMessage("\n       ");
      eval->outputMessage(cp);
    }
    return ArrayVector();
  }
  throw Exception("no help for that topic");
}

