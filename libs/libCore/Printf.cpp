#include "Printf.hpp"
#include "Print.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"
#include <QTextCodec>

static int flagChar(char c) {
  return ((c == '#') ||  (c == '0') || (c == '-') ||  
	  (c == ' ') ||  (c == '+'));
}
  
static int convspec(char c) {
  return ((c == 'd') || (c == 'i') || (c == 'o') || 
	  (c == 'u') || (c == 'x') || (c == 'X') ||
	  (c == 'e') || (c == 'E') || (c == 'f') || 
	  (c == 'F') || (c == 'g') || (c == 'G') ||
	  (c == 'a') || (c == 'A') || (c == 'c') || 
	  (c == 's'));
}
    
static char* validateFormatSpec(char* cp) {
  if (*cp == '%') return cp+1;
  while ((*cp) && flagChar(*cp)) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  while ((*cp) && (*cp == '.')) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  if ((*cp) && convspec(*cp)) 
    return cp+1;
  else
    return 0;
}

static char* validateScanFormatSpec(char* cp) {
  if (*cp == '%') return cp+1;
  while ((*cp) && flagChar(*cp)) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  while ((*cp) && (*cp == '.')) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  if ((*cp) && (convspec(*cp) || (*cp == 'h') || (*cp == 'l')))
    return cp+1;
  else
    return 0;
}

QString ConvertEscapeSequences(const QString &src) {
  QString dest;
  int i=0;
  while (i < src.size()) {
    if ((src[i] != '\\') || (i == (src.size()-1))) {
      dest += src[i];
    } else {
      if (src[i+1] == 'n') {
	dest += '\n';
	i++;
      } else if (src[i+1] == 't') {
	dest += '\t';
	i++;
      } else if (src[i+1] == 'r') {
	dest += '\r';
	i++;
      } else {
	dest += src[i+1];
	i++;
      }
    }
    i++;
  }
  return dest;
}
  
//Common routine used by sprintf,printf,fprintf.  They all
//take the same inputs, and output either to a string, the
//console or a file.  For output to a console or a file, 
//we want escape-translation on.  For output to a string, we
//want escape-translation off.  So this common routine prints
//the contents to a string, which is then processed by each 
//subroutine.
QString XprintfFunction(int nargout, const ArrayVector& arg) {
  char *buff = strdup(qPrintable(arg[0].asString()));
  // Search for the start of a format subspec
  char *dp = buff;
  char *np;
  char sv;
  // Buffer to hold each sprintf result
#define BUFSIZE 65536
  char nbuff[BUFSIZE];
  // Buffer to hold the output
  QString op;
  int nextArg = 1;
  // Scan the string
  while (*dp) {
    np = dp;
    while ((*dp) && (*dp != '%')) dp++;
    // Print out the formatless segment
    sv = *dp;
    *dp = 0;
    snprintf(nbuff,BUFSIZE,np);
    op += nbuff;
    *dp = sv;
    // Process the format spec
    if (*dp) {
      np = validateFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + QString(dp));
      else {
	if (*(np-1) == '%') {
	  snprintf(nbuff,BUFSIZE,"%%");
	  op += nbuff;
	  dp+=2;
	} else {
	  sv = *np;
	  *np = 0;
	  if (arg.size() <= nextArg)
	    throw Exception("not enough arguments to satisfy format specification");
	  Array nextVal(arg[nextArg++]);
	  if ((*(np-1) != 's') && (nextVal.isEmpty())) {
	    op += "[]";
	  } else {
	    switch (*(np-1)) {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
	      snprintf(nbuff,BUFSIZE,dp,nextVal.asInteger());
	      op += nbuff;
	      break;
	    case 'e':
	    case 'E':
	    case 'f':
	    case 'F':
	    case 'g':
	    case 'G':
	      snprintf(nbuff,BUFSIZE,dp,nextVal.asDouble());
	      op += nbuff;
	      break;
	    case 's':
	      op += nextVal.asString();
	    }
	  }
	  *np = sv;
	  dp = np;
	}
      }
    }
  }
  free(buff);
  return op;
}


//!
//@Module SPRINTF Formated String Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to a string.  The general syntax for its use is
//@[
//  y = sprintf(format,a1,a2,...).
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|a_i| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|sprintf| command is not vectorized!  Each
//variable must be a scalar.  The returned value @|y| contains the
//string that would normally have been printed. For
//more details on the format string, see @|printf|.  
//@@Examples
//Here is an example of a loop that generates a sequence of files based on
//a template name, and stores them in a cell array.
//@<
//l = {}; for i = 1:5; s = sprintf('file_%d.dat',i); l(i) = {s}; end;
//l
//@>
//@@Signature
//function sprintf SprintfFunction
//inputs varargin
//outputs string
//!
ArrayVector SprintfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("sprintf requires at least one (string) argument");
  if (!arg[0].isString())
    throw Exception("sprintf format argument must be a string");
  return ArrayVector(Array(ConvertEscapeSequences(XprintfFunction(nargout,arg))));
}
  
//!
//@Module PRINTF Formated Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to the output.  The general syntax for its use is
//@[
//  printf(format,a1,a2,...)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|a_i| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|printf| command is not vectorized!  Each
//variable must be a scalar.
//
//It is important to point out that the @|printf| function does not
//add a newline (or carriage return) to the output by default.  That
//can lead to some confusing behavior if you do not know what to expect.
//For example, the command @|printf('Hello')| does not appear to
//produce any output.  In fact, it does produce the text, but it then
//gets overwritten by the prompt.  To see the text, you need 
//@|printf('Hello\n')|.  This seems odd, but allows you to assemble a
//line using multiple @|printf| commands, including the @|'\n'| when
//you are done with the line.  You can also use the @|'\r'| character
//as an explicit carriage return (with no line feed).  This allows you
//to write to the same line many times (to show a progress string, for
//example).
//
//@@Format of the format string:
//
//The  format  string  is a character string, beginning and ending in its
//initial shift state, if any.  The format string is composed of zero  or
//more   directives:  ordinary  characters  (not  %),  which  are  copied
//unchanged to the output stream; and conversion specifications, each  of
//which results in fetching zero or more subsequent arguments.  Each 
//conversion specification is introduced by the character %, and ends with a
//conversion  specifier.  In between there may be (in this order) zero or
//more flags, an optional minimum field width, and an optional precision.
//
//The  arguments must correspond properly (after type promotion) with the
//conversion specifier, and are used in the order given.
//
//@@The flag characters:
//The character @|%| is followed by zero or more of the following flags:
//\begin{itemize}
//  \item @|\#|   The  value  should be converted to an ``alternate form''.  For @|o| conversions, the first character of the output  string  is  made  zero (by prefixing a @|0| if it was not zero already).  For @|x| and @|X| conversions, a nonzero result has the string @|'0x'| (or @|'0X'| for  @|X|  conversions) prepended to it.  For @|a, A, e, E, f, F, g,| and @|G|  conversions, the result will always  contain  a  decimal  point,  even  if  no digits follow it (normally, a decimal point appears  in the results of those conversions only if  a  digit  follows).  For @|g| and @|G| conversions, trailing zeros are not removed from the  result as they would otherwise be.  For other  conversions,  the  result is undefined.
//  \item @|0|   The value should be zero padded.  For @|d, i, o, u, x, X, a, A, e, E, f, F, g,| and @|G| conversions, the converted value is padded  on the  left  with  zeros rather than blanks.  If the @|0| and @|-| flags  both appear, the @|0| flag is ignored.  If  a  precision  is  given  with  a numeric conversion @|(d, i, o, u, x, and X)|, the @|0| flag is  ignored.  For other conversions, the behavior is undefined.
//  \item @|-|   The converted value is to be left adjusted on the  field  boundary.  (The default is right justification.) Except for @|n| conversions, the converted value is padded on the right  with  blanks, rather than on the left with blanks or zeros.  A @|-| overrides a @|0| if both are given.
//  \item @|' '| (a space) A blank should be left before a  positive  number  (or empty string) produced by a signed conversion.
//  \item @|+| A  sign  (@|+| or @|-|) always be placed before a number produced by a signed conversion.  By default a sign is used only for  negative numbers. A @|+| overrides a space if both are used.
//\end{itemize}
//@@The field width:
//An  optional decimal digit string (with nonzero first digit) specifying a 
//minimum field width.  If the converted  value  has  fewer  characters than 
//the  field  width,  it will be padded with spaces on the left (or right, 
//if the left-adjustment flag has been given).  A  negative  field width is 
//taken as a @|'-'| flag followed by a positive field  width. In no case does 
//a non-existent or small field width cause truncation of a field; if the 
//result of a conversion is wider than the  field  width, the field is 
//expanded to contain the conversion result.
//
//@@The precision:
//
//An  optional  precision,  in the form of a period (@|'.'|)  followed by an optional decimal digit string.  If the precision is given as just @|'.'|, or the precision is negative, the precision is  taken  to  be zero.   This  gives the minimum number of digits to appear for @|d, i, o, u, x|, and @|X| conversions, the number of digits to appear after the radix character  for  @|a, A, e, E, f|, and @|F| conversions, the maximum number of significant digits for @|g| and @|G| conversions, or the  maximum  number  of  characters to be printed from a string for s conversions.
//
//@@The conversion specifier:
//
//A character that specifies the type of conversion to be  applied.   The
//conversion specifiers and their meanings are:
//\begin{itemize}
//\item @|d,i|   The  int  argument is converted to signed decimal notation.  The  precision, if any, gives the minimum number of digits that  must   appear;  if  the  converted  value  requires fewer digits, it is    padded on the left with zeros. The default precision is @|1|.  When @|0|  is printed with an explicit precision @|0|, the output is empty.
//\item @|o,u,x,X|   The unsigned int argument is converted to  unsigned  octal  (@|o|),  unsigned  decimal  (@|u|),  or unsigned hexadecimal (@|x| and @|X|) notation.  The letters @|abcdef| are used for @|x| conversions;  the  letters @|ABCDEF| are used for @|X| conversions.  The precision, if any,  gives the minimum number of digits that must appear; if the converted  value  requires  fewer  digits, it is padded on the left  with zeros. The default precision is @|1|.  When @|0| is printed  with  an explicit precision @|0|, the output is empty.
//\item @|e,E|    The  double  argument  is  rounded  and  converted  in the style  @|[-]d.ddde dd| where there is one digit before  the  decimal-point  character and the number of digits after it is equal to the precision; if the precision is missing, it is taken as  @|6|;  if  the    precision  is  zero,  no  decimal-point character appears.  An @|E|  conversion uses the letter @|E| (rather than @|e|)  to  introduce  the  exponent.   The exponent always contains at least two digits; if  the value is zero, the exponent is @|00|.
//\item @|f,F|   The double argument is rounded and converted to decimal notation  in  the  style  @|[-]ddd.ddd|, where the number of digits after the decimal-point character is equal to the precision specification.  If  the precision is missing, it is taken as @|6|; if the precision  is explicitly zero, no decimal-point character  appears.   If  a   decimal point appears, at least one digit appears before it.
//\item @|g,G|   The double argument is converted in style @|f| or @|e| (or @|F| or @|E|  for  @|G|  conversions).  The precision specifies the number of significant digits.  If the precision is missing, @|6| digits  are  given;  if  the  precision is zero, it is treated as @|1|.  Style e is used   if the exponent from its conversion is less than @|-4|  or  greater than or equal to the precision.  Trailing zeros are removed from  the fractional part of the result; a decimal point appears  only  if it is followed by at least one digit.
//\item @|c| The int argument is  converted  to  an  unsigned  char, and  the resulting character is written.
//\item @|s| The string argument is printed.
//\item @|%|   A @|'%'| is written. No argument is converted. The complete conversion specification is @|'%%'|.
//\end{itemize}
//@@Example
//Here are some examples of the use of @|printf| with various arguments.  First we print out an integer and double value.
//@<
//printf('intvalue is %d, floatvalue is %f\n',3,1.53);
//@>
//Next, we print out a string value.
//@<
//printf('string value is %s\n','hello');
//@>
//Now, we print out an integer using 12 digits, zeros up front.
//@<
//printf('integer padded is %012d\n',32);
//@>
//Print out a double precision value with a sign, a total of 18 characters (zero prepended if necessary), a decimal point, and 12 digit precision.
//@<
//printf('float value is %+018.12f\n',pi);
//@>
//@@Signature
//sfunction printf PrintfFunction
//inputs varargin
//outpus none
//!
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("printf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("printf format argument must be a string");
  eval->outputMessage(ConvertEscapeSequences(XprintfFunction(nargout,arg)));
  return ArrayVector();
}

//!
//@Module DISP Display a Variable or Expression
//@@Section IO
//@@Usage
//Displays the result of a set of expressions.  The @|disp| function
//takes a variable number of arguments, each of which is an expression
//to output:
//@[
//  disp(expr1,expr2,...,exprn)
//@]
//This is functionally equivalent to evaluating each of the expressions
//without a semicolon after each.
//@@Example
//Here are some simple examples of using @|disp|.
//@<
//a = 32;
//b = 1:4;
//disp(a,b,pi)
//@>
//@@Signature
//sfunction disp DispFunction
//inputs varargin
//outputs none
//!
ArrayVector DispFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  for (int i=0;i<arg.size();i++) 
    PrintArrayClassic(arg[i],eval->getPrintLimit(),eval);
  return ArrayVector();
} 

ArrayVector ScanfFunction(QFile *fp, QString format) {
  char *buff = strdup(qPrintable(format));
  //#error - Invalid use of QTS
  //  QTextStream in(fp);
  // Search for the start of a format subspec
  char *dp = buff;
  char *np;
  char sv;
  char dum;
  bool shortarg;
  bool doublearg;
  // Scan the string
  ArrayVector values;
  while (*dp) {
    np = dp;
    while ((*dp) && (*dp != '%')) dp++;
    // Print out the formatless segment
    sv = *dp;
    *dp = 0;
    for (int i=0;i<strlen(np);i++) 
      fp->getChar(&dum);
    if (fp->atEnd())
      values.push_back(EmptyConstructor());
    *dp = sv;
    // Process the format spec
    if (*dp) {
      np = validateScanFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + QString(dp));
      else {
	if (*(np-1) == '%') {
	  fp->getChar(&dum);
	  dp+=2;
	} else {
	  shortarg = false;
	  doublearg = false;
	  if (*(np-1) == 'h') {
	    shortarg = true;
	    np++;
	  } else if (*(np-1) == 'l') {
	    doublearg = true;
	    np++;
	  } 
	  sv = *np;
	  *np = 0;
	  switch (*(np-1)) {
	  case 'd':
	  case 'i':
	    values.push_back(Array(QFileReadInteger(fp,0)));
	    break;
	  case 'o':
	    values.push_back(Array(QFileReadInteger(fp,8)));
	    break;
	  case 'u':
	    values.push_back(Array(QFileReadInteger(fp,10)));
	    break;
	  case 'x':
	  case 'X':
	    values.push_back(Array(QFileReadInteger(fp,16)));
	    break;
	  case 'c':
	    values.push_back(Array(QFileReadInteger(fp,10)));
	    break;
	  case 'e':
	  case 'E':
	  case 'f':
	  case 'F':
	  case 'g':
	  case 'G':
	    values.push_back(Array(QFileReadFloat(fp)));
	    break;
	  case 's': 
	    {
	      QByteArray r;
	      char t;
	      while (fp->getChar(&t) && !isspace(t))
		r.push_back(t);
	      fp->ungetChar(t);
	      values.push_back(Array(QTextCodec::codecForLocale()->
				     toUnicode(r)));
	      break;
	    }
	  default:
	    throw Exception("unsupported fscanf configuration");
	  }
	  *np = sv;
	  dp = np;
	}
      }
    }
  }
  free(buff);
  return values;
}
