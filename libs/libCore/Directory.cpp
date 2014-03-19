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
#include "Utils.hpp"
#include <stdio.h>
#include "System.hpp"
#include "Algorithms.hpp"

//@@Signature
//sfunction cd ChangeDirFunction
//inputs path
//outputs none
//DOCBLOCK os_cd
ArrayVector ChangeDirFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("cd function requires exactly one argument");
  eval->changeDir(TildeExpand(arg[0].asString()));
  return ArrayVector();
}

static void TabledOutput(StringVector sysresult, Interpreter* eval) {
  int maxlen = 0;
  // Find the maximal length
  for (int i=0;i<(int)sysresult.size();i++) {
    int ellen(sysresult[i].size());
    maxlen = (maxlen < ellen) ? ellen : maxlen;
  }
  // Calculate the number of columns that fit..
  int outcolumns;
  int termwidth = eval->getTerminalWidth()-1;
  outcolumns = termwidth/(maxlen+1);
  if (outcolumns < 1) outcolumns = 1;
  int colwidth = termwidth/outcolumns;
  int entryCount = 0;
  while (entryCount < (int)sysresult.size()) {
    char buffer[4096];
    sprintf(buffer,"%s",qPrintable(sysresult[entryCount]));
    int wlen;
    wlen = strlen(buffer);
    for (int j=wlen;j<colwidth;j++)
      buffer[j] = ' ';
    buffer[colwidth] = 0;
    eval->outputMessage(buffer);
    entryCount++;
    if (entryCount % outcolumns == 0)
      eval->outputMessage("\n");
  }
  eval->outputMessage("\n");
}

//@@Signature
//sfunction what WhatFunction
//inputs folder
//outputs none
//DOCBLOCK inspection_what
ArrayVector WhatFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  FMDir pdir(FMDir::current());
  boost::filesystem::directory_iterator p(pdir.boostPath());
  StringVector mFiles;
  StringVector matFiles;
  StringVector classFiles;
  while (p != boost::filesystem::directory_iterator())
    {
      FMString ext = p->path().extension().string();
      FMString filename = p->path().filename().string();
      FMString stem = p->path().stem().string();
      if (ext == ".m" || ext == ".M")
	mFiles.push_back(stem);
      else if (ext == ".mat" || ext == ".MAT")
	matFiles.push_back(stem);
      else if (filename.startsWith("@"))
	{
	  stem = stem.right(stem.size()-1);
	  classFiles.push_back(stem);
	}
      ++p;
    }
  eval->outputMessage("\n");
  if (mFiles.size() > 0)
    {
      eval->outputMessage(FMString("M-Files in directory ") + pdir.currentPath() + "\n\n");
      TabledOutput(mFiles,eval);
      eval->outputMessage("\n");
    }
  if (matFiles.size() > 0)
    {
      eval->outputMessage(FMString("MAT-Files in directory ") + pdir.currentPath() + "\n\n");
      TabledOutput(matFiles,eval);
      eval->outputMessage("\n");
    }
  if (classFiles.size() > 0)
    {
      eval->outputMessage(FMString("Classes in directory ") + pdir.currentPath() + "\n\n");
      TabledOutput(classFiles,eval);
      eval->outputMessage("\n");
    }
  return ArrayVector();
}


//@@Signature
//function dirsep DirSepFunction
//inputs none
//outputs y
//DOCBLOCK os_dirsep
ArrayVector DirSepFunction(int nargout, const ArrayVector& arg) {
  boost::filesystem::path slash("/");
  FMString preferredSlash = slash.make_preferred().native();
  return ArrayVector(Array(preferredSlash));
}

//@@Signature
//function pwd PrintWorkingDirectoryFunction
//inputs none
//outputs y
//DOCBLOCK os_pwd
ArrayVector PrintWorkingDirectoryFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(FMString(boost::filesystem::current_path().make_preferred().native())));
}

