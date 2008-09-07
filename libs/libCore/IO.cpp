/*
 * Copyright (c) 2002-2006 Samit Basu
 * Copyright (c) 2006 Thomas Beutlich
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
#include <string>
#include "HandleList.hpp"
#include "Interpreter.hpp"
#include "Serialize.hpp"
#include "Print.hpp"
#include <QImageWriter>
#include <QString>
#include "Utils.hpp"
#include "MatIO.hpp"
#include <QtCore>
#include "Algorithms.hpp"


class PrintfStream{
public:
    virtual PrintfStream& operator <<( const char* data ) = 0;
};

class PrintfFileStream : public PrintfStream{
private:
    FILE* fp;
public:
    PrintfFileStream( FILE* fp_ ) : fp(fp_){};
    virtual PrintfFileStream& operator <<( const char* data ){
	fprintf( fp, "%s", data );
	return *this;
    };
};

class PrintfStringStream : public PrintfStream{
private:
    std::string* str;
public:
    PrintfStringStream( std::string* str_ ) : str(str_){};
    virtual PrintfStringStream& operator <<( const char* data ){
	*str += std::string( data );
	return *this;
    };

//!
//@Module FORMAT Control the Format of Matrix Display
//@@Section IO
//@@Usage
//FreeMat supports several modes for displaying matrices (either through the
//@|disp| function or simply by entering expressions on the command line.  
//There are several options for the format command.  The default mode is equivalent
//to
//@[
//   format short
//@]
//which generally displays matrices with 4 decimals, and scales matrices if the entries
//have magnitudes larger than roughly @|1e2| or smaller than @|1e-2|.   For more 
//information you can use 
//@[
//   format long
//@]
//which displays roughly 7 decimals for @|float| and @|complex| arrays, and 14 decimals
//for @|double| and @|dcomplex|.  You can also use
//@[
//   format short e
//@]
//to get exponential format with 4 decimals.  Matrices are not scaled for exponential 
//formats.  Similarly, you can use
//@[
//   format long e
//@]
//which displays the same decimals as @|format long|, but in exponential format.
//You can also use the @|format| command to retrieve the current format:
//@[
//   s = format
//@]
//where @|s| is a string describing the current format.
//@@Example
//We start with the short format, and two matrices, one of double precision, and the
//other of single precision.
//@<
//format short
//a = randn(4)
//b = float(randn(4))
//@>
//Note that in the short format, these two matrices are displayed with the same format.
//In @|long| format, however, they display differently
//@<
//format long
//a
//b
//@>
//Note also that we we scale the contents of the matrices, FreeMat rescales the entries
//with a scale premultiplier.
//@<
//format short
//a*1e4
//a*1e-4
//b*1e4
//b*1e-4
//@>
//Next, we use the exponential formats:
//@<
//format short e
//a*1e4
//a*1e-4
//b*1e4
//b*1e-4
//@>
//Finally, if we assign the @|format| function to a variable, we can retrieve the 
//current format:
//@<
//format short
//t = format
//@>
//@@Signature
//function format FormatFunction
//inputs format exptype
//outputs format
//!
ArrayVector FormatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() > 0) {
    QString argtxt;
    for (int i=0;i<arg.size();i++) argtxt += arg[i].asString().toUpper();
    if (argtxt == "NATIVE") SetPrintFormatMode(format_native);
    else if (argtxt == "SHORT") SetPrintFormatMode(format_short);
    else if (argtxt == "LONG") SetPrintFormatMode(format_long);
    else if (argtxt == "SHORTE") SetPrintFormatMode(format_short_e);
    else if (argtxt == "LONGE") SetPrintFormatMode(format_long_e);
    else throw Exception("unrecognized argument to the format command");
  }
  if (nargout > 0) {
    switch(GetPrintFormatMode()) {
    case format_native:
      return ArrayVector(Array(QString("native")));
    case format_short:
      return ArrayVector(Array(QString("short")));
    case format_long:
      return ArrayVector(Array(QString("long")));
    case format_short_e:
      return ArrayVector(Array(QString("short e")));
    case format_long_e:
      return ArrayVector(Array(QString("long e")));
    }
    return ArrayVector(Array(QString("unknown?")));
  }
  return ArrayVector();
}


//!
//@Module SETPRINTLIMIT Set Limit For Printing Of Arrays
//@@Section IO
//@@Usage
//Changes the limit on how many elements of an array are printed
//using either the @|disp| function or using expressions on the
//command line without a semi-colon.  The default is set to 
//one thousand elements.  You can increase or decrease this
//limit by calling
//@[
//  setprintlimit(n)
//@]
//where @|n| is the new limit to use.
//@@Example
//Setting a smaller print limit avoids pages of output when you forget the semicolon on an expression.
//@<
//A = randn(512);
//setprintlimit(10)
//A
//setprintlimit(1000)
//@>
//@@Signature
//sfunction setprintlimit SetPrintLimitFunction
//inputs linecount
//outputs none
//!
ArrayVector SetPrintLimitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("setprintlimit requires one, scalar integer argument");
  eval->setPrintLimit(arg[0].asInteger());
  return ArrayVector();
}

//!
//@Module GETPRINTLIMIT Get Limit For Printing Of Arrays
//@@Section IO
//@@Usage
//Returns the limit on how many elements of an array are printed
//using either the @|disp| function or using expressions on the
//command line without a semi-colon.  The default is set to 
//one thousand elements.  You can increase or decrease this
//limit by calling @|setprintlimit|.  This function is provided
//primarily so that you can temporarily change the output truncation
//and then restore it to the previous value (see the examples).
//@[
//   n=getprintlimit
//@]
//where @|n| is the current limit in use.
//@@Example
//Here is an example of using @|getprintlimit| along with @|setprintlimit| to temporarily change the output behavior of FreeMat.
//@<
//A = randn(100,1);
//n = getprintlimit
//setprintlimit(5);
//A
//setprintlimit(n)
//@>
//@@Signature
//sfunction getprintlimit GetPrintLimitFunction
//inputs none
//outputs linecount
//!
ArrayVector GetPrintLimitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(double(eval->getPrintLimit())));

class PrintfDataServer{
private:
    const ArrayVector arg;
    int vec_ind;
    int elem_ind;
    bool hasMoreData;
    void IncDataPtr(void){
	if( ++elem_ind >= arg[ vec_ind ].getLength() ){
	    if( ++vec_ind < arg.size() ){
		elem_ind = 0;
	    }
	    else{
		hasMoreData = false;
	    }
	}
    };
    Array GetCurrentArg(void){
    
    };
  
public:
    PrintfDataServer( const ArrayVector& arg_ ):arg(arg_),vec_ind(1),elem_ind(0){
	//vec_ind starts with 1, because zeroth argument is format string
	hasMoreData = (arg.size() > 1); //( (arg.size() > 1) && (arg[1].getLength() > 0));
    };

    void GetNextAsDouble(double& data){
	if( !hasMoreData )
	    throw Exception("Error: no more data");
	Array d(arg[ vec_ind ]);
	d.promoteType( FM_DOUBLE );
	const double *pVal = (double const*)d.getDataPointer();
	data = *(pVal + elem_ind);
	IncDataPtr();
    };

    void GetNextAsString(std::string& str){
	if( !hasMoreData )
	    throw Exception("Error: no more data");
	Array d(arg[ vec_ind ]);
	
	if( d.dataClass()== FM_STRING ){
	    const char *pVal = (char const*)d.getDataPointer();
	    while( elem_ind < d.getLength() ){
		str.push_back(*(pVal + elem_ind++));
	    }
	}else{
	    d.promoteType( FM_STRING );
	    const char *pVal = (char const*)d.getDataPointer();
	    str.push_back(*(pVal + elem_ind));
	}
	IncDataPtr();
    };
    bool HasMoreData(void){ return hasMoreData; };
};

//Common routine used by sprintf,printf,fprintf.  They all
//take the same inputs, and output either to a string, the
//console or a file.  For output to a console or a file, 
//we want escape-translation on.  For output to a string, we
//want escape-translation off.  So this common routine prints
//the contents to a string, which is then processed by each 
//subroutine.
void PrintfHelperFunction(int nargout, const ArrayVector& arg, PrintfStream& output, QByteArray& errmsg, Array& ret, bool convEscape = false ) 
{
    Array format(arg[0]);
    std::string frmt = format.getContentsAsString();

    std::vector<char> buff( frmt.length()+1 ); //vectors are contiguous in memory. we'll use it instead of char[]
    strncpy(&buff[0], frmt.c_str(), frmt.length()+1 );

    // Search for the start of a format subspec
    char *dp = &buff[0];
    char *np;
    char sv;
    int nprn = 0;
    int noutput = 0;
    // Buffer to hold each sprintf result
#define BUFSIZE 65534
    char nbuff[BUFSIZE+1];
    memset( nbuff, 0, BUFSIZE+1 );

    // Buffer to hold the output
    //output.clear();
    errmsg.clear();

    PrintfDataServer ps( arg );

    //do while there is still data to output or format string to save
    while( (*dp) || ps.HasMoreData() ) {
	if ( !(*dp) && ps.HasMoreData() ) //still have arguments, need to rewind format.
	    dp = &buff[0];

	np = dp;
	int nbuf_ind = 0;
	//copy string upto formatting character and do escape conversion in the process
	while ((*dp) && (*dp != '%') && nbuf_ind < BUFSIZE ){ 
	    if (convEscape && isEscape(dp)) {
		switch (*(dp+1)) {
		  case '\\':
		      *(nbuff+nbuf_ind) = '\\';
		      break;
		  case 'n':
		      *(nbuff+nbuf_ind) = '\n';
		      break;
		  case 't':
		      *(nbuff+nbuf_ind) = '\t';
		      break;
		  case 'r':
		      *(nbuff+nbuf_ind) = '\r';
		      break;
		}
		dp += 2; ++nbuf_ind;
	    } else
		*(nbuff+nbuf_ind++) = *(dp++);
	}

	// Print out the formatless segment
	*(nbuff+nbuf_ind) = 0;
	nprn = nbuf_ind; noutput += nbuf_ind;
	output << nbuff;    

	
	// Process the format spec
	if (*dp == '%' && *(dp+1)) {
	    np = validateFormatSpec(dp+1);
	    if (!np)
		throw Exception("erroneous format specification " + std::string(dp));
	    else {
		if (*(np-1) == '%') {
		    nprn = snprintf(nbuff,BUFSIZE,"%%"); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
		    output << nbuff;    
		    sv=0;
		} else 
		if( *(np-1) == 's') {
		    std::string str;
		    ps.GetNextAsString( str );
		    const char* pStr = str.c_str();
		    sv = *np;
		    *np = 0;
		    nprn = snprintf(nbuff,BUFSIZE,dp,pStr); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
		    output << nbuff;
		} else{
		    sv = *np;
		    *np = 0;

		    double data;
		    ps.GetNextAsDouble( data );

		    switch (*(np-1)) 
		    {
		    case 'd':
		    case 'i':
		    case 'o':
		    case 'u':
		    case 'x':
		    case 'X':
		    case 'c':
			nprn = snprintf(nbuff,BUFSIZE,dp,(int32)data); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
			output << nbuff;
			break;
		    case 'e':
		    case 'E':
		    case 'f':
		    case 'F':
		    case 'g':
		    case 'G':
			nprn = snprintf(nbuff,BUFSIZE,dp,data); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
			output << nbuff;
			break;
		    }
		}
		if( sv )
		    *np = sv;
		dp = np;
	    }
	}
    }
    ret = Array::doubleConstructor( noutput );
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
//@@Tests
//@$"y=sprintf('hello %d',5)","'hello 5'","exact"
//@$"y=sprintf('%d aa %s',5,'bcd')","'5 aa bcd'","exact"
//@$"y=sprintf('%d %%aa %s %f',5,'bcd',5)","'5 %aa bcd 5.000000'","exact"
//@$"y=sprintf('%d aa ',[5 6; 7 8])","'5 aa 7 aa 6 aa 8 aa '","exact"
//!
ArrayVector SprintfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("sprintf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("sprintf format argument must be a string");


  std::string outf;
  PrintfStringStream textstream( &outf );
  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, arg, textstream, errmsg, output, true );
  ArrayVector ret;
 
  ret << Array::stringConstructor( outf );
  ret << output;
  return ret;
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
//!
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("printf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("printf format argument must be a string");

  std::string outf;
  PrintfStringStream textstream( &outf );

  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, arg, textstream, errmsg, output, true );
  ArrayVector ret;
 
  eval->outputMessage( outf );
  return ArrayVector();
}

//!
//@Module FGETS Read a String from a File
//@@Section IO
//@@Usage
//Reads a string from a file including the line end character.  The general syntax for its use
//is
//@[
//  s = fgets(handle)
//@]
//This function reads characters from the file @|handle| into
//a @|string| array @|s| until it encounters the end of the file
//or a newline.  The newline, if any, is retained in the output
//string.  If the file is at its end, (i.e., that @|feof| would
//return true on this handle), @|fgets| returns an empty
//string.
//@@Example
//First we write a couple of strings to a test file.
//@<
//fp = fopen('testtext','w');
//fprintf(fp,'String 1\n');
//fprintf(fp,'String 2\n');
//fclose(fp);
//@>
//Next, we read then back.
//@<
//fp = fopen('testtext','r')
//fgets(fp)
//fgets(fp)
//fclose(fp);
//@>
//!
ArrayVector FgetsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("fgetline takes one argument, the file handle");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  char buffer[65535];
  fgets(buffer,sizeof(buffer),fptr->fp);
  if (feof(fptr->fp))
    return SingleArrayVector(Array::emptyConstructor());
  return SingleArrayVector(Array::stringConstructor(buffer));
}

ArrayVector FgetlineFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
	//eval->warningMessage( "fgetline is deprecated. Use fgets instead." );
	return FgetsFunction( nargout, arg );
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
//!
ArrayVector Str2NumFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("str2num takes a single argument, the string to convert into a number");
  Array data(arg[0]);
  if (!data.isString())
    throw Exception("the first argument to str2num must be a string");
  return ArrayVector() << Array::doubleConstructor(ArrayToDouble(data));
}

ArrayVector ScanfHelperFunction( FILE* fp, const ArrayVector& arg )
{
  Array format(arg[0]);
  Array errormsg = Array::stringConstructor( "" );
  Array dims;
  int firstdim = -1;
  int nMaxRead = -1, nRead = 0;

  if( arg.size() == 2 ){
      dims = arg[1];

      if( !(dims.isIntegerClass() || dims.isReal()) || !dims.isPositive() || ( dims.getLength()>2 ))
          throw Exception("dimension should be integer scalar or vector");
  
      dims.promoteType( FM_DOUBLE );
      const double *dim = (const double *)dims.getDataPointer();
      if( dims.getLength() == 1 ){
	  if( IsFinite( dim[0] ) ){
	      nMaxRead = dim[0];
	      firstdim = dim[0];
	  }
      }
      else{
	  if( !IsFinite( dim[0] ) )
	      throw Exception("Illegal size.");
	  firstdim = dim[0];
	  if( IsFinite( dim[1] ) )
	      nMaxRead = dim[0]*dim[1];
      }
  }

  std::string frmt = format.getContentsAsString();

  std::vector<char> buff( frmt.length()+1 ); //vectors are contiguous in memory. we'll use it instead of char[]
  strncpy(&buff[0], frmt.c_str(), frmt.length()+1 );
  
  // Search for the start of a format subspec
  char *dp = &buff[0];
  char *np;
  char sv;
  bool shortarg;
  bool doublearg;
  bool stringarg = true;
  int nNumRead = 0;
  int nCharRead = 0;

  // Scan the string
  ArrayVector values;
  while( !feof( fp ) && ( nMaxRead < 0 || nRead < nMaxRead ) ){
    if ( !(*dp) ) //rewind the format
	dp = &buff[0];
    np = dp;
    while ((*dp) && (*dp != '%')) dp++;
    // Print out the formatless segment
    sv = *dp;
    *dp = 0;
    fscanf(fp,np);
    if (feof(fp))
      values.push_back(Array::emptyConstructor());
    *dp = sv;
    // Process the format spec
    if (*dp) {
      np = validateScanFormatSpec(dp+1);
      if (!np){
	  errormsg=Array::stringConstructor("erroneous format specification " + std::string(dp));
	  goto exit;
      }
      else {
	if (*(np-1) == '%') {
	  fscanf(fp,"%%");
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
	    if (shortarg) {
	      short sdumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&sdumint);
		values.push_back(Array::int16Constructor(sdumint));
		stringarg = false; ++nNumRead; ++nRead;
	      }
	    } else {
	      int sdumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&sdumint);
		values.push_back(Array::int32Constructor(sdumint));
		stringarg = false; ++nNumRead; ++nRead;
	      }
	    }
	    break;
	  case 'o':
	  case 'u':
	  case 'x':
	  case 'X':
	  case 'c':
	    if (shortarg) {
	      int sdumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&sdumint);
		values.push_back(Array::int32Constructor(sdumint));
		stringarg = false; ++nNumRead; ++nRead;
	      }
	    } else {
	      unsigned int dumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&dumint);
		values.push_back(Array::uint32Constructor(dumint));
		stringarg = false; ++nNumRead; ++nRead;
	      }
	    }
	    break;
	  case 'e':
	  case 'E':
	  case 'f':
	  case 'F':
	  case 'g':
	  case 'G':
	    if (doublearg) {
	      double dumfloat;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&dumfloat);
		values.push_back(Array::doubleConstructor(dumfloat));
		stringarg = false; ++nNumRead; ++nRead;
	      }
	    } else {
	      float dumfloat;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&dumfloat);
		values.push_back(Array::floatConstructor(dumfloat));
		stringarg = false; ++nNumRead; ++nRead;
	      }
	    }
	    break;
	  case 's':
	    char stbuff[4096];
	    if (feof(fp))
	      values.push_back(Array::emptyConstructor());
	    else {
	      fscanf(fp,dp,stbuff);
	      values.push_back(Array::stringConstructor(stbuff));
	      nCharRead += strlen( stbuff ); ++nRead;
	    }
	    break;
	  default:
	      errormsg = Array::stringConstructor("illegal format");
	      goto exit;
	  }
	  *np = sv;
	  dp = np;
	}
      }
    }
  }
exit:
  ArrayVector ret; 
  ret.resize(4); //sscanf returns 4 parameters (array, numread, errormsg, nextind)
  ret[3] = Array::doubleConstructor( ftell( fp ) );
  ret[2] = errormsg;
  ret[1] = Array::doubleConstructor( nRead );
  
  int nVecElem; //number of elements in the output vector. Unfortunately may be different from the number of elements we read in.
  nVecElem = nNumRead + nCharRead;
  //the tricky case: due to string conversion we actually have more data than we expected. 
  if( firstdim > 0 && (((int)(nNumRead + nCharRead)) % ((int) firstdim ))){
    nVecElem = (((int)(nNumRead + nCharRead)) / ((int) firstdim )) * firstdim + firstdim;
  }

  if( !stringarg ){ //At least one of the read values was numerical. Convert strings to numbers.
      ret[0] = Array::doubleVectorConstructor( nVecElem );
      double *data = (double*) ret[0].getReadWriteDataPointer();

      ArrayVector::iterator it = values.begin();
      while( it != values.end() ){
	  if( it->isString() ){
	    const char* strdata = (const char*) it->getDataPointer();
	    for( int j=0; j < it->getLength(); j++ ){
	      *(data++) = strdata[j];
	    }
	  }
	  else{
	      *(data++) = it->getContentsAsDoubleScalar();
	  }
	  ++it;
      }
  }
  else{ //all string input
      std::string outstring;

      ArrayVector::iterator it = values.begin();
      while( it != values.end() ){
	  if( it->isString() ){
	    outstring.append( it->getContentsAsString() );
	  }
	  else{
	    throw Exception("Internal Error in sscanf.");
	  }
	  ++it;
      }
      ret[0] = Array::stringConstructor( outstring );
  }
  if( firstdim > 0 ){
    Dimensions dim( firstdim, nVecElem / firstdim ); 
    ret[0].reshape( dim );
  }
  else if( !stringarg ){
    Dimensions dim( nNumRead + nCharRead, 1 ); 
    ret[0].reshape( dim );
  }
  return ret;
}

// How does sscanf work...
// a whitespace matches any number/type of whitespace
// a non-whitespace must be matched exactly
// a %d, %ld, etc.  reads a number... 
// a %s matches a sequence of characters that does
// not contain whitespaces...
// a mismatch - 

//!
//@Module SSCANF Formated String Input Function (C-Style)
//@@Section IO
//@@Usage
//Reads values from a string.  The general syntax for its use is
//@[
//  [a, count, errmesg, nextindex] = sscanf(text,format,size)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the input.  Each value that is parsed
//from the @|text| occupies one output slot.  See @|printf|
//for a description of the format. @|size| is in the form n or [m, n].
//!

class AutoFileCloser {
  FILE *fp;
public:
  AutoFileCloser(FILE *g) {fp = g;}
  ~AutoFileCloser() {fclose(fp);}
};

ArrayVector SscanfFunction(int nargout, const ArrayVector& arg) {
  if ((arg.size() > 3) || (arg.size() < 2) || (!arg[0].isString()) || (!arg[1].isString()))
    throw Exception("incorrect number or types or the parameters");
  Array text(arg[0]);
  std::string txt = text.getContentsAsString();

  FILE *fp = tmpfile();
  AutoFileCloser afc(fp);
  if (!fp)
    throw Exception("sscanf was unable to open a temp file (and so it won't work)");
  fprintf(fp,"%s",txt.c_str());
  rewind(fp);
  if (feof(fp))
    return SingleArrayVector(Array::emptyConstructor());

  ArrayVector helper_arg; 
  helper_arg << arg[1];
  if( arg.size() == 3 )
      helper_arg << arg[2];
  return ScanfHelperFunction( fp, helper_arg );
}

//!
//@Module FSCANF Formatted File Input Function (C-Style)
//@@Section IO
//@@Usage
//Reads values from a file.  The general syntax for its use is
//@[
//  [a, count, errmsg, nextind] = fscanf(handle,format,size)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the input.  Each value that is parsed from
//the file described by @|handle| occupies one output slot.
//See @|printf| for a description of the format. @|size| is in the form n or [m, n].
//Note that if
//the file is at the end-of-file, the fscanf will return 
//!
ArrayVector FscanfFunction(int nargout, const ArrayVector& arg) {
  if ((arg.size() > 3) || (arg.size() < 2) || (!arg[1].isString()))
      throw Exception("incorrect number or types or the parameters");

  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (feof(fptr->fp))
    return SingleArrayVector(Array::emptyConstructor());

  ArrayVector helper_arg; 
  helper_arg << arg[1];
  if( arg.size() == 3 )
      helper_arg << arg[2];

  return ScanfHelperFunction( fptr->fp, helper_arg );
}


//!
//@Module FPRINTF Formated File Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to a file.  The general syntax for its use is
//@[
//  fprintf(fp,format,a1,a2,...).
//@]
//or, 
//@[
//  fprintf(format,a1,a2,...).
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|ai| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|fprintf| command is not vectorized!  Each
//variable must be a scalar.  The value @|fp| is the file handle.  If @|fp| is omitted,
//file handle @|1| is assumed, and the behavior of @|fprintf| is effectively equivalent to @|printf|. For
//more details on the format string, see @|printf|.
//@@Examples
//A number of examples are present in the Examples section of the @|printf| command.
//!
 ArrayVector FprintfFunction(int nargout, const ArrayVector& arg, 
			     Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("fprintf requires at least one (string) argument");
  ArrayVector argCopy(arg);
  int handle = 1;
  if (arg.size() > 1) {
    Array tmp(arg[0]);
    if (tmp.isScalar()) {
      handle = tmp.getContentsAsIntegerScalar();
      argCopy.pop_front();
    }
    else {
      handle=1;
    }
  }
  Array format(argCopy[0]);
  if (!format.isString())
    throw Exception("fprintf format argument must be a string");
  ArrayVector argcopy(arg);
  argcopy.pop_front();
  
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));

  PrintfFileStream textstream( fptr->fp );
  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, argcopy, textstream, errmsg, output, true );
  return ArrayVector() << output;
}

  
ArrayVector SaveNativeFunction(string filename, StringVector names, Interpreter* eval) {
  File ofile(filename,"wb");
  Serialize output(&ofile);
  output.handshakeServer();
  Context *cntxt = eval->getContext();
  for (int i=0;i<names.size();i++) {
    ArrayReference toWrite;
    char flags;
    if (!(names[i].compare("ans") == 0)) {
      toWrite = cntxt->lookupVariable(names[i]);
      if (!toWrite.valid())
	throw Exception(std::string("unable to find variable ")+
			names[i]+" to save to file "+filename);
      flags = 'n';
      if (cntxt->isVariableGlobal(names[i]))
	flags = 'g';
      if (cntxt->isVariablePersistent(names[i]))	
	flags = 'p';
      output.putString(names[i].c_str());
      output.putByte(flags);
      output.putArray(*toWrite);
    }
  }
  output.putString("__eof");
  return ArrayVector();
}
  
static ArrayVector SaveASCIIFunction(QString filename, StringVector names, bool tabsMode,
				     bool doubleMode, Interpreter* eval) {
  QFile fp(filename);
  if (!fp.open(QIODevice::WriteOnly))
    throw Exception("unable to open file " + filename + " for writing.");
  Context *cntxt = eval->getContext();
  for (int i=0;i<names.size();i++) {
    if (!(names[i] == "ans")) {
      ArrayReference toWrite = cntxt->lookupVariable(names[i]);
      if (!toWrite.valid())
	throw Exception("unable to find variable " + names[i] + 
			" to save to file "+filename);
      if (toWrite->isReferenceType()) {
	eval->warningMessage("variable " + names[i] + " is not numeric - cannot write it to an ASCII file");
	continue;
      }
      if (!toWrite->is2D()) {
	eval->warningMessage("variable " + names[i] + " is not 2D - cannot write it to an ASCII file");
	continue;
      }
      if (toWrite->isComplex()) 
	eval->warningMessage("variable " + names[i] + " is complex valued - only real part will be written to ASCII file");
      Array A(*toWrite); A = A.toClass(Double).asDenseArray();
      int rows = int(A.rows());
      int cols = int(A.columns());
      const BasicArray<double> &dp(A.constReal<double>());
      QTextStream out(&fp);
      if (doubleMode)
	out.setRealNumberPrecision(15);
      else
	out.setRealNumberPrecision(7);
      for (int i=0;i<rows;i++) {
	for (int j=0;j<cols;j++) {
	  out << dp[j*rows+i+1];
	  if (tabsMode && (j < (cols-1)))
	    out << "\t";
	  else
	    out << " ";
	}
	out << "\n";
      }
    }
  }
  return ArrayVector();
}

//!
//@Module SAVE Save Variables To A File
//@@Section IO
//@@Usage
//Saves a set of variables to a file in a machine independent format.
//There are two formats for the function call.  The first is the explicit
//form, in which a list of variables are provided to write to the file:
//@[
//  save filename a1 a2 ...
//@]
//In the second form,
//@[
//  save filename
//@]
//all variables in the current context are written to the file.  The 
//format of the file is a simple binary encoding (raw) of the data
//with enough information to restore the variables with the @|load|
//command.  The endianness of the machine is encoded in the file, and
//the resulting file should be portable between machines of similar
//types (in particular, machines that support IEEE floating point 
//representation).
//
//You can also specify both the filename as a string, in which case
//you also have to specify the names of the variables to save.  In
//particular
//@[
//   save('filename','a1','a2')
//@]
//will save variables @|a1| and @|a2| to the file.
//
//Starting with version 2.0, FreeMat can also read and write MAT
//files (the file format used by MATLAB) thanks to substantial 
//work by Thomas Beutlich.  Support for MAT files in version 2.1
//has improved over previous versions.  In particular, classes
//should be saved properly, as well as a broader range of sparse
//matrices.  Compression is supported for both reading and writing
//to MAT files.  MAT file support is still in the alpha stages, so 
//please be cautious with using it to store critical 
//data.  The file format is triggered
//by the extension.  To save files with a MAT format, simply
//use a filename with a ".mat" ending.
//
//The @|save| function also supports ASCII output.  This is a very limited
//form of the save command - it can only save numeric arrays that are
//2-dimensional.  This form of the @|save| command is triggered using
//@[
//   save -ascii filename var1 var 2
//@]
//although where @|-ascii| appears on the command line is arbitrary (provided
//it comes after the @|save| command, of course).  Be default, the @|save|
//command uses an 8-digit exponential format notation to save the values to
//the file.  You can specify that you want 16-digits using the
//@[
//   save -ascii -double filename var1 var2
//@]
//form of the command.  Also, by default, @|save| uses spaces as the 
//delimiters between the entries in the matrix.  If you want tabs instead,
//you can use
//@[
//   save -ascii -tabs filename var1 var2
//@]
//(you can also use both the @|-tabs| and @|-double| flags simultaneously).
//
//Finally, you can specify that @|save| should only save variables that
//match a particular regular expression.  Any of the above forms can be
//combined with the @|-regexp| flag:
//@[
//   save filename -regexp pattern1 pattern2
//@]
//in which case variables that match any of the patterns will be saved.
//@@Example
//Here is a simple example of @|save|/@|load|.  First, we save some 
//variables to a file.
//@< 
//D = {1,5,'hello'};
//s = 'test string';
//x = randn(512,1);
//z = zeros(512);
//who
//save loadsave.dat
//@>
//Next, we clear the variables, and then load them back from the file.
//@<
//clear D s x z
//who
//load loadsave.dat
//who
//@>
//@@Tests
//@{ test_save1.m
//% Test the save and load capability with cell arrays (bug 1581481)
//function test_val = test_save1
//   a{1} = 'bert';
//   save tmp.mat a
//   b = a;
//   load tmp.mat
//   test_val = strcomp(a{1},b{1});
//@}
//@@Signature
//sfunction save SaveFunction
//inputs varargin
//outputs none
//!
ArrayVector SaveFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  ArrayVector argCopy;
  if (arg.size() == 0) return ArrayVector();
  bool asciiMode = false;
  bool tabsMode = false;
  bool doubleMode = false;
  bool matMode = false;
  bool regexpMode = false;
  for (int i=0;i<arg.size();i++) {
    if (arg[i].isString()) {
      if (arg[i].asString().toUpper() == "-MAT")
	matMode = true;
      else if (arg[i].asString().toUpper() == "-ASCII")
	asciiMode = true;
      else if (arg[i].asString().toUpper() == "-REGEXP")
	regexpMode = true;
      else if (arg[i].asString().toUpper() == "-DOUBLE")
	doubleMode = true;
      else if (arg[i].asString().toUpper() == "-TABS")
	tabsMode = true;
      else
	argCopy << arg[i];
    } else
      argCopy << arg[i];
  }
  if (argCopy.size() < 1) throw Exception("save requires a filename argument");
  QString fname(argCopy[0].asString());
  if (!asciiMode && !matMode) {
    if (fname.endsWith(".mat",Qt::CaseInsensitive))
      matMode = true;
    if (fname.endsWith(".txt",Qt::CaseInsensitive))
      asciiMode = true;
  }
  StringVector names;
  for (int i=1;i<argCopy.size();i++) {
    if (!arg[i].isString())
      throw Exception("unexpected non-string argument to save command");
    names << argCopy[i].asString();
  }
  Context *cntxt = eval->getContext();
  StringVector toSave;
  if (regexpMode || (names.size() == 0)) {
    StringVector allNames = cntxt->listAllVariables();
    for (int i=0;i<(int)allNames.size();i++)
      if ((names.size() == 0) || contains(names,allNames[i],regexpMode))
	toSave << allNames[i];
  } else 
    toSave = names;
  if (matMode)
    return MatSaveFunction(fname,toSave,eval);
  else if (asciiMode)
    return SaveASCIIFunction(fname,toSave,tabsMode,doubleMode,eval);
  else
    return SaveNativeFunction(fname,toSave,eval);
}

static int ParseNumber(QString tx) {
  int lookahead = 0;
  int len = 0;
  if ((tx[len] == '+') || (tx[len] == '-'))
    len++;
  lookahead = len;
  while (tx[len].isDigit()) len++;
  lookahead = len;
  if (tx[lookahead] == '.') {
    lookahead++;
    len = 0;
    while (tx[len+lookahead].isDigit()) len++;
    lookahead+=len;
  }
  if ((tx[lookahead] == 'E') || (tx[lookahead] == 'e')) {
    lookahead++;
    if ((tx[lookahead] == '+') || (tx[lookahead] == '-')) {
      lookahead++;
    }
    len = 0;
    while (tx[len+lookahead].isDigit()) len++;
    lookahead+=len;
  }
  return lookahead;
}

static void ParseComplexValue(QString tx, double &real, double &imag) {
  int lnum = ParseNumber(tx);
  int rnum = ParseNumber(tx.mid(lnum));
  QString num1 = tx.left(lnum);
  QString num2 = tx.mid(lnum,rnum);
  if (num1.isEmpty() && num2.isEmpty()) {
    real = 0; imag = 1;
    return;
  }
  if (num1 == "+") num1 = "+1";
  if (num2 == "+") num2 = "+1";
  if (num1 == "-") num1 = "-1";
  if (num2 == "-") num2 = "-1";
  if (num2.isEmpty()) {
    real = 0;
    imag = num1.toDouble();
  } else {
    real = num1.toDouble();
    imag = num2.toDouble();
  }
}

static int DecodeSpreadsheetColumn(QString tx) {
  tx.toUpper();
  QByteArray txb(tx.toLatin1());
  for (int i=0;i<txb.count();i++) 
    txb[i] = txb[i] - 'A';
  int ret = 0;
  for (int i=0;i<txb.count();i++) 
    ret += (int) (txb.at(i)*pow(26.0,txb.count()-1-i));
  return ret;
}

static void DecodeSpreadsheetRange(QString tx, int &startrow, int &startcol,
			    int &stoprow, int &stopcol) {
  QString colstart;
  QString rowstart;
  QString colstop;
  QString rowstop;
  while (tx.at(0).isLetter()) {
    colstart += tx.left(1);
    tx = tx.mid(1);
  }
  while (tx.at(0).isDigit()) {
    rowstart += tx.left(1);
    tx = tx.mid(1);
  }
  tx = tx.mid(1);
  tx = tx.mid(1);
  while (tx.at(0).isLetter()) {
    colstop += tx.left(1);
    tx = tx.mid(1);
  }
  while (tx.at(0).isDigit()) {
    rowstop += tx.left(1);
    tx = tx.mid(1);
  }
  startrow = rowstart.toInt()-1;
  stoprow = rowstop.toInt()-1;
  startcol = DecodeSpreadsheetColumn(colstart);
  stopcol = DecodeSpreadsheetColumn(colstop);
}

//!
//@Module DLMREAD Read ASCII-delimited File
//@@Section IO
//@@Usage
//Loads a matrix from an ASCII-formatted text file with a delimiter
//between the entries.  This function is similar to the @|load -ascii|
//command, except that it can handle complex data, and it allows you
//to specify the delimiter.  Also, you can read only a subset of the
//data from the file.  The general syntax for the @|dlmread| function
//is
//@[
//    y = dlmread(filename)
//@]
//where @|filename| is a string containing the name of the file to read.
//In this form, FreeMat will guess at the type of the delimiter in the 
//file.  The guess is made by examining the input for common delimiter
//characters, which are @|,;:| or a whitespace (e.g., tab).  The text
//in the file is preprocessed to replace these characters with whitespace
//and the file is then read in using a whitespace for the delimiter.
//
//If you know the delimiter in the file, you can specify it using
//this form of the function:
//@[
//    y = dlmread(filename, delimiter)
//@]
//where @|delimiter| is a string containing the delimiter.  If @|delimiter|
//is the empty string, then the delimiter is guessed from the file.
//
//You can also read only a portion of the file by specifying a start row
//and start column:
//@[
//    y = dlmread(filename, delimiter, startrow, startcol)
//@]
//where @|startrow| and @|startcol| are zero-based.  You can also specify
//the data to read using a range argument:
//@[
//    y = dlmread(filename, delimiter, range)
//@]
//where @|range| is either a vector @|[startrow,startcol,stoprow,stopcol]|
//or is specified in spreadsheet notation as @|B4..ZA5|.
//
//Note that complex numbers can be present in the file if they are encoded
//without whitespaces inside the number, and use either @|i| or @|j| as 
//the indicator.  Note also that when the delimiter is given, each incidence
//of the delimiter counts as a separator.  Multiple separators generate
//zeros in the matrix.
//@@Signature
//function dlmread DlmReadFunction
//inputs filename delimiter startrow startcol
//outputs y
//!
ArrayVector DlmReadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) 
    throw Exception("dlmread expects a filename");
  QFile ifile(arg[0].asString());
  if (!ifile.open(QFile::ReadOnly))
    throw Exception("filename " + arg[0].asString() + " could not be opened");
  bool no_delimiter = true;
  QString delimiter;
  if (arg.size() >= 2) {
    delimiter = arg[1].asString();
    no_delimiter = (delimiter.size() == 0);
  }
  int col_count = 0;
  int row_count = 0;
  QList<QList<double> > data_real;
  QList<QList<double> > data_imag;
  QTextStream str(&ifile);
  while (!str.atEnd()) {
    QString whole_line = str.readLine(0);
    QStringList line_pieces(whole_line.split("\r"));
    for (int i=0;i<line_pieces.size();i++) {
      QString line = line_pieces[i];
      QStringList elements;
      if (no_delimiter) {
	if (line.contains(QRegExp("[,;:]")))
	  elements = line.split(QRegExp("[,;:]"));
	else {
	  line = line.simplified();
	  elements = line.split(' ');
	}
      } else {
	elements = line.split(QString(delimiter)[0]);
      }
      QList<double> row_data_real;
      QList<double> row_data_imag;
      row_count++;
      for (int i=0;i<elements.size();i++) {
	QString element(elements[i]);
	element.replace(" ","");
	if (element.contains('i') || element.contains('I') ||
	    element.contains('j') || element.contains('J')) {
	  double real, imag;
	  ParseComplexValue(element,real,imag);
	  row_data_real << real;
	  row_data_imag << imag;
	} else {
	  row_data_real << element.toDouble();
	  row_data_imag << 0;
	}
      }
      col_count = qMax(col_count,elements.size());
      data_real << row_data_real;
      data_imag << row_data_imag;
    }
  }
  int startrow = 0;
  int startcol = 0;
  int stoprow = row_count-1;
  int stopcol = col_count-1;
  if (arg.size() == 4) {
    startrow = arg[2].asInteger();
    startcol = arg[3].asInteger();
  } else if (arg.size() == 3) {
    if (arg[2].isVector() && (arg[2].length() == 4)) {
      Array range(arg[2].asDenseArray().toClass(Int32));
      const BasicArray<int32> &dp(range.constReal<int32>());
      startrow = dp[1];
      startcol = dp[2];
      stoprow = dp[3];
      stopcol = dp[4];
    } else if (arg[2].isString()) 
      DecodeSpreadsheetRange(arg[2].asString(),startrow,startcol,stoprow,stopcol);
    else
      throw Exception("Unable to decode the range arguments to the dlmread function");
  }
  Array A;
  startrow = qMax(0,qMin(row_count-1,startrow));
  startcol = qMax(0,qMin(col_count-1,startcol));
  stoprow = qMax(0,qMin(row_count-1,stoprow));
  stopcol = qMax(0,qMin(col_count-1,stopcol));
  int numrows = stoprow-startrow+1;
  int numcols = stopcol-startcol+1;
  if ((numrows > 0) && (numcols > 0) && (row_count > 0) && (col_count > 0)) {
    bool anyNonzeroImaginary = false;
    for (int i=startrow;i<=stoprow;i++) 
      for (int j=0;j<=qMin(data_real[i].size()-1,stopcol);j++) 
	if (data_imag[i][j] != 0) anyNonzeroImaginary = true;
    if (!anyNonzeroImaginary) {
      A = Array(Double,NTuple(numrows,numcols));
      BasicArray<double> &dp(A.real<double>());
      for (int i=startrow;i<=stoprow;i++)
	for (int j=startcol;j<=stopcol;j++)
	  if (j <= (data_real[i].size()-1))
	    dp[i-startrow+(j-startcol)*numrows+1] = data_real[i][j];
    } else {
      A = Array(Double,NTuple(numrows,numcols));
      BasicArray<double> &dp(A.real<double>());
      BasicArray<double> &ip(A.imag<double>());
      for (int i=startrow;i<=stoprow;i++)
	for (int j=startcol;j<=stopcol;j++)
	  if (j <= (data_real[i].size()-1)) {
	    dp[i-startrow+(j-startcol)*numrows+1] = data_real[i][j];
	    ip[i-startrow+(j-startcol)*numrows+1] = data_imag[i][j];
	  }
    }
  }
  return ArrayVector(A);
}

static ArrayVector LoadASCIIFunction(int nargout, QString filename, Interpreter* eval) {
  // Hmmm...
  QFile ifile(filename);
  if (!ifile.open(QFile::ReadOnly))
    throw Exception("filename " + filename + " could not be opened");
  QTextStream str(&ifile);
  int i=0;
  int col_count = 0;
  int row_count = 0;
  QList<double> data;
  bool evenData = true;
  while (!str.atEnd() && evenData) {
    QString line = str.readLine(0);
    line = line.simplified();
    QStringList elements(line.split(' '));
    if (row_count == 0) 
      col_count = elements.size();
    else if (elements.size() != col_count)
      evenData = false;
    if (evenData) {
      row_count++;
      for (i=0;i<elements.size();i++) 
	data << elements[i].toDouble();
    }
  }
  if (!evenData)
    eval->warningMessage("data in ASCII file does not have a uniform number of entries per row");
  // Now construct the matrix
  Array A;
  if ((row_count > 0) && (col_count > 0)) {
    A = Array(Double,NTuple(row_count,col_count));
    BasicArray<double> &dp(A.real<double>());
    for (int r=0;r<row_count;r++) 
      for (int c=0;c<col_count;c++) 
	dp[r+c*row_count+1] = data.at(r*col_count+c);
  }
  if (nargout == 1)
    return ArrayVector(A);
  else {
    QFileInfo fi(filename);
    eval->getContext()->insertVariable(fi.baseName(),A);
  }
  return ArrayVector();
}

static ArrayVector LoadNativeFunction(int nargout, QString filename,
				      StringVector names, bool regexpmode, Interpreter* eval) {
  QFile ofile(filename);
  if (!ofile.open(QIODevice::ReadOnly))
    throw Exception("Unable to open " + filename + " to read data");
  Serialize input(&ofile);
  input.handshakeClient();
  QString arrayName = input.getString();
  StringVector fieldnames;
  ArrayVector fieldvalues;
  while (arrayName != "__eof") {
    Array toRead;
    char flag;
    flag = input.getByte();
    input.getArray(toRead);
    if ((names.size() == 0) || 
	(contains(names,arrayName,regexpmode) && (nargout == 0))) {
      switch (flag) {
      case 'n':
	break;
      case 'g':
	eval->getContext()->addGlobalVariable(arrayName);
	break;
      case 'p':
	eval->getContext()->addPersistentVariable(arrayName);
	break;
      default:
	throw Exception(QString("unrecognized variable flag ") + flag + 
			QString(" on variable ") + arrayName);
      }
      eval->getContext()->insertVariable(arrayName,toRead);
    } else {
      fieldnames << arrayName;
      fieldvalues << toRead;
    }
    arrayName = input.getString();
  }
  if (nargout == 0)
    return ArrayVector();
  else
    return ArrayVector(StructConstructor(fieldnames,fieldvalues));
}

//!
//@Module LOAD Load Variables From A File
//@@Section IO
//@@Usage
//Loads a set of variables from a file in a machine independent format.
//The @|load| function takes one argument:
//@[
//  load filename,
//@]
//or alternately,
//@[
//  load('filename')
//@]
//This command is the companion to @|save|.  It loads the contents of the
//file generated by @|save| back into the current context.  Global and 
//persistent variables are also loaded and flagged appropriately.  By
//default, FreeMat assumes that files that end in a @|.mat| or @|.MAT|
//extension are MATLAB-formatted files.  Also, FreeMat assumes that 
//files that end in @|.txt| or @|.TXT| are ASCII files. 
//For other filenames, FreeMat first tries to open the file as a 
//FreeMat binary format file (as created by the @|save| function).  
//If the file fails to open as a FreeMat binary file, then FreeMat 
//attempts to read it as an ASCII file.  
//
//You can force FreeMat to assume a particular format for the file
//by using alternate forms of the @|load| command.  In particular,
//@[
//  load -ascii filename
//@]
//will load the data in file @|filename| as an ASCII file (space delimited
//numeric text) loaded into a single variable in the current workspace
//with the name @|filename| (without the extension).
//
//For MATLAB-formatted data files, you can use
//@[
//  load -mat filename
//@]
//which forces FreeMat to assume that @|filename| is a MAT-file, regardless
//of the extension on the filename.
//
//You can also specify which variables to load from a file (not from 
//an ASCII file - only single 2-D variables can be successfully saved and
//retrieved from ASCII files) using the additional syntaxes of the @|load|
//command.  In particular, you can specify a set of variables to load by name
//@[
//  load filename Var_1 Var_2 Var_3 ...
//@]
//where @|Var_n| is the name of a variable to load from the file.  
//Alternately, you can use the regular expression syntax
//@[
//  load filename -regexp expr_1 expr_2 expr_3 ...
//@]
//where @|expr_n| is a regular expression (roughly as expected by @|regexp|).
//Note that a simpler regular expression mechanism is used for this syntax
//than the full mechanism used by the @|regexp| command.
//
//Finally, you can use @|load| to create a variable containing the 
//contents of the file, instead of automatically inserting the variables
//into the curent workspace.  For this form of @|load| you must use the
//function syntax, and capture the output:
//@[
//  V = load('arg1','arg2',...)
//@]
//which returns a structure @|V| with one field for each variable
//retrieved from the file.  For ASCII files, @|V| is a double precision
//matrix.
//
//@@Example
//Here is a simple example of @|save|/@|load|.  First, we save some variables to a file.
//@<
//D = {1,5,'hello'};
//s = 'test string';
//x = randn(512,1);
//z = zeros(512);
//who
//save loadsave.dat
//@>
//Next, we clear the variables, and then load them back from the file.
//@<
//clear D s x z
//who
//load loadsave.dat
//who
//@>
//@@Signature
//sfunction load LoadFunction
//inputs varargin
//outputs y
//!
ArrayVector LoadFunction(int nargout, const ArrayVector& arg, 
			 Interpreter* eval) {
  // Process the arguments to extract the "-mat", "-ascii" and "-regexp" 
  // flags.
  ArrayVector argCopy;
  if (arg.size() == 0) return ArrayVector();
  bool asciiMode = false;
  bool matMode = false;
  bool regexpMode = false;
  for (int i=0;i<arg.size();i++) {
    if (arg[i].isString()) {
      if (arg[i].asString().toUpper() == "-MAT")
	matMode = true;
      else if (arg[i].asString().toUpper() == "-ASCII")
	asciiMode = true;
      else if (arg[i].asString().toUpper() == "-REGEXP")
	regexpMode = true;
      else
	argCopy << arg[i];
    } else
      argCopy << arg[i];
  }
  if (argCopy.size() < 1)  throw Exception("load requries a filename argument");
  QString fname(argCopy[0].asString());
  // If one of the filemode flags has not been specified, try to 
  // guess if it is an ASCII file or a MAT file
  if (!matMode && !asciiMode) {
    if (fname.endsWith(".mat",Qt::CaseInsensitive))
      matMode = true;
    if (fname.endsWith(".txt",Qt::CaseInsensitive)) {
      asciiMode = true;
    } else {
      // Could be an ASCII file - try to open it native
      try {
	QFile of(fname);
	if (!of.open(QIODevice::ReadOnly))
	  throw Exception("nope");
	Serialize input(&of);
	input.handshakeClient();
      } catch(Exception& e) {
	asciiMode = true;
      }
    }
  }
  StringVector names;
  for (int i=1;i<argCopy.size();i++) {
    if (!arg[i].isString())
      throw Exception("unexpected non-string argument to load command");
    names << argCopy[i].asString();
  }
  // Read the data file using the appropriate handler
  try {
    if (matMode)
      return MatLoadFunction(nargout,fname,names,regexpMode,eval);
    else if (asciiMode)
      return LoadASCIIFunction(nargout,fname,eval);
    else
      return LoadNativeFunction(nargout,fname,names,regexpMode,eval);
  } catch (Exception& e) {
    throw Exception("unable to read data from file " + fname + " - it may be corrupt, or FreeMat may not understand the format.  See help load for more information.  The specific error was: " + e.msg());
  }
  return ArrayVector();
}

//!
//@Module GETLINE Get a Line of Input from User
//@@Section IO
//@@Usage
//Reads a line (as a string) from the user.  This function has
//two syntaxes.  The first is 
//@[
//  a = getline(prompt)
//@]
//where @|prompt| is a prompt supplied to the user for the query.
//The second syntax omits the @|prompt| argument:
//@[
//  a = getline
//@]
//Note that this function requires command line input, i.e., it 
//will only operate correctly for programs or scripts written
//to run inside the FreeMat GUI environment or from the X11 terminal.
//If you build a stand-alone application and expect it to operate 
//cross-platform, do not use this function (unless you include
//the FreeMat console in the final application).
//@@Signature
//sfunction getline GetLineFunction
//inputs prompt
//outputs string
//!
ArrayVector GetLineFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  QString prompt;
  if (arg.size() < 1)
    prompt = "";
  else {
    if (!arg[0].isString())
      throw Exception("getline requires a string prompt");
    prompt = arg[0].asString();
  }
  return ArrayVector(Array(eval->getLine(prompt)));
}
