#ifndef __Array_hpp__
#define __Array_hpp__

#include <QSharedData>
#include "Types.hpp"
#include "BasicArray.hpp"
#include "IndexArray.hpp"
#include "FastList.hpp"
#include "SparseMatrix.hpp"
#include "Exception.hpp"
class StringArray;
class StructArray;
class Array;

typedef FastList<Array> ArrayVector;
typedef QVector<ArrayVector> ArrayMatrix;

// Assume this has to fit in 32 bits
//   scalar flag  - 1 bit
//   sparse flag  - 1 bit
//   complex flag - 1 bit
//   class flag   - 5 bits
//   user class   - 24 bits

typedef unsigned DataClass;

const DataClass Invalid = 0;
const DataClass CellArray = 1;
const DataClass Struct = 2;
const DataClass StringArray = 3;
const DataClass Bool = 4;
const DataClass Int8 = 5;
const DataClass UInt8 = 6;
const DataClass Int16 = 7;
const DataClass UInt16 = 8;
const DataClass Int32 = 9;
const DataClass UInt32 = 10;
const DataClass Int64 = 11;
const DataClass UInt64 = 12;
const DataClass Float = 13;
const DataClass Double = 14;

template <typename T>
static inline DataClass GetDataClass(T = 0);

typedef struct {
  DataClass Class : 5;
  unsigned Scalar : 1;
  unsigned Complex : 1;
  unsigned Sparse : 1;
} Type;

class SharedObject : public QSharedData {
  Type m_type;
  void *m_p;
public:
  SharedObject(Type t, void* p);
  SharedObject(const SharedObject& copy);
  SharedObject& operator=(const SharedObject &copy);
  ~SharedObject();
  inline void* ptr() const {return m_p;}
  inline Type type() const {return m_type;}
};

typedef struct {
  QSharedDataPointer<SharedObject> p;
  union {
    bool Bool;
    int8 Int8;
    uint8 UInt8;
    int16 Int16;
    uint16 UInt16;
    int32 Int32;
    uint32 UInt32;
    int64 Int64;
    uint64 UInt64;
    float Float;
    double Double;
  };
} Data;

class Array {
public:
  inline Array() {m_type.Class = Invalid;}
  // Defined in ArrayPrivate
  template <typename T> inline explicit Array(T real); 
  template <typename T> inline explicit Array(T real, T imag); 
  template <typename T> 
  inline Array(const BasicArray<T> &r) {
    m_type.Class = GetDataClass<T>();
    m_type.Complex = 0;
    m_type.Sparse = 0;
    m_type.Scalar = 0;
    m_real.p = new SharedObject(m_type, new BasicArray<T>(r));
  }
  template <typename T> 
  inline Array(const BasicArray<T> &r, const BasicArray<T> &i) {
    m_type.Class = GetDataClass<T>();
    m_type.Complex = 1;
    m_type.Sparse = 0;
    m_type.Scalar = 0;
    m_real.p = new SharedObject(m_type, new BasicArray<T>(r));
    m_imag.p = new SharedObject(m_type, new BasicArray<T>(i));
  }
  template <typename T>
  inline Array(BasicArray<T> *r) {
    m_type.Class = GetDataClass<T>();
    m_type.Complex = 0;
    m_type.Sparse = 0;
    m_type.Scalar = 0;
    m_real.p = new SharedObject(m_type,r);
  }
  template <typename T>
  inline Array(BasicArray<T> *r, BasicArray<T> *i) {
    m_type.Class = GetDataClass<T>();
    m_type.Complex = 1;
    m_type.Sparse = 0;
    m_type.Scalar = 0;
    m_real.p = new SharedObject(m_type,r);
    m_imag.p = new SharedObject(m_type,i);
  }
  Array(DataClass t, const NTuple &dims = NTuple(0,0));
  Array(const QString &text);
  template <typename T>
  inline Array(const SparseMatrix<T>& real) {
    m_type.Class = GetDataClass<T>();
    m_type.Complex = 0;
    m_type.Sparse = 1;
    m_type.Scalar = 0;
    m_real.p = new SharedObject(m_type,new SparseMatrix<T>(real));
  }
  template <typename T>
  Array(const SparseMatrix<T>& real, const SparseMatrix<T>& imag) {
    m_type.Class = GetDataClass<T>();
    m_type.Complex = 1;
    m_type.Sparse = 1;
    m_type.Scalar = 0;
    m_real.p = new SharedObject(m_type,new SparseMatrix<T>(real));
    m_imag.p = new SharedObject(m_type,new SparseMatrix<T>(imag));
  }
  static Array scalarConstructor(DataClass t) {
    Array ret;
    ret.m_type.Class = t;
    ret.m_type.Complex = 0;
    ret.m_type.Sparse = 0;
    ret.m_type.Scalar = 1;
    return ret;
  }
  const NTuple dimensions() const;
  const index_t length() const {return dimensions().count();}
  const index_t rows() const {return dimensions()[0];}
  const index_t columns() const {return dimensions()[1];}
  const index_t cols() const {return dimensions()[1];}
  inline const Type type() const { return m_type; }
  inline const DataClass dataClass() const {return m_type.Class;}
  QString className() const;
  bool isUserClass() const;
  inline bool isArray() const {return (m_type.Scalar == 0);}
  inline bool isVector() const {return dimensions().isVector();}
  inline bool isColumnVector() const {return dimensions().isColumnVector();}
  inline bool isRowVector() const {return dimensions().isRowVector();}
  inline bool is2D() const {return dimensions().is2D();}
  inline bool isSquare() const {return (is2D() && (rows() == columns()));}
  inline bool isString() const {return m_type.Class == StringArray;}
  inline bool isSparse() const {return m_type.Sparse == 1;}
  inline bool isReferenceType() const {
    return ((m_type.Class == Invalid) || (m_type.Class == CellArray) ||
	    (m_type.Class == Struct));
  }
  QString asString() const;
  int asInteger() const;
  double asDouble() const;
  inline bool isDouble() const {
    return (m_type.Class == Double);
  }
  inline bool isScalar() const {
    return ((m_type.Scalar == 1) || dimensions().isScalar());
  }
  template <typename T>
  inline BasicArray<T>& real() {
    return (*reinterpret_cast<BasicArray<T>*>(m_real.p->ptr()));
  }
  inline const StructArray& constStructPtr() const {
    return (*reinterpret_cast<const StructArray*>(m_real.p->ptr()));
  }
  inline StructArray& structPtr() {
    return (*reinterpret_cast<StructArray*>(m_real.p->ptr()));
  }
  template <typename T>
  inline const SparseMatrix<T>& constRealSparse() const {
    return (*reinterpret_cast<const SparseMatrix<T> *>(m_real.p->ptr()));
  }
  template <typename T>
  inline const SparseMatrix<T>& constImagSparse() const {
    if (!m_imag.p)
      throw Exception("Illegal request for imaginary part of real-only array");
    return (*reinterpret_cast<const SparseMatrix<T> *>(m_imag.p->ptr()));
  }
  template <typename T>
  inline SparseMatrix<T>& realSparse() {
    return (*reinterpret_cast<SparseMatrix<T> *>(m_real.p->ptr()));
  }
  template <typename T>
  inline SparseMatrix<T>& imagSparse() {
    if (!m_imag.p) {
      m_imag.p = new SharedObject(m_type, new SparseMatrix<T>(dimensions()));
      m_type.Complex = 1;
    }
    return (*reinterpret_cast<SparseMatrix<T> *>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constReal() const {
    return (*reinterpret_cast<const BasicArray<T>*>(m_real.p->ptr()));
  }						
  template <typename T>
  inline BasicArray<T>& imag() {
    if (!m_imag.p) {
      m_imag.p = new SharedObject(m_type, new BasicArray<T>(dimensions()));
      m_type.Complex = true;
    }
    return (*reinterpret_cast<BasicArray<T>*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constImag() const {
    if (!m_imag.p)
      throw Exception("Illegal request for imaginary part of real-only array");
    return (*reinterpret_cast<const BasicArray<T>*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline BasicArray<T> fortran() const {
    if (m_imag.p)
      return MergeComplex(constReal<T>(),constImag<T>());
    else
      return MergeComplex(constReal<T>());
  }
  template <typename T> inline T constRealScalar() const;
  template <typename T> inline T constImagScalar() const;
  template <typename T> inline T& realScalar();
  template <typename T> inline T& imagScalar();
  inline bool allReal() const {
    return (m_type.Complex == 0);
  }
  inline bool isComplex() const {
    return !allReal();
  }
  void forceComplex();
  const Array asComplex() const;
  const index_t asIndexScalar() const; 
  const Array toClass(DataClass t) const;

  const Array get(const IndexArray& index) const;
  const Array get(const Array& index) const;
  const Array get(index_t index) const;
  const ArrayVector get(const QString& field) const;

  const Array get(const NTuple& index) const;
  const Array get(const IndexArrayVector& indices) const;
  const Array get(const ArrayVector& indices) const;

  void set(const Array& index, const Array& data);
  void set(const IndexArray& index, const Array& data);
  void set(index_t index, const Array& data);

  void set(const ArrayVector& index, const Array& data);
  void set(const IndexArrayVector& index, const Array& data);
  void set(const NTuple& index, const Array& data);
  void set(const QString& field, ArrayVector& data);

  void resize(const NTuple &size);
  void resize(index_t size);
  void reshape(const NTuple &size);

  Array asDenseArray() const;
  void ensureNotScalarEncoded() {if (m_type.Scalar == 1) *this = asDenseArray();}
  inline bool isEmpty() const {return length() == 0;}

  bool operator==(const Array &b) const;
  inline bool operator!=(const Array &b) const {return !(*this == b);}
  void addField(QString name);
  void* getVoidPointer();
private:
  Data m_real;
  Data m_imag;
  Type m_type;
};

class ArrayIterator : public BaseIterator<Array, Array> {
public:
  ArrayIterator(Array *ptr, int dim) : 
    BaseIterator<Array,Array>(ptr,dim) {}
};

class ConstArrayIterator : public ConstBaseIterator<Array, Array> {
public:
  ConstArrayIterator(const Array *ptr, int dim) :
    ConstBaseIterator<Array,Array>(ptr,dim) {}
};

bool IsColonOp(const Array &arg);
bool IsNonNegative(const Array &arg);
bool IsInteger(const Array &arg);
bool IsUnsigned(const Array &arg);

const IndexArray IndexArrayFromArray(const Array &index);

const ArrayVector ArrayVectorFromCellArray(const Array &arg);
const Array CellArrayFromArrayVector(ArrayVector &arg, index_t cnt);
const Array CellArrayFromArray(const Array &arg);

void SetCellContents(Array &cell, const Array& index, ArrayVector& data);
void SetCellContents(Array &cell, const ArrayVector& index, ArrayVector& data);
QStringList FieldNames(const Array& arg);

bool IsCellStringArray(const Array &arg);

Array Vectorize(const Array& arg);

template <typename T>
SparseMatrix<T> ToRealSparse(const Array& data) {
  if (data.type().Sparse == 1) return data.constRealSparse<T>();
  Array cdata(data);
  if (cdata.isScalar())
    cdata = data.asDenseArray();
  if (!cdata.is2D()) throw Exception("Sparse matrix cannot be created from multidimensional arrays");
  return SparseMatrix<T>(cdata.constReal<T>());
}

template <typename T>
SparseMatrix<T> ToImagSparse(const Array& data) {
  if (data.type().Sparse == 1) return data.constImagSparse<T>();
  Array cdata(data);
  if (cdata.isScalar())
    cdata = cdata.asDenseArray();
  if (!cdata.is2D()) throw Exception("Sparse matrix cannot be created from multidimensional arrays");
  return SparseMatrix<T>(cdata.constImag<T>());
}

template <typename T>
Array DiagonalArray(const BasicArray<T> &in) {
  BasicArray<T> retmat(NTuple(in.length(),in.length()));
  for (index_t i=1;i<=in.length();i++)
    retmat.set(NTuple(i,i),in.get(i));
  return Array(retmat);
}

template <typename T>
Array DiagonalArray(const BasicArray<T> &in_r, 
		    const BasicArray<T> &in_i) {
  BasicArray<T> retmat_r(NTuple(in_r.length(),in_r.length()));
  BasicArray<T> retmat_i(NTuple(in_i.length(),in_i.length()));
  for (index_t i=1;i<=in_r.length();i++) {
    retmat_r.set(NTuple(i,i),in_r.get(i));
    retmat_i.set(NTuple(i,i),in_i.get(i));
  }
  return Array(retmat_r,retmat_i);
}

BasicArray<Array> ArrayVectorToBasicArray(const ArrayVector& a);

Array MatrixConstructor(const ArrayMatrix& data);
Array CellConstructor(const ArrayMatrix& data);
Array CellConstructor(const ArrayVector& data);
Array StructConstructor(const StringVector& fields, const ArrayVector& values);
inline Array EmptyConstructor() {return Array(Double);}

bool TestForCaseMatch(const Array &s, const Array &r);
bool RealAllZeros(const Array &t);

Type ScalarType(Type x);

Array GetDiagonal(const Array &a, int diagonal = 0);
Array DiagonalArray(const Array &f, int diagonal = 0);
bool IsSymmetric(const Array &a);

Array RangeConstructor(double start, double step, double stop, bool vertical);
Array Transpose(const Array &A);
Array Hermitian(const Array &A);
Array Negate(const Array &A);
Array Real(const Array &A);
Array Imag(const Array &A);

// Suppose we support a get/set interface:// And we support slicing through the iterators
// themselves.  For example, consider
// p = A(1:2:end,1:5:end);
// We can capture this as an iterator:
//  V = SliceIterator(A,1:2:end,1:5:end);
// Or in a variant itself.
//  B = Array(A,1:2:end,1:5:end)
// In which case, as long as we have get/set support
// for the variant class, we are OK.  We do need a
// sideband for the imaginary/real part of the equation


// How do we want to ultimately implement something like:
// A(1,:,3) = B(4,:)
// Where A and B are arbitrary variants.  In the name of 
// clarity, we have given up some of the performance.  One idea
// is to do something like this:

#include "ArrayPrivate.hpp"

#endif
