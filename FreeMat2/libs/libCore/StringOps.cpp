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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "MemPtr.hpp"
#if HAVE_PCRE
#ifdef WIN32
#define PCRE_STATIC
#endif
#include <pcre.h>
#endif
#include <QtCore>
#include "Printf.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"

//!
//@Module STRCMP String Compare Function
//@@Section STRING
//@@Usage
//Compares two string, and returns @|true| if they equal
//and @|false| if not.
//@@Signature
//function strcmp StrCmpFunction
//inputs string1 string2
//outputs flag
//!
ArrayVector StrCmpFunction(int nargout, const ArrayVector& arg) {
  Array retval;
  if (arg.size() != 2)
    throw Exception("strcmp function requires two arguments");
  if (!(arg[0].isString()))
    return ArrayVector(Array(bool(false)));
  if (!(arg[1].isString()))
    return ArrayVector(Array(bool(false)));
  if (!(arg[0].dimensions() == arg[1].dimensions()))
    return ArrayVector(Array(bool(false)));
  else
    return ArrayVector(Array(bool(arg[0].asString() == arg[1].asString())));
  return ArrayVector(Array(bool(false)));
}

//!
//@Module STRSTR String Search Function
//@@Section STRING
//@@Usage
//Searches for the first occurance of one string inside another.
//The general syntax for its use is
//@[
//   p = strstr(x,y)
//@]
//where @|x| and @|y| are two strings.  The returned integer @|p|
//indicates the index into the string @|x| where the substring @|y|
//occurs.  If no instance of @|y| is found, then @|p| is set to
//zero.
//@@Example
//Some examples of @|strstr| in action
//@<
//strstr('hello','lo')
//strstr('quick brown fox','own')
//strstr('free stuff','lunch')
//@>
//@@Signature
//function strstr StrStrFunction
//inputs x y
//outputs flag
//!
ArrayVector StrStrFunction(int nargout, const ArrayVector& arg) {
  Array retval, arg1, arg2;
  if (arg.size() != 2)
    throw Exception("strstr function requires two string arguments");
  if (!(arg[0].isString()))
    throw Exception("strstr function requires two string arguments");
  if (!(arg[1].isString()))
    throw Exception("strstr function requires two string arguments");
  return ArrayVector(Array(double(arg[0].asString().indexOf(arg[1].asString())+1)));
}


//!
//@Module STRREP_STRING String Replace Function
//@@Section STRING
//@@Usage
//Replaces instances of a substring string with another.  This is a lower
//level function used by @|strrep|.
//@@Signature
//function strrep_string StrRepStringFunction
//inputs mainstring searchstring repstring
//outputs modifiedstring
//!
ArrayVector StrRepStringFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("strrep_string function requires three string arguments");
  if (!(arg[0].isString()))
    throw Exception("strrep_string function requires three string arguments");
  if (!(arg[1].isString()))
    throw Exception("strrep_string function requires three string arguments");
  if (!(arg[2].isString()))
    throw Exception("strrep_string function requires three string arguments");
  return ArrayVector(Array(arg[0].asString().replace(arg[1].asString(),arg[2].asString())));
}

//!
//@Module REGEXP Regular Expression Matching Function
//@@Section STRING
//@@Usage
//Matches regular expressions in the provided string.  This function is
//complicated, and compatibility with MATLABs syntax is not perfect.  The
//syntax for its use is
//@[
//  regexp('str','expr')
//@]
//which returns a row vector containing the starting index of each substring
//of @|str| that matches the regular expression described by @|expr|.  The
//second form of @|regexp| returns six outputs in the following order:
//@[
//  [start stop tokenExtents match tokens names] = regexp('str','expr')
//@]
//where the meaning of each of the outputs is defined below.
//\begin{itemize}
//\item @|start| is a row vector containing the starting index of each 
//substring that matches the regular expression.
//\item @|stop| is a row vector containing the ending index of each 
//substring that matches the regular expression.
//\item @|tokenExtents| is a cell array containing the starting and ending
//indices of each substring that matches the @|tokens| in the regular
//expression.  A token is a captured part of the regular expression.
//If the @|'once'| mode is used, then this output is a @|double| array.
//\item @|match| is a cell array containing the text for each substring
//that matches the regular expression.  In @|'once'| mode, this is a 
//string.
//\item @|tokens| is a cell array of cell arrays of strings that correspond
//to the tokens in the regular expression.  In @|'once'| mode, this is a
//cell array of strings.
//\item @|named| is a structure array containing the named tokens captured
//in a regular expression. Each named token is assigned a field in the resulting
//structure array, and each element of the array corresponds to a different
//match.
//\end{itemize}
//If you want only some of the the outputs,  you can use the 
//following variant of @|regexp|:
//@[
//  [o1 o2 ...] = regexp('str','expr', 'p1', 'p2', ...)
//@]
//where @|p1| etc. are the names of the outputs (and the order we want
//the outputs in).  As a final variant, you can supply some mode 
//flags to @|regexp|
//@[
//  [o1 o2 ...] = regexp('str','expr', p1, p2, ..., 'mode1', 'mode2')
//@]
//where acceptable @|mode| flags are:
//\begin{itemize}
//\item @|'once'| - only the first match is returned.
//\item @|'matchcase'| - letter case must match (selected by default for @|regexp|)
//\item @|'ignorecase'| - letter case is ignored (selected by default for @|regexpi|)
//\item @|'dotall'| - the @|'.'| operator matches any character (default)
//\item @|'dotexceptnewline'| - the @|'.'| operator does not match the newline character
//\item @|'stringanchors'| - the @|^| and @|$| operators match at the beginning and 
//end (respectively) of a string.
//\item @|'lineanchors'| - the @|^| and @|$| operators match at the beginning and
//end (respectively) of a line.
//\item @|'literalspacing'| - the space characters and comment characters @|#| are matched
//as literals, just like any other ordinary character (default).
//\item @|'freespacing'| - all spaces and comments are ignored in the regular expression.
//You must use '\ ' and '\#' to match spaces and comment characters, respectively.
//\end{itemize}
//Note the following behavior differences between MATLABs regexp and FreeMats:
//\begin{itemize}
//\item If you have an old version of @|pcre| installed, then named tokens must use the
//older @|<?P<name>| syntax, instead of the new @|<?<name>| syntax.  
//\item The @|pcre| library is pickier about named tokens and their appearance in 
//expressions.  So, for example, the regexp from the MATLAB 
//manual @|'(?<first>\\w+)\\s+(?<last>\\w+)|(?<last>\\w+),\\s+(?<first>\\w+)'|
//does not work correctly (as of this writing) because the same named 
//tokens appear multiple
//times.  The workaround is to assign different names to each token, and then collapse
//the results later.
//\end{itemize}
//@@Example
//Some examples of using the @|regexp| function
//@<
//[start,stop,tokenExtents,match,tokens,named] = regexp('quick down town zoo','(.)own')
//@>
//@@Signature
//function regexp RegExpFunction
//inputs string expr varargin
//outputs start stop tokenExtents match tokens names
//@@Signature
//function regexpi RegExpIFunction
//inputs string expr varargin
//outputs start stop tokenExtents match tokens names
//@@Signature
//function regexprepdriver RegExpRepDriverFunction
//inputs varargin
//outputs y
//!
#if HAVE_PCRE
static bool isSlotSpec(QString t) {
  return ((t == "start") ||
	  (t == "end") ||
	  (t == "tokenExtents") ||
	  (t == "match") ||
	  (t == "tokens") ||
	  (t == "names"));
}
#endif

static ArrayVector RegExpCoreFunction(StringVector stringed_args, bool defaultMatchCase) {
#if HAVE_PCRE
  // These are the default options
  bool globalMatch = true;
  bool matchCase = defaultMatchCase;
  bool greedyDot = true;
  bool stringAnchors = true;
  bool literalSpacing = true;
  
  int start_slot = 0;
  int end_slot = 1;
  int tokenExtents_slot = 2;
  int match_slot = 3;
  int tokens_slot = 4;
  int names_slot = 5;
  int count_slots = 6;

  // This is a cut-paste-hack of the pcredemo program that comes with pcre
  QList<uint32> startList;
  QList<uint32> stopList;
  QList<uint32> tokenStartList;
  QList<uint32> tokenStopList;
  QList<QList<uint32> > tokenExtentsList;
  QList<StringVector> tokenList;
  StringVector matchList;
  StringVector namedTokenNames;
  QList<StringVector> namedTokenValues;

  pcre *re;
  const char *error;
  const char *pattern;
  const char *subject;
  unsigned char *name_table;
  int erroffset;
  int namecount;
  int name_entry_size;
  int subject_length;
  int rc, i;
  ArrayVector retvec;

  if (stringed_args.size() < 2)
    throw Exception("regexpcore requires at least two arguments - both strings, the data string and pattern string");

  // Check for output specifiers
  bool slot_specs_present = false;
  for (int i=2;i<stringed_args.size();i++) 
    if (isSlotSpec(stringed_args[i])) {
      slot_specs_present = true;
      break;
    }
  if (slot_specs_present) {
    // output specifiers are active... eliminate all outputs by default
    start_slot = end_slot = tokenExtents_slot = match_slot = tokens_slot = names_slot = -1;
    count_slots = 0;
    // Process inputs
    for (int i=2;i<stringed_args.size();i++) {
      QString t = stringed_args[i];
      if (t == "start")  start_slot = count_slots++;
      if (t == "end") end_slot = count_slots++;
      if (t == "tokenExtents") tokenExtents_slot = count_slots++;
      if (t == "match") match_slot = count_slots++;
      if (t == "tokens") tokens_slot = count_slots++;
      if (t == "names") names_slot = count_slots++;
    }
  }
  
  for (int i=0;i<count_slots;i++)
    retvec << EmptyConstructor();
  
  for (int j=2;j<stringed_args.size();j++) {
    if (stringed_args[j]=="once")
      globalMatch = false;
    else if (stringed_args[j]=="matchcase")
      matchCase = true;
    else if (stringed_args[j]=="ignorecase")
      matchCase = false;
    else if (stringed_args[j]=="dotall")
      greedyDot = true;
    else if (stringed_args[j]=="dotexceptnewline")
      greedyDot = false;
    else if (stringed_args[j]=="stringanchors")
      stringAnchors = true;
    else if (stringed_args[j]=="lineanchors")
      stringAnchors = false;
    else if (stringed_args[j]=="literalspacing")
      literalSpacing = true;
    else if (stringed_args[j]=="freespacing")
      literalSpacing = false;
  }

  pattern = stringed_args[1].toUtf8().constData();
  subject = stringed_args[0].toUtf8().constData();
  subject_length = (int)strlen(subject);
  QString qsubject(subject);

  int options = 0;

  if (!matchCase) options |= PCRE_CASELESS;
  if (greedyDot) options |= PCRE_DOTALL;
  if (!stringAnchors) options |= PCRE_MULTILINE;
  if (!literalSpacing) options |= PCRE_EXTENDED;

  /*************************************************************************
   * Now we are going to compile the regular expression pattern, and handle *
   * and errors that are detected.                                          *
   *************************************************************************/
  
  re = pcre_compile(
		    pattern,              /* the pattern */
		    options,              /* default options */
		    &error,               /* for error message */
		    &erroffset,           /* for error offset */
		    NULL);                /* use default character tables */
  
  /* Compilation failed: print the error message and exit */
  
  if (re == NULL) 
    throw Exception(QString("regular expression compilation failed at offset ") + 
		    erroffset + ": " + error);
  
  /* Determine how many capture expressions there are */
  int captureCount;
  rc = pcre_fullinfo(re,NULL,PCRE_INFO_CAPTURECOUNT,&captureCount);
  
  /* Allocate the output vector */
  int ovectorSize = (captureCount+1)*3;
  MemBlock<int> ovectorData(ovectorSize);
  int *ovector(&ovectorData);
  
  /*************************************************************************
   * If the compilation succeeded, we call PCRE again, in order to do a     *
   * pattern match against the subject string. This does just ONE match. If *
   * further matching is needed, it will be done below.                     *
   *************************************************************************/
  
  rc = pcre_exec(
		 re,                   /* the compiled pattern */
		 NULL,                 /* no extra data - we didn't study the pattern */
		 subject,              /* the subject string */
		 subject_length,       /* the length of the subject */
		 0,                    /* start at offset 0 in the subject */
		 0,                    /* default options */
		 ovector,              /* output vector for substring information */
		 ovectorSize);         /* number of elements in the output vector */

  /* Matching failed: handle error cases */
  
  if (rc < 0) {
    pcre_free(re);     /* Release memory used for the compiled pattern */
    return retvec;
  }
  
  /* Match succeded */

  startList << ovector[0]+1;
  stopList << ovector[1];
  matchList << qsubject.mid(ovector[0],ovector[1]-ovector[0]);

  /*************************************************************************
   * We have found the first match within the subject string. If the output *
   * vector wasn't big enough, set its size to the maximum. Then output any *
   * substrings that were captured.                                         *
   *************************************************************************/

  /* The output vector wasn't big enough */

  if (rc == 0) 
    throw Exception("internal error - more outputs than expected?");

  /* Show substrings stored in the output vector by number. Obviously, in a real
     application you might want to do things other than print them. */

  QList<uint32> tEList;
  StringVector   tList;
  for (i = 1; i < rc; i++)
    {
      tEList << ovector[2*i]+1;
      tEList << ovector[2*i+1];
      tList << qsubject.mid(ovector[2*i],ovector[2*i+1]-ovector[2*i]);
    }

  tokenExtentsList << tEList;
  tokenList << tList;


  /* See if there are any named substrings, and if so, show them by name. First
     we have to extract the count of named parentheses from the pattern. */
  
  (void)pcre_fullinfo(
		      re,                   /* the compiled pattern */
		      NULL,                 /* no extra data - we didn't study the pattern */
		      PCRE_INFO_NAMECOUNT,  /* number of named substrings */
		      &namecount);          /* where to put the answer */


  if (namecount > 0)  {
    unsigned char *tabptr;
    
    /* Before we can access the substrings, we must extract the table for
       translating names to numbers, and the size of each entry in the table. */
    
    (void)pcre_fullinfo(
			re,                       /* the compiled pattern */
			NULL,                     /* no extra data - we didn't study the pattern */
			PCRE_INFO_NAMETABLE,      /* address of the table */
			&name_table);             /* where to put the answer */
    
    (void)pcre_fullinfo(
			re,                       /* the compiled pattern */
			NULL,                     /* no extra data - we didn't study the pattern */
			PCRE_INFO_NAMEENTRYSIZE,  /* size of each entry in the table */
			&name_entry_size);        /* where to put the answer */
    
    /* Now we can scan the table and, for each entry, print the number, the name,
       and the substring itself. */
    
    tabptr = name_table;
    for (i = 0; i < namecount; i++)
      {
	namedTokenValues << StringVector();
	int n = (tabptr[0] << 8) | tabptr[1];
	namedTokenNames << QString((char*)(tabptr+2)).left(name_entry_size-3);
	namedTokenValues[i] << qsubject.mid(ovector[2*n],ovector[2*n+1]-ovector[2*n]);
	tabptr += name_entry_size;
      }
  }


  /*************************************************************************
   * If the "-g" option was given on the command line, we want to continue  *
   * to search for additional matches in the subject string, in a similar   *
   * way to the /g option in Perl. This turns out to be trickier than you   *
   * might think because of the possibility of matching an empty string.    *
   * What happens is as follows:                                            *
   *                                                                        *
   * If the previous match was NOT for an empty string, we can just start   *
   * the next match at the end of the previous one.                         *
   *                                                                        *
   * If the previous match WAS for an empty string, we can't do that, as it *
   * would lead to an infinite loop. Instead, a special call of pcre_exec() *
   * is made with the PCRE_NOTEMPTY and PCRE_ANCHORED flags set. The first  *
   * of these tells PCRE that an empty string is not a valid match; other   *
   * possibilities must be tried. The second flag restricts PCRE to one     *
   * match attempt at the initial string position. If this match succeeds,  *
   * an alternative to the empty string match has been found, and we can    *
   * proceed round the loop.                                                *
   *************************************************************************/


  /* Loop for second and subsequent matches */
  
  if (globalMatch && (ovector[1] < subject_length)) {
    for (;;)
      {
	int options = 0;                 /* Normally no options */
	int start_offset = ovector[1];   /* Start at end of previous match */
	
	/* If the previous match was for an empty string, we are finished if we are
	   at the end of the subject. Otherwise, arrange to run another match at the
	   same point to see if a non-empty match can be found. */
	
	if (ovector[0] == ovector[1])
	  {
	    if (ovector[0] == subject_length) break;
	    options = PCRE_NOTEMPTY | PCRE_ANCHORED;
	  }
	
	/* Run the next matching operation */
	
	rc = pcre_exec(
		       re,                   /* the compiled pattern */
		       NULL,                 /* no extra data - we didn't study the pattern */
		       subject,              /* the subject string */
		       subject_length,       /* the length of the subject */
		       start_offset,         /* starting offset in the subject */
		       options,              /* options */
		       ovector,              /* output vector for substring information */
		       ovectorSize);           /* number of elements in the output vector */
	
	/* This time, a result of NOMATCH isn't an error. If the value in "options"
	   is zero, it just means we have found all possible matches, so the loop ends.
	   Otherwise, it means we have failed to find a non-empty-string match at a
	   point where there was a previous empty-string match. In this case, we do what
	   Perl does: advance the matching position by one, and continue. We do this by
	   setting the "end of previous match" offset, because that is picked up at the
	   top of the loop as the point at which to start again. */
	
	if (rc == PCRE_ERROR_NOMATCH)
	  {
	    if (options == 0) break;
	    ovector[1] = start_offset + 1;
	    continue;    /* Go round the loop again */
	  }
	
	/* Other matching errors are not recoverable. */
	
	if (rc < 0)
	  {
	    break;
	  }
	
	/* Match succeded */
	
	/* The match succeeded, but the output vector wasn't big enough. */
	
	if (rc == 0)
	  throw Exception("internal error - more outputs than expected?");
	
	startList << ovector[0]+1;
	stopList << ovector[1];
	matchList << qsubject.mid(ovector[0],ovector[1]-ovector[0]);

	QList<uint32> tEList;
	StringVector   tList;
	for (i = 1; i < rc; i++)
	  {
	    tEList << ovector[2*i]+1;
	    tEList << ovector[2*i+1];
	    tList << qsubject.mid(ovector[2*i],ovector[2*i+1]-ovector[2*i]);
	  }
	
	tokenExtentsList << tEList;
	tokenList << tList;

	if (namecount > 0) {
	  unsigned char *tabptr = name_table;
	  tabptr = name_table;
	  for (i = 0; i < namecount; i++)
	    {
	      int n = (tabptr[0] << 8) | tabptr[1];
	      namedTokenValues[i] << qsubject.mid(ovector[2*n],ovector[2*n+1]-ovector[2*n]);
	      tabptr += name_entry_size;
	    }
	}
      }      /* End of loop to find second and subsequent matches */
  }

  // Return this data to the user
  Array start(DoubleVectorFromQList(startList));
  Array end(DoubleVectorFromQList(stopList));
  Array matches(CellArrayFromStringVector(matchList));
  // Now build the tokens array
  ArrayVector tokensArrayContents;
  for (int i=0;i<tokenList.size();i++) 
    tokensArrayContents << CellArrayFromStringVector(tokenList[i]);
  Array tokens(CellConstructor(tokensArrayContents));
  // Finally the token extents array
  ArrayVector tokensExtentsContents;
  for (int i=0;i<tokenExtentsList.size();i++)
    tokensExtentsContents << DoubleVectorFromQList(tokenExtentsList[i]);
  Array tokenExtents(CellConstructor(tokensExtentsContents));
  // The named token data has to be resliced
  ArrayVector namedTokenValueContents;
  for (int i=0;i<namedTokenValues.size();i++)
    namedTokenValueContents << CellArrayFromStringVector(namedTokenValues[i]);
  Array namedTokens(StructConstructor(namedTokenNames,namedTokenValueContents));
  // Stuff it all into a return vector
  pcre_free(re);
  if (start_slot >= 0)
    retvec[start_slot] = start;
  if (end_slot >= 0)
    retvec[end_slot] = end;
  if (tokenExtents_slot >= 0)
    retvec[tokenExtents_slot] = tokenExtents;
  if (match_slot >= 0)
    retvec[match_slot] = matches;
  if (tokens_slot >= 0)
    retvec[tokens_slot] = tokens;
  if (names_slot >= 0)
    retvec[names_slot] = namedTokens;
  return retvec;
#else
  throw Exception("regexp support not compiled in (requires PCRE library)");
#endif  

}

// res is organized like this:
//  <o1 o2 o3 o4>
//  <o1 o2 o3 o4>
// ...
// We want to perform a transpose
static ArrayVector CellifyRegexpResults(QList<ArrayVector> res, const NTuple &dims) {
  ArrayVector retVec;
  if (res.size() == 0) return ArrayVector();
  for (int i=0;i<res[0].size();i++) {
    ArrayVector slice;
    for (int j=0;j<res.size();j++) 
      slice << res[j][i];
    int slice_len = slice.size();
    Array sliceArray(CellArrayFromArrayVector(slice,slice_len));
    sliceArray.reshape(dims);
    retVec << sliceArray;
  }
  return retVec;
}

static ArrayVector RegExpWrapperFunction(int nargout, const ArrayVector& arg, 
					 bool caseMatch) {
  if (arg.size() < 2) throw Exception("regexp requires at least two arguments to function");
  for (int i=2;i<arg.size();i++) 
    if (!arg[i].isString())
      throw Exception("all arguments to regexp must be strings");
  if (arg[0].isString() && arg[1].isString()) {
    // If both arguments are scalar strings...
    StringVector stringed_args;
    // Convert the argument array to strings
    for (int i=0;i<arg.size();i++) 
      stringed_args << arg[i].asString();
    return RegExpCoreFunction(stringed_args,caseMatch);
  } else if (IsCellStringArray(arg[0]) && arg[1].isString()) {
    StringVector arg0(StringVectorFromArray(arg[0]));
    QList<ArrayVector> results;
    for (int j=0;j<arg0.size();j++) {
      StringVector stringed_args;
      stringed_args << arg0[j];
      for (int i=1;i<arg.size();i++) 
	stringed_args << arg[i].asString();
      results << RegExpCoreFunction(stringed_args,caseMatch);
    }
    return CellifyRegexpResults(results,arg[0].dimensions());
  } else if (arg[0].isString() && IsCellStringArray(arg[1])) {
    StringVector arg1(StringVectorFromArray(arg[1]));
    QList<ArrayVector> results;
    for (int j=0;j<arg1.size();j++) {
      StringVector stringed_args;
      stringed_args << arg[0].asString();
      stringed_args << arg1[j];
      for (int i=2;i<arg.size();i++) 
	stringed_args << arg[i].asString();
      results << RegExpCoreFunction(stringed_args,caseMatch);
    }
    return CellifyRegexpResults(results,arg[1].dimensions());
  } else if (IsCellStringArray(arg[0]) && IsCellStringArray(arg[1])) {
    if (arg[0].length() != arg[1].length())
      throw Exception("cell-arrays of strings as the first two arguments to regexp must be the same size");
    StringVector dp(StringVectorFromArray(arg[0]));
    StringVector sp(StringVectorFromArray(arg[1]));
    QList<ArrayVector> results;
    for (int j=0;j<arg[0].length();j++) {
      StringVector stringed_args;
      stringed_args << dp[j];
      stringed_args << sp[j];
      for (int i=2;i<arg.size();i++) 
	stringed_args << arg[i].asString();
      results << RegExpCoreFunction(stringed_args,caseMatch);
    }
    return CellifyRegexpResults(results,arg[0].dimensions());
  } else {
    throw Exception("first two arguments to regexp must be strings of cell-arrays of strings");
  }
  return ArrayVector();
}

ArrayVector RegExpFunction(int nargout, const ArrayVector&arg) {
  return RegExpWrapperFunction(nargout, arg, true);
}

ArrayVector RegExpIFunction(int nargout, const ArrayVector& arg) {
  return RegExpWrapperFunction(nargout, arg, false);
}

// Perform a replace 
QString RegExpRepCoreFunction(QString subject,
			      QString pattern,
			      StringVector modes,
			      StringVector replacements) {
#if HAVE_PCRE
  // These are the default options
  bool globalMatch = true;
  bool matchCase = true;
  bool greedyDot = true;
  bool stringAnchors = true;
  bool literalSpacing = true;
  
  pcre *re;
  const char *error;
  int erroffset;
  int rc;

  for (int j=0;j<modes.size();j++) {
    if (modes[j]=="once")
      globalMatch = false;
    else if (modes[j]=="matchcase")
      matchCase = true;
    else if (modes[j]=="ignorecase")
      matchCase = false;
    else if (modes[j]=="dotall")
      greedyDot = true;
    else if (modes[j]=="dotexceptnewline")
      greedyDot = false;
    else if (modes[j]=="stringanchors")
      stringAnchors = true;
    else if (modes[j]=="lineanchors")
      stringAnchors = false;
    else if (modes[j]=="literalspacing")
      literalSpacing = true;
    else if (modes[j]=="freespacing")
      literalSpacing = false;
  }

  int options = 0;

  if (!matchCase) options |= PCRE_CASELESS;
  if (greedyDot) options |= PCRE_DOTALL;
  if (!stringAnchors) options |= PCRE_MULTILINE;
  if (!literalSpacing) options |= PCRE_EXTENDED;

  /*************************************************************************
   * Now we are going to compile the regular expression pattern, and handle *
   * and errors that are detected.                                          *
   *************************************************************************/
  
  re = pcre_compile(
		    pattern.toUtf8().constData(),      /* the pattern */
		    options,              /* default options */
		    &error,               /* for error message */
		    &erroffset,           /* for error offset */
		    NULL);                /* use default character tables */
  
  /* Compilation failed: print the error message and exit */
  
  if (re == NULL) 
    throw Exception(QString("regular expression compilation failed at offset ") + 
		    erroffset + ": " + error);

  /* Determine how many capture expressions there are */
  int captureCount;
  rc = pcre_fullinfo(re,NULL,PCRE_INFO_CAPTURECOUNT,&captureCount);
  
  /* Allocate the output vector */
  int ovectorSize = (captureCount+1)*3;
  MemBlock<int> ovectorData(ovectorSize);
  int *ovector(&ovectorData);
  
  /*************************************************************************
   * If the compilation succeeded, we call PCRE again, in order to do a     *
   * pattern match against the subject string. This does just ONE match. If *
   * further matching is needed, it will be done below.                     *
   *************************************************************************/
  
  rc = pcre_exec(
		 re,                   /* the compiled pattern */
		 NULL,                 /* no extra data - we didn't study the pattern */
		 subject.toUtf8().constData(),      /* the subject string */
		 subject.toUtf8().size(),       /* the length of the subject */
		 0,                    /* start at offset 0 in the subject */
		 0,                    /* default options */
		 ovector,              /* output vector for substring information */
		 ovectorSize);         /* number of elements in the output vector */

  /* Matching failed: handle error cases */
  
  if (rc < 0) {
    pcre_free(re);     /* Release memory used for the compiled pattern */
    return subject;
  }
  
  QString outputString;
  QString tokenSelect;
  int nextReplacement = 0;
  int inputPointer = 0;
  int outputPtr = 0;
  int digitFinder = 0;
  int replacementLength = replacements[nextReplacement].size();
  int tokenNumber;
  /* Match succeeded... start building up the output string */
  while (inputPointer < ovector[0]) outputString += subject[inputPointer++];
  /* Now insert the replacement string */
  while (outputPtr < replacementLength) {
    if (replacements[nextReplacement][outputPtr] != '$')
      outputString += replacements[nextReplacement][outputPtr++];
    else
      if ((outputPtr < (replacementLength-1)) &&
	  replacements[nextReplacement][outputPtr+1].isDigit()) {
	// Try to collect a token name
	digitFinder = outputPtr+1;
	while ((digitFinder < replacementLength) && 
	       replacements[nextReplacement][digitFinder].isDigit()) {
	  // Add this digit to the token name
	  tokenSelect += replacements[nextReplacement][digitFinder];
	  digitFinder++;
	}
	// try to map this to a token number
	tokenNumber = tokenSelect.toInt();
	// Is this a valid tokenNumber?
	if (tokenNumber <= captureCount) {
	  // Yes - skip
	  outputPtr = digitFinder;
	  // Push the capturecount...
	  for (int p=ovector[2*tokenNumber];p<ovector[2*tokenNumber+1];p++)
	    outputString += subject[p];
	} else {
	  // No - just push the '$' to the output, and bump outputPtr
	  outputString += '$';
	  outputPtr++;
	}
      }
  }
  /* advance the input pointer */
  inputPointer = ovector[1];
  
  if (replacements.size() > 1)
    nextReplacement++;
  
  if (globalMatch && (ovector[1] < (int)subject.size())) {
    for (;;)
      {
	int options = 0;                 /* Normally no options */
	int start_offset = ovector[1];   /* Start at end of previous match */
	
	/* If the previous match was for an empty string, we are finished if we are
	   at the end of the subject. Otherwise, arrange to run another match at the
	   same point to see if a non-empty match can be found. */
	
	if (ovector[0] == ovector[1])
	  {
	    if (ovector[0] == (int) subject.size()) break;
	    options = PCRE_NOTEMPTY | PCRE_ANCHORED;
	  }
	
	/* Run the next matching operation */
	
	rc = pcre_exec(
		       re,                   /* the compiled pattern */
		       NULL,                 /* no extra data - we didn't study the pattern */
		       subject.toUtf8().constData(),      /* the subject string */
		       subject.toUtf8().size(),       /* the length of the subject */
		       start_offset,         /* starting offset in the subject */
		       options,              /* options */
		       ovector,              /* output vector for substring information */
		       ovectorSize);           /* number of elements in the output vector */
	
	/* This time, a result of NOMATCH isn't an error. If the value in "options"
	   is zero, it just means we have found all possible matches, so the loop ends.
	   Otherwise, it means we have failed to find a non-empty-string match at a
	   point where there was a previous empty-string match. In this case, we do what
	   Perl does: advance the matching position by one, and continue. We do this by
	   setting the "end of previous match" offset, because that is picked up at the
	   top of the loop as the point at which to start again. */
	
	if (rc == PCRE_ERROR_NOMATCH)
	  {
	    if (options == 0) break;
	    ovector[1] = start_offset + 1;
	    continue;    /* Go round the loop again */
	  }
	
	/* Other matching errors are not recoverable. */
	
	if (rc < 0)
	  {
	    break;
	  }
	
	/* Match succeded */
	
	/* The match succeeded, but the output vector wasn't big enough. */
	
	if (rc == 0)
	  throw Exception("internal error - more outputs than expected?");
	
	/* output characters from the subject string until we reach the next match */
	while (inputPointer < ovector[0]) outputString += subject[inputPointer++];
	inputPointer = ovector[1];

	/* output the replacement string */
	replacementLength = replacements[nextReplacement].size();
	outputPtr = 0;
	tokenSelect = "";
	/* Now insert the replacement string */
	while (outputPtr < replacementLength) {
	  if (replacements[nextReplacement][outputPtr] != '$')
	    outputString += replacements[nextReplacement][outputPtr++];
	  else
	    if ((outputPtr < (replacementLength-1)) &&
		replacements[nextReplacement][outputPtr+1].isDigit()) {
	      // Try to collect a token name
	      digitFinder = outputPtr+1;
	      while ((digitFinder < replacementLength) && 
		     replacements[nextReplacement][digitFinder].isDigit()) {
		// Add this digit to the token name
		tokenSelect += replacements[nextReplacement][digitFinder];
		digitFinder++;
	      }
	      // try to map this to a token number
	      tokenNumber = tokenSelect.toInt();
	      // Is this a valid tokenNumber?
	      if (tokenNumber <= captureCount) {
		// Yes - skip
		outputPtr = digitFinder;
		// Push the capturecount...
		for (int p=ovector[2*tokenNumber];p<ovector[2*tokenNumber+1];p++)
		  outputString += subject[p];
	      } else {
		// No - just push the '$' to the output, and bump outputPtr
		outputString += '$';
		outputPtr++;
	      }
	    }
	}
	if (replacements.size() > 1) {
	  nextReplacement++;
	  if (nextReplacement >= replacements.size())
	    break; // No more replacments to make
	}
      }      /* End of loop to find second and subsequent matches */
  }

  while (inputPointer < (int) subject.size()) 
    outputString += subject[inputPointer++];
  
  pcre_free(re);
  return outputString;
#else
  throw Exception("regexprep support not compiled in (requires PCRE library)");
#endif
}

ArrayVector RegExpRepDriverFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3) throw Exception("regexprep requires at least three arguments to function");
  for (int i=3;i<arg.size();i++) 
    if (!arg[i].isString())
      throw Exception("all arguments to regexprep must be strings");
  StringVector modes;
  for (int i=3;i<arg.size();i++)
    modes << arg[i].asString();
  QString subject = arg[0].asString();
  QString pattern = arg[1].asString();
  StringVector replist;
  if (arg[2].isString())
    replist << arg[2].asString();
  else if (IsCellStringArray(arg[2])) {
    StringVector dp(StringVectorFromArray(arg[2]));
    for (int i=0;i<dp.size();i++)
      replist << dp[i];
  }
  return ArrayVector(Array(RegExpRepCoreFunction(subject,pattern,modes,replist)));
}				  

//!
//@Module NUM2STR Convert Numbers To Strings
//@@Section ARRAY
//@@Usage
//Converts an array into its string representation.  The general syntax
//for this function is
//@[
//   s = num2str(X)
//@]
//where @|s| is a string (or string matrix) and @|X| is an array.  By
//default, the @|num2str| function uses 4 digits of precision and an 
//exponent if required.  If you want more digits of precision, you can 
//specify the precition via the form
//@[
//   s = num2str(X, precision)
//@]
//where @|precision| is the number of digits to include in the string
//representation.  For more control over the format of the output, you 
//can also specify a format specifier (see @|printf| for more details).
//@[
//   s = num2str(X, format)
//@]
//where @|format| is the specifier string.
//!

template <class T>
static Array Num2StrHelperReal(const BasicArray<T> &dp, const char *formatspec) {
  ConstBasicIterator<T> iter(&dp,1);
  StringVector all_rows;
  while (iter.isValid()) {
    QString row_string;
    for (index_t i=1;i<=iter.size();i++) {
      if (i != 1) row_string += " ";
      row_string += QString().sprintf(formatspec,iter.get());
      iter.next();
    }
    all_rows << row_string;
    iter.nextSlice();
  }
  return StringArrayFromStringVector(all_rows);
}

template <class T>
Array Num2StrHelperComplex(const BasicArray<T> &rp, const BasicArray<T> &ip, const char *formatspec) {
  ConstBasicIterator<T> iter_real(&rp,1);
  ConstBasicIterator<T> iter_imag(&ip,1);
  StringVector all_rows;
  while (iter_real.isValid() && iter_imag.isValid()) {
    QString row_string;
    for (index_t i=1;i<=iter_real.size();i++) {
      if (i != 1) row_string += " ";
      row_string += QString().sprintf(formatspec,iter_real.get());
      if (iter_imag.get() >= 0) row_string += "+";
      row_string += QString().sprintf(formatspec,iter_imag.get());
      row_string += "i";
      iter_real.next();
      iter_imag.next();
    }
    all_rows << row_string;
    iter_real.nextSlice();
    iter_imag.nextSlice();
  }
  return StringArrayFromStringVector(all_rows);
}

template <typename T>
static ArrayVector Num2Str(const Array &X, const char *formatspec) {
  if (X.allReal())
    return ArrayVector(Num2StrHelperReal(X.constReal<T>(),formatspec));
  else
    return ArrayVector(Num2StrHelperComplex(X.constReal<T>(),X.constImag<T>(),formatspec));
}

#define MacroNum2Str(ctype,cls) \
  case cls: return Num2Str<ctype>(X,formatspec);

ArrayVector Num2StrFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("num2str function requires at least one argument");
  Array X(arg[0]);
  if (X.isReferenceType())
    throw Exception("num2str function requires a numeric input");
  if (X.isSparse())
    throw Exception("num2str not defined for sparse inputs");
  X = X.asDenseArray();
  char formatspec[1024];
  if ((X.dataClass() != Float) && (X.dataClass() != Double))
    sprintf(formatspec,"%%d");
  else
    sprintf(formatspec,"%%g"); //without preceding space
  if (arg.size() > 1) {
    Array format(arg[1]);
    if (format.isString())
      strcpy(formatspec,qPrintable(format.asString()));
  }
  switch (X.dataClass())  {
    default: throw Exception("illegal type argument to num2str");
      MacroExpandCases(MacroNum2Str);
  }
}

//!
//@Module STR2NUM Convert a String to a Number
//@@Section IO
//@@Usage
//Converts a string to a number.  The general syntax for its use
//is
//@[
//  x = str2num(string)
//@]
//Here @|string| is the data string, which contains the data to 
//be converted into a number.  The output is in double precision,
//and must be typecasted to the appropriate type based on what
//you need.
//@@Signature
//function str2num Str2NumFunction
//inputs string
//outputs x
//!
ArrayVector Str2NumFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("str2num takes a single argument, the string to convert into a number");
  Array data(arg[0]);
  if (!data.isString())
    throw Exception("the first argument to str2num must be a string");
  return ArrayVector(Array(data.asDouble()));
}


//!
//@Module SSCANF Formated String Input Function (C-Style)
//@@Section IO
//@@Usage
//Reads values from a string.  The general syntax for its use is
//@[
//  [a1,...,an] = sscanf(text,format)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the input.  Each value that is parsed
//from the @|text| occupies one output slot.  See @|printf|
//for a description of the format.
//@@Signature
//function sscanf SscanfFunction
//inputs text format
//outputs varargout
//!
ArrayVector SscanfFunction(int nargout, const ArrayVector& arg) {
  if ((arg.size() != 2) || (!arg[0].isString()) || (!arg[1].isString()))
    throw Exception("sscanf takes two arguments, the text and the format string");
  QTemporaryFile fp;
  if (!fp.open())
    throw Exception("sscanf was unable to open a temp file (and so it won't work)");
  QTextStream out(&fp);
  out << arg[0].asString();
  out.seek(0);
  fp.seek(0);
  return ScanfFunction(&fp,arg[1].asString());
}
