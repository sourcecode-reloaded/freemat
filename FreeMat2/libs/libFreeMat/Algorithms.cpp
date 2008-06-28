#include "Algorithms.hpp"
#include "Struct.hpp"
#include "IEEEFP.hpp"
#include "SparseCCS.hpp"

void WarningMessage(QString msg) {
#warning FIXME
  std::cout << "Warning:" << qPrintable(msg);
}

const Array StringArrayFromStringVector(const StringVector& arg) {
  int maxlen = 0;
  for (int i=0;i<arg.size();i++)
    maxlen = qMax(maxlen,arg[i].size());
  Array ret(StringArray,NTuple(arg.size(),maxlen));
  BasicArray<uint16> &sp(ret.real<uint16>());
  for (index_t i=1;i!=arg.size();i++) 
    for (index_t j=1;j!=arg[int(i-1)].size();j++)
      sp.set(NTuple(i,j),arg[int(i-1)][int(j-1)].unicode());
  return ret;
}

Array Vectorize(const Array& arg) {
  Array A(arg);
  A.reshape(NTuple(A.length(),1));
  return A;
}

bool IsColonOp(const Array &x) {
  return (x.dataClass() == StringArray) && (x.asString() == ":");
}

bool IsCellStringArray(const Array &arg) {
  if (arg.dataClass() != CellArray) return false;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i!=arg.length();i++)
    if (!rp[i].isString()) return false;
  return true;
}

template <typename T>
static inline bool Tis_negative(const Array &x) {
  if (x.isSparse())
    return IsNonNegative(x.constRealSparse<T>());
  else if (x.isScalar())
    return IsNonNegative(x.constRealScalar<T>());
  else
    return IsNonNegative(x.constReal<T>());
}

#define MacroIsNonNegative(ctype, cls) \
  case cls: return Tis_negative<ctype>(x);

bool IsNonNegative(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.dataClass()) {
  default:
    return false;
  case Bool:
  case UInt8:
  case UInt16:
  case StringArray:
  case UInt32:
  case UInt64:
    return true;
    MacroExpandCasesSigned(MacroIsNonNegative);
  }
}
#undef MacroIsNonNegative


bool IsUnsigned(const Array &x) {
  switch (x.dataClass()) {
  default:
    return false;
  case Bool:
  case UInt8:
  case StringArray:
  case UInt16:
  case UInt32:
  case UInt64:
    return true;
  }
}

template <typename T>
static inline bool Tis_integer(const Array &x) {
  if (x.isSparse()) {
    return IsInteger(x.constRealSparse<T>());
  } else if (x.isScalar()) {
    return IsInteger(x.constRealScalar<T>());
  } else {
    return IsInteger(x.constReal<T>());
  }
}

bool IsInteger(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.dataClass()) {
  default:
    return false;
  case Int8:
  case UInt8:
  case Int16:
  case UInt16:
  case Int32:
  case UInt32:
  case Int64:
  case UInt64:
    return true;
  case Float: return Tis_integer<float>(x);
  case Double: return Tis_integer<double>(x);
  }
}

const IndexArray IndexArrayFromArray(const Array &index) {
  if ((index.dataClass() == Bool) && (index.type().Scalar == 1))
    return Find(index.asDenseArray().constReal<bool>());
  if (index.type().Sparse == 1)
    throw Exception("Sparse indexing not supported currently");
  if (!index.allReal())
    WarningMessage("Complex part of index ignored");
  if (index.dataClass() == Double)
    return index.constReal<index_t>();
  if (index.dataClass() == Bool)
    return Find(index.constReal<logical>());
  Array index_converted(index.toClass(Double));
  if (!index_converted.allReal())
    WarningMessage("Complex part of index ignored");
  return index_converted.constReal<index_t>();
  if (IsColonOp(index))
    return IndexArray(-1);
  throw Exception("Unsupported index type");
}

const ArrayVector ArrayVectorFromCellArray(const Array &arg) {
  if (arg.dataClass() != CellArray) 
    throw Exception("Unsupported type for call to toArrayVector");
  ArrayVector ret;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i<=arg.length();i++)
    ret.push_back(rp.get(i));
  return ret;
}

const Array CellArrayFromArray(const Array & arg) {
  Array ret(CellArray,NTuple(1,1));
  ret.real<Array>().set(1,arg);
  return ret;
}

const Array CellArrayFromArrayVector(ArrayVector &arg, index_t cnt) {
  Array ret(CellArray,NTuple(1,arg.size()));
  BasicArray<Array> &rp(ret.real<Array>());
  for (index_t i=1;i!=cnt;i++) {
    rp.set(i,arg.front());
    arg.pop_front();
  }
  return ret;
}

Array CellConstructor(const ArrayVector &arg) {
  ArrayVector copy(arg);
  return CellArrayFromArrayVector(copy,copy.size());
}

const Array CellArrayFromStringVector(const StringVector& arg) {
  Array ret(CellArray,NTuple(1,arg.size()));
  BasicArray<Array> &rp(ret.real<Array>());
  for (index_t i=1;i!=arg.size();i++) 
    rp.set(i,arg[int(i-1)]);
  return rp;
}

StringVector StringVectorFromArray(const Array &arg) {
  if (arg.isString())
    return StringVector(arg.asString());
  if (!IsCellStringArray(arg)) throw Exception("Cannot convert array to a set of a strings");
  StringVector ret;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i!=rp.length();i++)
    ret.push_back(rp[i].asString());
  return ret;
}

void SetCellContents(Array &cell, const Array& index, 
		     ArrayVector& data) {
  if (cell.dataClass() != CellArray)
    throw Exception("A{B} = C only supported for cell arrays.");
  if (IsColonOp(index)) {
    if (cell.length() > data.size())
      throw Exception("Not enough right hand side values to satisfy left hand side expression.");
    cell.set(index,CellArrayFromArrayVector(data,data.size()));
    data.clear();
    return;
  }
  IndexArray ndx(IndexArrayFromArray(index));
  if (ndx.length() > data.size())
    throw Exception("Not enought right hand side values to satisfy left hand side expression.");
  cell.set(ndx,CellArrayFromArrayVector(data,ndx.length()));
}

void SetCellContents(Array &cell, const ArrayVector& index, 
		     ArrayVector& data) {
  if (cell.dataClass() != CellArray)
    throw Exception("A{B1,B2,...BN} = B only supported for cell arrays.");
  IndexArrayVector addr;
  NTuple dims;
  for (int i=0;i<index.size();i++) {
    addr.push_back(IndexArrayFromArray(index[i]));
    if (IsColonOp(addr[i]))
      dims[i] = cell.dimensions()[i];
    else
      dims[i] = addr[i].length();
  }
  if (data.size() < dims.count())
    throw Exception("Not enough right hand side values to satisfy left hand side expression");
  cell.set(addr,CellArrayFromArrayVector(data,dims.count()));
}

QStringList FieldNames(const Array& arg) {
  if (arg.dataClass() != Struct)
    throw Exception("fieldnames only valid for structure arrays");
  const StructArray &rp(arg.constStructPtr());
  return rp.fieldNames();
}

static inline void do_single_sided_algo_double(double a, double b,double *pvec, 
					       int adder, int count) {
  double d = a;
  for (int i=0;i<count;i++) {
    pvec[i*adder] = (double) d;
    d += b;
  }
}
  
static inline void do_double_sided_algo_double(double a, double b, double c, double *pvec, 
					       int adder, int count) {
  if (count%2) {
    do_single_sided_algo_double(a,b,pvec,adder,count/2);
    do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2+1);
  } else {
    do_single_sided_algo_double(a,b,pvec,adder,count/2);
    do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2);
  }
}

Array RangeConstructor(double minval, double stepsize, double maxval, bool vert) {
  NTuple dim;
  if (stepsize == 0) 
    throw Exception("step size must be nonzero in colon expression");
  //ideally, n = (c-a)/b
  // But this really defines an interval... we let
  // n_min = min(c-a)/max(b)
  // n_max = max(c-a)/min(b)
  // where min(x) = {y \in fp | |y| is max, |y| < |x|, sign(y) = sign(x)}
  //       max(x) = {y \in fp | |y| is min, |y| > |x|, sign(y) = sign(x)}
  double ntest_min = nextafter(maxval-minval,0)/nextafter(stepsize,stepsize+stepsize);
  double ntest_max = nextafter(maxval-minval,maxval-minval+stepsize)/nextafter(stepsize,0);
  index_t npts = (index_t) floor(ntest_max);
  bool use_double_sided = (ntest_min <= npts) && (npts <= ntest_max);
  npts++;
  if (npts < 0) npts = 0;
  if (vert) {
    dim = NTuple(npts,1);
  } else {
    dim = NTuple(1,npts);
  }
  BasicArray<double> rp(dim);
  if (use_double_sided)
    do_double_sided_algo_double(minval,stepsize,maxval,rp.data(),int(1),int(npts));
  else
    do_single_sided_algo_double(minval,stepsize,rp.data(),int(1),int(npts));
  return Array(rp);
}

template <typename T>
static inline Array T_Transpose(const Array &x) {
  if (x.isScalar())
    return x;
  if (x.isSparse()) {
    if (x.allReal())
      return Array(Transpose(x.constRealSparse<T>()));
    else
      return Array(Transpose(x.constRealSparse<T>()),
		   Transpose(x.constImagSparse<T>()));
  } else {
    if (x.allReal())
      return Array(Transpose(x.constReal<T>()));
    else
      return Array(Transpose(x.constReal<T>()),
		   Transpose(x.constImag<T>()));
  }
}

#define MacroTranspose(ctype,cls) \
  case cls: return T_Transpose<ctype>(A);

Array Transpose(const Array &A) {
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by transpose operator");
    MacroExpandCasesAll(MacroTranspose);
  }
}

#undef MacroTranspose

template <typename T>
static inline Array T_Hermitian(const Array &x) {
  if (x.isScalar())
    if (x.allReal())
      return x;
    else 
      return Array(x.constRealScalar<T>(),(T)-x.constImagScalar<T>());
  if (x.isSparse()) {
    if (x.allReal())
      return Array(Transpose(x.constRealSparse<T>()));
    else
      return Array(Transpose(x.constRealSparse<T>()),
		   Negate(Transpose(x.constImagSparse<T>())));
  } else {
    if (x.allReal())
      return Array(Transpose(x.constReal<T>()));
    else
      return Array(Transpose(x.constReal<T>()),
		   Negate(Transpose(x.constImag<T>())));
  }
}

#define MacroHermitian(ctype,cls) \
  case cls: return T_Hermitian<ctype>(A);

Array Hermitian(const Array &A) {
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by transpose operator");
    MacroExpandCasesSigned(MacroHermitian);
  }
}

#undef MacroHermitian

BasicArray<Array> ArrayVectorToBasicArray(const ArrayVector& a) {
  BasicArray<Array> retvec(NTuple(a.size(),1));
  for (int i=0;i<a.size();i++) 
    retvec.set(index_t(i+1),a.at(i));
  return retvec;
}

NTuple ConvertArrayToNTuple(const Array &A) {
  if (A.length() > NDims)
    throw Exception(QString("MAT Variable has more dimensions ") + 
		    QString("than maxDims (currently set to ") + 
		    NDims + ")."); // FIXME - more graceful ways to do this
  Array B(A.asDenseArray().toClass(Index));
  const BasicArray<index_t> &rp(B.constReal<index_t>());
  NTuple dm;
  for (index_t i=1;i!=B.length();i++)
    dm[int(i-1)] = rp[i];
  return dm;
}

Array ConvertNTupleToArray(const NTuple &D) {
  index_t Dsize(D.lastNotOne());
  BasicArray<index_t> rp(NTuple(1,Dsize));
  for (index_t i=1;i!=Dsize;i++)
    rp.set(i,D[int(i-1)]);
  return Array(rp);
}

template <typename T>
static inline Array Treal_part(const Array *ptr) {
  if (ptr->isScalar())
    return Array(ptr->constRealScalar<T>());
  else if (ptr->isSparse())
    return Array(ptr->constRealSparse<T>());
  else
    return Array(ptr->constReal<T>());
}

#define MacroReal(ctype,cls)			\
  case cls: return Treal_part<ctype>(&A);

Array Real(const Array &A) {
  if (A.isReferenceType()) return A;
  switch (A.dataClass()) {
  default: throw Exception("unhandled type for real");
    MacroExpandCasesSimple(MacroReal);
  }
}

#undef MacroReal

template <typename T>
static inline Array Timag_part(const Array *ptr) {
  if (ptr->isScalar())
    return Array(ptr->constImagScalar<T>());
  else if (ptr->isSparse())
    return Array(ptr->constImagSparse<T>());
  else
    return Array(ptr->constImag<T>());
}

#define MacroImag(ctype,cls)			\
  case cls: return Timag_part<ctype>(&A);

Array Imag(const Array &A) {
  if (A.isReferenceType()) 
    throw Exception("unhandled type for imag");
  switch (A.dataClass()) {
  default: throw Exception("unhandled type for imag");
    MacroExpandCasesSimple(MacroImag);
  }
}

#undef MacroImag

template <typename T>
static inline Array Tmerge(const Array *p_real, const Array *p_imag) {
  if (p_real->isScalar())
    return Array(p_real->constRealScalar<T>(),p_imag->constRealScalar<T>());
  else if (p_real->isSparse())
    return Array(p_real->constRealSparse<T>(),p_imag->constRealSparse<T>());
  else
    return Array(p_real->constReal<T>(),p_imag->constReal<T>());
}

Array MergeToComplex(const Array &Real, const Array &Imag) {
  if (Real.dataClass() != Imag.dataClass()) 
    throw Exception("cannot merge unlike types into a complex type");
  if (Real.isSparse() ^ Imag.isSparse())
    throw Exception("cannot merge sparse and not-sparse arrays into a complex type");
  if (Real.isReferenceType() || Imag.isReferenceType())
    throw Exception("reference types cannot be combined into a complex type");
  if (Real.dimensions() != Imag.dimensions())
    throw Exception("cannot merge different sized arrays into a complex type");
  
}
#undef MacroMerge

template <typename T>
static inline bool Tanynotfinite(const Array *ptr) {
  if (ptr->isScalar())
    if (ptr->allReal())
      return (!IsFinite(ptr->constRealScalar<T>()));
    else
      return (!IsFinite(ptr->constRealScalar<T>()) || !IsFinite(ptr->constImagScalar<T>()));
  else if (ptr->isSparse())
    if (ptr->allReal())
      return AnyNotFinite(ptr->constRealSparse<T>());
    else
      return (AnyNotFinite(ptr->constRealSparse<T>()) || AnyNotFinite(ptr->constImagSparse<T>()));
  else
    if (ptr->allReal())
      return AnyNotFinite(ptr->constReal<T>());
    else
      return (AnyNotFinite(ptr->constReal<T>()) || AnyNotFinite(ptr->constImag<T>()));
}

#define MacroAnyNotFinite(ctype,cls)		\
  case cls: return Tanynotfinite<ctype>(&A);

bool AnyNotFinite(const Array &A) {
  if (A.isReferenceType()) return false;
  switch (A.dataClass()) {
  default: return false;
    MacroExpandCasesSimple(MacroAnyNotFinite);
  }
}

#undef MacroAnyNotFinite

template <typename T>
static inline bool Tispositive(const Array *ptr) {
  if (ptr->isScalar() && ptr->allReal())
    return (ptr->constRealScalar<T>() >= 0);
  else if (ptr->isSparse())
    return IsPositive(ptr->constRealSparse<T>());
  else
    return IsPositive(ptr->constReal<T>());
}

#define MacroIsPositive(ctype,cls)		\
  case cls: return Tispositive<ctype>(&A);

bool IsPositive(const Array &A) {
  switch (A.dataClass()) {
  default: return true;
    MacroExpandCasesSigned(MacroIsPositive);
  }
}

#undef MacroIsPositive

template <typename T>
static inline bool Tissymmetric(const Array *ptr) {
  if (ptr->isScalar()) 
    return true;
  else if (ptr->isSparse())
    if (ptr->allReal())
      return IsSymmetric(ptr->constRealSparse<T>());
    else
      return IsSymmetric(ptr->constRealSparse<T>(),ptr->constImagSparse<T>());
  else 
    if (ptr->allReal())
      return IsSymmetric(ptr->constReal<T>());
    else
      return IsSymmetric(ptr->constReal<T>(),ptr->constImag<T>());
}

#define MacroIsSymmetric(ctype,cls)		\
  case cls: return Tissymmetric<ctype>(&A);

bool IsSymmetric(const Array &A) {
  switch (A.dataClass()) {
  default: return true;
    MacroExpandCasesSimple(MacroIsSymmetric);
  }
}

#undef MacroIsSymmetric

template <typename T>
static inline Array Tgetdiagonal(const Array *ptr, int diagonal) {
  if (ptr->isScalar())
    if (diagonal == 0) 
      return *ptr;
    else
      return Array(ptr->dataClass());
  else if (ptr->isSparse())
    if (ptr->allReal())
      return Array(GetDiagonal(ptr->constRealSparse<T>(),diagonal));
    else
      return Array(GetDiagonal(ptr->constRealSparse<T>(),diagonal),
		   GetDiagonal(ptr->constImagSparse<T>(),diagonal));
  else
    if (ptr->allReal())
      return Array(GetDiagonal(ptr->constReal<T>(),diagonal));
    else
      return Array(GetDiagonal(ptr->constReal<T>(),diagonal),
		   GetDiagonal(ptr->constImag<T>(),diagonal));
}

#define MacroGetDiagonal(ctype,cls)			\
  case cls: return Tgetdiagonal<ctype>(&a,diagonal);

Array GetDiagonal(const Array &a, int diagonal) {
  switch (a.dataClass()) {
  default: throw Exception("unsupported type for get diagonal");
    MacroExpandCasesSimple(MacroGetDiagonal);
  }
}

#undef MacroGetDiagonal

template <typename T>
static inline Array Tdiagonalarray(const Array *ptr, int diagonal) {
  if (ptr->isSparse())
    if (ptr->allReal())
      return Array(DiagonalArray(ptr->constRealSparse<T>(),diagonal));
    else
      return Array(DiagonalArray(ptr->constRealSparse<T>(),diagonal),
		   DiagonalArray(ptr->constImagSparse<T>(),diagonal));
  else
    if (ptr->allReal())
      return Array(DiagonalArray(ptr->constReal<T>(),diagonal));
    else
      return Array(DiagonalArray(ptr->constReal<T>(),diagonal),
		   DiagonalArray(ptr->constImag<T>(),diagonal));
}

#define MacroDiagonalArray(ctype,cls)			\
  case cls: return Tdiagonalarray<ctype>(&A,diagonal);

Array DiagonalArray(const Array &A, int diagonal) {
  switch (A.dataClass()) {
  default: throw Exception("unsupported type for diagonal array constructor");
    MacroExpandCasesSimple(MacroDiagonalArray);
  }
}

#undef MacroDiagonalArray


// Do a horizontal concatenation of a set of arrays
// Horizontal concatenation means that all dimensions
// except for the number of columns must match.  The
// type logic is the opposite of FreeMat 3.6!  The smallest
// type dominates the type of the output (space-preserving
// as opposed to precision-preserving).  This change is to 
// support the M-lab model.

// This is the generic version that works along any dimension.
// It only works with dense arrays, and not with structure arrays
Array NCat(const ArrayVector& data, int catdim) {
  // Compute the output dataclass
  if (data.size() == 0) return EmptyConstructor();
  DataClass cls(data[0].dataClass());
  for (int i=1;i<data.size();i++)
    cls = qMin(cls,data[i].dataClass());
  NTuple outdims(data[0].dimensions());
  // Compute the output dimensions and validate each of the elements
  for (int i=1;i<data.size();i++) {
    NTuple eldims(data[i].dimensions());
    for (int j=0;j<NDims;j++)
      if ((eldims[j] != outdims[j]) && (j != catdim))
	throw Exception(QString("Mismatch in dimension %1 for elements being concatenated along dimension %2").arg(j+1).arg(catdim+1));
    outdims[catdim] += eldims[catdim];
  }
  Array retval(cls,outdims);
  ArrayIterator iter(&retval,catdim);
  while (iter.isValid()) {
    int selector = 0;
    index_t adjusted = 1;
    for (index_t i=1;i!=iter.size();i++) {
      while (adjusted > data[selector].dimensions()[catdim]) {
	adjusted -= data[selector].dimensions()[catdim];
	selector++;
      }
      iter.set(data[selector].get(iter.pos().replace(catdim,adjusted)).toClass(cls));
      adjusted++;
      iter.next();
    }
    iter.nextSlice();
  }
  return retval;
}

Array HCat(const ArrayVector& data) {
  return NCat(data,1);
}

Array VCat(const ArrayVector& data) {
  return NCat(data,0);
}

Array MatrixConstructor(const ArrayMatrix& data) {
  ArrayVector hcatresults;
  for (int i=0;i<data.size();i++)
    hcatresults.push_back(HCat(data[i]));
  return VCat(hcatresults);
}

Array CellConstructor(const ArrayMatrix& data) {
  ArrayVector hcatresults;
  for (int i=0;i<data.size();i++) 
    hcatresults.push_back(CellConstructor(data[i]));
  return VCat(hcatresults);
}

Array StructConstructor(const StringVector& fnames, const ArrayVector& values) {
  if (fnames.size() != values.size())
    throw Exception("Number of field names must match number of values in structure constructor.");
  /**
   * First, we have to make sure that each entry of "values" have 
   *  1.  cell arrays of the same size,
   *  2.  single element cell arrays,
   *  3.  single values.
   */
  NTuple dims;
  bool nonSingularFound = false;
  for (int i=0;i<values.size();i++) {
    /**
     * Check the type of the entry.  If its a non-cell array, then
     * then ignore this entry.
     */
    if (values[i].dataClass() == CellArray) {
      /**
       * This is a cell-array, so look for non-scalar cell-arrays.
       */
      if (!values[i].isScalar()) {
	if (!nonSingularFound) {
	  nonSingularFound = true;
	  dims = values[i].dimensions();
	} else
	  if (dims != values[i].dimensions())
	    throw Exception("Array dimensions of non-scalar entries must agree in structure construction.");
      }
    }
  }
  /**
   * At this point we can construct the dimensions of the output.
   */
  if (!nonSingularFound) {
    dims = NTuple(1,1);
  }
  /**
   * The dimensions of the object have been identified.  Set the
   * dimensions of the object and the field names.  Then allocate
   * the space.
   */
  StructArray qp;
  for (int i=0;i<fnames.size();i++) {
    BasicArray<Array> rp(dims);
    Array rval = values[i];
    if (!rval.isEmpty() && rval.isSparse())
      throw Exception("Sparse arrays not supported for struct constructor.");
    if (rval.isEmpty())
      rp.fill(rval);
    else {
      if (rval.dataClass() == CellArray) {
	if (rval.isScalar())
	  rp.fill(rval.get(1));
	else
	  rp = rval.constReal<Array>();
      } else
	rp.fill(rval);
    }
    qp.insert(fnames[i],rp);
  }
  return Array(qp);
}

template <typename T>
static bool RealAllZeros(const Array &t) {
  if (t.isScalar()) 
    return (t.constRealScalar<T>() == 0);
  else if (t.isSparse())
    return AllZeros(t.constRealSparse<T>());
  else
    return AllZeros(t.constReal<T>());
}

#define MacroRealZeros(ctype,cls)		\
  case cls: return RealAllZeros<ctype>(t);

bool RealAllZeros(const Array &t) {
  switch (t.dataClass()) {
  default: throw Exception("Unhandled case -- argument must be numeric");
    MacroExpandCasesSimple(MacroRealZeros);
  }
}

template <typename T>
static Array TPermute(const Array &x, const NTuple &dp) {
  if (x.allReal()) 
    return Array(Permute(x.constReal<T>(),dp));
  else 
    return Array(Permute(x.constReal<T>(),dp),Permute(x.constImag<T>(),dp));
}

static Array StructPermute(const Array &x, const NTuple &dp) {
  const StructArray &sp(x.constStructPtr());
  StructArray ret;
  for (int i=0;i<sp.fieldCount();i++)
    ret.insert(sp.fieldName(i),Permute(sp[i],dp));
  return Array(ret);
}

#define MacroPermute(ctype,cls)			\
  case cls: return TPermute<ctype>(A,dp);

Array Permute(const Array &A, const NTuple &dp) {
  // Check for an identity permutation
  NTuple outdims(A.dimensions().permute(dp));
  bool id_perm = true;
  for (int i=0;i<NDims;i++)
    if (dp[i] != (i+1)) id_perm = false;
  if (id_perm) return A;
  if (A.is2D()) return Transpose(A);
  if (A.isSparse()) throw Exception("illegal permutation for sparse arrays");
  if (A.isScalar()) return A;
  if (A.isEmpty()) return Array(A.dataClass(),outdims);
  switch (A.dataClass()) {
  default: throw Exception("Unsupported dataclass for permute");
  case Struct:
    return StructPermute(A,dp);
    MacroExpandCasesAll(MacroPermute);
  }
}

// s is the switch value to test, x is the values to test against
bool TestForCaseMatch(const Array &s, const Array &x) {
  if (!(s.isScalar() || s.isString()))
    throw Exception("Switch argument must be a scalar or a string");
  // And we had better not be a reference type
  if (s.isReferenceType())
    throw Exception("Switch argument cannot be a reference type (struct or cell array)");
  // If x is a scalar, we just need to call the scalar version
  if (((x.dataClass() != CellArray) && x.isScalar()) || x.isString())
    return (s == x);
  const BasicArray<Array> &xp(x.constReal<Array>());
  for (index_t i=1;i!=xp.length();i++) 
    if (s == xp[i]) return true;
  return false;
}

static BasicArray<index_t> DecompressCCSCols(const BasicArray<index_t> &colstart, index_t len) {
  BasicArray<index_t> retval(NTuple(1,len));
  index_t p=1;
  for (index_t i=2;i!=colstart.length();i++)
    for (index_t j=1;j!=(colstart[i]-colstart[i-1]);j++) {
      retval[p] = index_t(i-2);
      p = p + 1;
    }
  return retval;
}

template <typename T>
static SparseMatrix<T> CCSToSparse(const BasicArray<index_t> &rowstart,
				   const BasicArray<index_t> &colstart,
				   const BasicArray<T> &Adata) {
  BasicArray<index_t> cols(DecompressCCSCols(colstart,Adata.length()));
  SparseMatrix<T> retvec;
  for (index_t i=1;i!=cols.length();i++) 
    retvec[NTuple(rowstart[i]+1,cols[i]+1)] = Adata[i];
  return retvec;
}

template <typename T>
static Array IJVToSparse(const BasicArray<index_t> &irp,
			 const BasicArray<index_t> &jcp,
			 const Array &pr, const Array &pi, 
			 bool complexFlag) {
  if (!complexFlag)
    return Array(CCSToSparse(irp,jcp,pr.asDenseArray().constReal<T>()));
  else
    return Array(CCSToSparse(irp,jcp,pr.asDenseArray().constReal<T>()),
		 CCSToSparse(irp,jcp,pi.asDenseArray().constImag<T>()));
}

#define MacroIJVToSparse(ctype,cls)					\
  case cls: return IJVToSparse<ctype>(irp,jcp,pr,pi,complexFlag);

Array MatIJVToSparse(const Array &ir, const Array &jc,
		     const Array &pr, const Array &pi, bool complexFlag) {
  const BasicArray<index_t> &irp(ir.constReal<index_t>());
  const BasicArray<index_t> &jcp(jc.constReal<index_t>());
  switch(pr.dataClass()) {
  default: throw Exception("Unsupported data type " + pr.className() + " in sparse constructor");
    MacroExpandCasesSimple(MacroIJVToSparse);
  }
}

template <typename T>
void SparseToIJVReal(const SparseMatrix<T> &x, Array &rows, Array &cols, Array &vals) {
  QVector<uint32> rowstart;
  QVector<uint32> colstart;
  QVector<T> xdata;
  SparseToCCS(x,rowstart,colstart,xdata);
  rows = Array(ToBasicArray(rowstart)).toClass(Index);
  cols = Array(ToBasicArray(colstart)).toClass(Index);
  vals = Array(ToBasicArray(xdata));
}

template <typename T>
void SparseToIJVComplex(const SparseMatrix<T> &xr, const SparseMatrix<T> &xi,
			Array &rows, Array &cols, Array &vals) {
  QVector<uint32> rowstart;
  QVector<uint32> colstart;
  QVector<T> xdata_real;
  QVector<T> xdata_imag;
  SparseToCCS(xr,rowstart,colstart,xdata_real);
  SparseToCCS(xi,rowstart,colstart,xdata_imag);
  rows = Array(ToBasicArray(rowstart)).toClass(Index);
  cols = Array(ToBasicArray(colstart)).toClass(Index);
  vals = Array(ToBasicArray(xdata_real),ToBasicArray(xdata_imag));
}

template <typename T>
void SparseToIJV(const Array &x, Array &rows, Array &cols, Array &vals) {
  if (x.allReal())
    SparseToIJVReal(x.constRealSparse<T>(),rows,cols,vals);
  else
    SparseToIJVComplex(x.constRealSparse<T>(),x.constImagSparse<T>(),rows,cols,vals);
}

#define MacroSparseToIJV(ctype,cls)				\
  case cls: return SparseToIJV<ctype>(x,rows,cols,vals);

void SparseToIJVMat(const Array &x, Array &rows, Array &cols, Array &vals) {
  if (!x.isSparse()) throw Exception("x must be sparse");
  switch (x.dataClass()) {
  default: throw Exception("unhandled case");
    MacroExpandCasesSimple(MacroSparseToIJV);
  }
}

#undef MacroSparseToIJV

template <typename T>
Array ToSparse(const Array &data) {
  if (data.allReal())
    return Array(ToRealSparse<T>(data));
  else
    return Array(ToRealSparse<T>(data),ToImagSparse<T>(data));
}

#define MacroToSparse(ctype,cls)		\
  case cls: return ToSparse<ctype>(data);

Array ToSparse(const Array& data) {
  switch (data.dataClass()) {
  default: throw Exception("unhandled case for sparse");
    MacroExpandCases(MacroToSparse);
  }
}

#undef MacroToSparse
