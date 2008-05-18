#include "Print.hpp"
#include "IEEEFP.hpp"
#include "FunctionDef.hpp"
#include "Interpreter.hpp"
#include "Struct.hpp"
#include <cmath>

#define MSGBUFLEN 2048

static FMFormatMode formatMode = format_short;

void SetPrintFormatMode(FMFormatMode m) {
  formatMode = m;
}

FMFormatMode GetPrintFormatMode() {
  return formatMode;
}

class ArrayFormatInfo {
public:
  int width;
  bool floatasint;
  int decimals;
  bool expformat;
  double scalefact;
  ArrayFormatInfo(int w = 0, bool f = false, 
		  int d = 0, bool e = false, 
		  double s = 1.0) : 
    width(w), floatasint(f), 
    decimals(d), expformat(e), 
    scalefact(s) {}
};

template <typename T>
static QString TSummarizeArrayCellEntryScalar(const Array &dp) {
  if (dp.allReal())
    return QString("%1").arg(dp.constRealScalar<T>());
  else
    if (dp.constImagScalar<T>() > 0)
      return QString("%1+%2 i").arg(dp.constRealScalar<T>()).arg(dp.constImagScalar<T>());
    else
      return QString("%1%2 i").arg(dp.constRealScalar<T>()).arg(dp.constImagScalar<T>());
}

#define MacroSummarize(ctype,cls) \
  case cls: return TSummarizeArrayCellEntryScalar<ctype>(a);

static QString SummarizeArrayCellEntry(const Array &a) {
  // Special cases
  if (a.isEmpty()) return QString("[]");
  if (a.isString() && a.rows() == 1) return a.asString();
  if (a.isScalar() && !a.isReferenceType()) {
    switch (a.dataClass()) {
      MacroExpandCases(MacroSummarize);
    default:   // fall through
      ;
    }
  }
  QString sparseFlag;
  if (a.isSparse()) sparseFlag = " sparse";
  return QString("[") + a.dimensions().toString() + " " + 
    a.className() + sparseFlag + " array ]";
}

template <class T>
static int GetNominalWidthInteger(const BasicArray<T> &qp) {
  char buffer[MSGBUFLEN];
  int count = int(qp.length());
  const T* array = qp.constData();
  int maxdigit = 0;
  for (int i=0;i<count;i++) {
    memset(buffer,0,MSGBUFLEN);
    sprintf(buffer,"%lld",(int64)array[i]);
    int j = maxdigit;
    while (buffer[j] && j>=maxdigit)
      j++;
    maxdigit = j;
  }
  return maxdigit;  
}
 
template <class T>
static void ComputeScaleFactor(const BasicArray<T> &qp, ArrayFormatInfo& format) {
  int count = int(qp.length());
  const T* array = qp.constData();
  T max_amplitude = 0;
  if (count == 0) return;
  if (format.expformat) return;
  bool finiteElementFound = false;
  for (int i=0;i<count;i++) {
    if (IsFinite(array[i]) && !finiteElementFound) {
      max_amplitude = array[i];
      finiteElementFound = true;
    }
    if ((IsFinite(array[i])) && 
	(fabs((double) array[i]) > fabs((double) max_amplitude)))
      max_amplitude = array[i];
  }
  if (!finiteElementFound) return;
  if (max_amplitude >= 100)
    format.scalefact = pow(double(10.0),double(floor(log10(max_amplitude))));
  else if (max_amplitude <= -100)
    format.scalefact = pow(double(10.0),double(floor(log10(-max_amplitude))));
  else if ((max_amplitude <= .1) && (max_amplitude>0))
    format.scalefact = pow(double(10.0),double(floor(log10(max_amplitude))));
  else if ((max_amplitude >= -.1) && (max_amplitude<0))
    format.scalefact = pow(double(10.0),double(floor(log10(-max_amplitude))));
  else
    format.scalefact = 1.0;
}


template <typename T>
static ArrayFormatInfo GetArrayFormatReal(T x = 0);

template <typename T>
static ArrayFormatInfo GetArrayFormatComplex(T x = 0);

template <>
static ArrayFormatInfo GetArrayFormatReal(float) {
  if (formatMode == format_short) 
    return ArrayFormatInfo(9,false,4);
  else if (formatMode == format_long)
    return ArrayFormatInfo(11,false,7);
  else if (formatMode == format_short_e)
    return ArrayFormatInfo(11,false,4,true);
  else if (formatMode == format_long_e)
    return ArrayFormatInfo(14,false,7,true);
}

template <>
static ArrayFormatInfo GetArrayFormatReal(double) {
  if (formatMode == format_short)
    return ArrayFormatInfo(9,false,4);
  else if (formatMode == format_long)
    return ArrayFormatInfo(18,false,14);
  else if (formatMode == format_short_e)
    return ArrayFormatInfo(11,false,4,true);
  else if (formatMode == format_long_e)
    return ArrayFormatInfo(21,false,14,true);
}

template <>
static ArrayFormatInfo GetArrayFormatComplex(float) {
  if (formatMode == format_short)
    return ArrayFormatInfo(19,false,4);
  else if (formatMode == format_long)
    return ArrayFormatInfo(23,false,7);
  else if (formatMode == format_short_e)
    return ArrayFormatInfo(19,false,4,true);
  else if (formatMode == format_long_e)
    return ArrayFormatInfo(23,false,7,true);
}

template <>
static ArrayFormatInfo GetArrayFormatComplex(double) {
  if (formatMode == format_short)
    return ArrayFormatInfo(19,false,4);
  else if (formatMode == format_long)
    return ArrayFormatInfo(37,false,14);
  else if (formatMode == format_short_e)
    return ArrayFormatInfo(19,false,4,true);
  else if (formatMode == format_long_e)
    return ArrayFormatInfo(37,false,14,true);
}

template <typename T>
static ArrayFormatInfo ComputeArrayFormatInfo(const Array &rp) {
  if (rp.allReal()) {
    if (IsInteger(rp.constReal<T>()))
      return ArrayFormatInfo(GetNominalWidthInteger(rp.constReal<T>()));
    ArrayFormatInfo ret = GetArrayFormatReal(T(0));
    ComputeScaleFactor(rp.constReal<T>(),ret);
    return ret;
  }
  BasicArray<T> combined(rp.fortran<T>());
  if (IsInteger(combined))
    return ArrayFormatInfo(GetNominalWidthInteger(combined));
  ArrayFormatInfo ret = GetArrayFormatComplex(T(0));
  ComputeScaleFactor(combined,ret);
  return ret;
}

#define MacroComputeArrayFormat(ctype,cls)	\
  case cls: return ComputeArrayFormatInfo<ctype>(ref);

ArrayFormatInfo ComputeArrayFormatInfo(const Array &ref) {
  switch (ref.dataClass()) {
  default: throw Exception("unexpected class for ComputeArrayFormatInfo");
    MacroExpandCasesNoBool(MacroComputeArrayFormat);
  case Bool:
    return ArrayFormatInfo(1);
  case StringArray:
    return ArrayFormatInfo(1);
  case CellArray:
    {
      int maxwidth = 1;
      for (index_t i=1;i<ref.length();i++) {
	int len = SummarizeArrayCellEntry(ref.get(i)).size();
	maxwidth = qMax(maxwidth,len);
      }
      return ArrayFormatInfo(maxwidth);
    }
  }
}

#undef MacroComputeArrayFormat

template <class T>
static void emitIntegerReal(Interpreter* io, T val, 
			    const ArrayFormatInfo &format,
			    bool sgned) {
  if (sgned) 
    io->outputMessage("%*lld",format.width,(int64)val);
  else 
    io->outputMessage("%*llu",format.width,(uint64)val);
}

template <class T>
static void emitFloatReal(Interpreter*io, T val, const ArrayFormatInfo &format) {
  if (val != 0) 
    if (format.expformat)
      io->outputMessage("%*.*e",format.width,format.decimals,val);
    else
      io->outputMessage("%*.*f",format.width,format.decimals,val/format.scalefact);
  else
    io->outputMessage("%*d",format.width,0);
}

template <class T>
static void emitIntegerComplex(Interpreter* io, T real, T imag, 
			       const ArrayFormatInfo &format,
			       bool sgned) {
  if (sgned) {
    io->outputMessage("%*lld",format.width,(int64)real);
    if (imag < 0)
      io->outputMessage(" -%*lld",format.width,(int64)-imag);
    else
      io->outputMessage(" +%*lld",format.width,(int64)imag);
  } else {
    io->outputMessage("%*lld",format.width,(int64)real);
    io->outputMessage(" +%*lld",format.width,(int64)imag);  
  }
}

template <class T>
static void emitFloatComplex(Interpreter* io, T real, T imag, 
			     const ArrayFormatInfo &format) {
  int width = format.width/2;
  if ((real != 0) || (imag != 0)) {
    if (format.expformat)
      io->outputMessage("%*.*e",width,format.decimals,real);
    else
      io->outputMessage("%*.*f",width,format.decimals,real/format.scalefact);
    if (imag < 0) {
      if (format.expformat)
	io->outputMessage(" -%*.*ei",width-1,format.decimals,-imag);
      else
	io->outputMessage(" -%*.*fi",width-1,format.decimals,-imag/format.scalefact);
    } else {
      if (format.expformat)
	io->outputMessage(" +%*.*ei",width-1,format.decimals,imag);
      else
	io->outputMessage(" +%*.*fi",width-1,format.decimals,imag/format.scalefact);
    }
  } else 
    io->outputMessage("%*d%*c",width,0,width+2,' ');
}

template <typename T>
static inline void EmitInteger(Interpreter* io, const Array &rp,
			       const ArrayFormatInfo &format, 
			       bool complex, bool sgned) {
  if (complex)
    emitIntegerComplex(io,rp.constRealScalar<T>(),		
		       rp.constImagScalar<T>(),format,sgned);	
  else									
    emitIntegerReal(io,rp.constRealScalar<T>(),format,sgned); 
}

template <typename T>
static inline void EmitFloat(Interpreter* io, const Array &rp,
			     const ArrayFormatInfo &format,
			     bool complex) {
  if (format.floatasint)
    return EmitInteger<T>(io,rp,format,complex,true);
  if (complex)
    emitFloatComplex(io,rp.constRealScalar<T>(),rp.constImagScalar<T>(),format);
  else
    emitFloatReal(io,rp.constRealScalar<T>(),format);
}

#define MacroEmitSignedInt(ctype,cls) \
  case cls: return EmitInteger<ctype>(io,rp,format,complex,true);

#define MacroEmitUnsignedInt(ctype,cls) \
  case cls: return EmitInteger<ctype>(io,rp,format,complex,false);

#define MacroEmitFloat(ctype,cls) \
  case cls: return EmitFloat<ctype>(io,rp,format,complex);

static void Emit(Interpreter* io, const Array &rp, 
		 const ArrayFormatInfo &format, bool complex) {
  switch (rp.dataClass()) {
  default: throw Exception("unexpected class for Emit");
    MacroExpandCasesIntSigned(MacroEmitSignedInt);
    MacroExpandCasesIntUnsigned(MacroEmitUnsignedInt);
    MacroEmitFloat(float,Float);
    MacroEmitFloat(double,Double);
  }
}

#undef MacroEmitFloat
#undef MacroEmitUnsignedInt
#undef MacroEmitSignedInt

//   case FM_STRING:
//     io->outputMessage("%c\0",((const char*) dp)[num]);
//     return;
//   case FM_CELL_ARRAY: {
//     Array *ap;
//     ap = (Array*) dp;
//     if (ap == NULL)
//       io->outputMessage("[]");
//     else
//       io->outputMessage(SummarizeArrayCellEntry(ap[num]));
//     io->outputMessage("  ");
//     break;
//   }

void PrintSheet(Interpreter *io, const ArrayFormatInfo &format,
		index_t offset, const Array &rp, int termWidth, 
		int &printlimit, bool complex) {
  if (printlimit == 0) return;
  // Determine how many columns will fit across
  // the terminal width
  int columns = int(rp.cols());
  int rows = int(rp.rows());
  int colsPerPage = (int) floor((termWidth-1)/((double) format.width + 2));
  colsPerPage = (colsPerPage < 1) ? 1 : colsPerPage;
  int pageCount = (int) ceil(columns/((double)colsPerPage));
  for (int k=0;k<pageCount;k++) {
    int colsInThisPage;
    colsInThisPage = columns - colsPerPage*k;
    colsInThisPage = (colsInThisPage > colsPerPage) ? 
      colsPerPage : colsInThisPage;
    if ((rows*columns > 1) && (pageCount > 1))
      io->outputMessage(" \nColumns %d to %d\n\n",
			k*colsPerPage+1,k*colsPerPage+colsInThisPage);
    else
      io->outputMessage("\n");
    for (int i=0;i<rows;i++) {
      io->outputMessage(" ");
      for (int j=0;j<colsInThisPage;j++) {
	Emit(io,rp.get(i+(k*colsPerPage+j)*rows+offset),format,complex);
	printlimit--;
	if (printlimit <= 0) return;
	io->outputMessage(" ");
      }
      io->outputMessage("\n");
    }
  }
  io->outputMessage("\n");
}
 
template <class T>
static inline void PrintSparseMatrix(const Array& A, Interpreter* io) {
  if (A.allReal())
    PrintMe(A.constRealSparse<T>(),io);
  else
    PrintMe(A.constRealSparse<T>(),A.constImagSparse<T>(),io);
}

static inline void PrintStructArray(const Array& A, Interpreter* io) {
  const StructArray &rp(A.constStructPtr());
  if (A.isScalar()) {
    for (int i=0;i<rp.fieldCount();i++) {
      io->outputMessage("    ");
      io->outputMessage(rp.fieldName(i));
      io->outputMessage(": ");
      io->outputMessage(SummarizeArrayCellEntry(rp[i].get(1)));
      io->outputMessage("\n");
    }
  } else {
    io->outputMessage(" Fields\n");
    for (int i=0;i<rp.fieldCount();i++) {
      io->outputMessage("    ");
      io->outputMessage(rp.fieldName(i));
      io->outputMessage("\n");
    }
  }
}

#define MacroSparse(ctype,cls) \
  case cls: return PrintSparseMatrix<ctype>(A,io);

void PrintArrayClassic(const Array &A, int printlimit, Interpreter* io) {
  if (printlimit == 0) return;
  int termWidth = io->getTerminalWidth();
  NTuple Adims(A.dimensions());
  if (A.isUserClass())  return;
  if (A.isEmpty()) {
    if (A.dimensions() == NTuple(0,0))
      io->outputMessage("  []\n");
    else {
      io->outputMessage("  Empty array ");
      io->outputMessage(A.dimensions().toString());
      io->outputMessage("\n");
    }
    return;
  }
  if (A.isSparse()) {
    switch (A.dataClass()) {
    default:
      throw Exception("Unknown sparse array");
      MacroExpandCases(MacroSparse);
    }
    return;
  }
  if (A.dataClass() == Struct)
    PrintStructArray(A,io);
  ArrayFormatInfo format(ComputeArrayFormatInfo(A));
  if (!A.isScalar() && (format.scalefact != 1))
    io->outputMessage("\n   %.1e * \n",format.scalefact);
  if (A.isScalar() && (format.scalefact != 1) && !format.floatasint)
    format.expformat = true;
  bool complexFlag = !A.allReal();
  if (A.is2D()) 
    PrintSheet(io,format,index_t(1),A,termWidth,printlimit,complexFlag);
  else {
    /**
     * For N-ary arrays, data slice  -  start with 
     * [1,1,1,...,1].  We keep doing the matrix
     * print , incrementing from the highest dimension,
     * and rolling downwards.
     */
    io->outputMessage("\n");
    index_t offset = 1;
    NTuple wdims(1,1);
    index_t rows = A.rows();
    index_t columns = A.cols();
    while (wdims <= A.dimensions()) {
      io->outputMessage("(:,:");
      for (int m=2;m<=A.dimensions().lastNotOne();m++) 
	io->outputMessage(",%d",wdims[m]);
      io->outputMessage(") = \n");
      PrintSheet(io,format,offset,A,termWidth,printlimit,complexFlag);
      offset += rows*columns;
      wdims[1]++; 
      A.dimensions().ripple(wdims);
    }
  }
  if (printlimit == 0)
    io->outputMessage("\nPrint limit has been reached.  Use setprintlimit function to enable longer printouts\n");
}

#undef MacroSparse

QString ArrayToPrintableString(const Array& a) {
  if (a.isEmpty()) return QString("[]");
  if (a.isSparse()) return QString("");
  if (a.isString()) return a.asString();
  if (a.isReferenceType()) return QString("");
  if (!a.isScalar() && !a.isString()) return QString("");
  switch (a.dataClass()) {
  default:
    return QString("?");
    MacroExpandCases(MacroSummarize);
  }
}

#undef MacroSummarize
  
