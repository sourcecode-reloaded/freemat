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

typedef struct {
  unsigned m_type;
} DataClass;

const DataClass Invalid = {0};
const DataClass CellArray = {1};
const DataClass Struct = {2};
const DataClass StringArray = {3};
const DataClass Bool = {4};
const DataClass Int8 = {5};
const DataClass UInt8 = {6};
const DataClass Int16 = {7};
const DataClass UInt16 = {8};
const DataClass Int32 = {9};
const DataClass UInt32 = {10};
const DataClass Int64 = {11};
const DataClass UInt64 = {12};
const DataClass Float = {13};
const DataClass Double = {14};
const DataClass Index = {14};

template <typename T>
static inline DataClass GetDataClass(T = T());

size_t ByteSizeOfDataClass(DataClass);

typedef struct {
  unsigned Class : 5;
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
  inline Array() {m_type.Class = Invalid.m_type;}
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
  Array(DataClass t, const NTuple &dims);
  explicit Array(const QChar &t);
  explicit Array(const QChar &, const QChar &);
  explicit Array(const QString &text);
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
  Array(const StructArray& real);
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
    if (!m_real.p)
      throw Exception("Illegal request for real array part of scalar - this is bug!");
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<BasicArray<T>*>(m_real.p->ptr()));
  }
  inline const StructArray& constStructPtr() const {
    if (!m_real.p)
      throw Exception("Illegal request for real part of undefined structure array - this is bug!");
    return (*reinterpret_cast<const StructArray*>(m_real.p->ptr()));
  }
  inline StructArray& structPtr() {
    if (!m_real.p)
      throw Exception("Illegal request for real part of undefined structure array - this is bug!");
    return (*reinterpret_cast<StructArray*>(m_real.p->ptr()));
  }
  template <typename T>
  inline const SparseMatrix<T>& constRealSparse() const {
    if (!m_real.p)
      throw Exception("Illegal request for (sparse) real part of scalar - this is bug!");
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<const SparseMatrix<T> *>(m_real.p->ptr()));
  }
  template <typename T>
  inline const SparseMatrix<T>& constImagSparse() const {
    if (!m_imag.p)
      throw Exception("Illegal request for (sparse) imaginary part of real-only array");
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<const SparseMatrix<T> *>(m_imag.p->ptr()));
  }
  template <typename T>
  inline SparseMatrix<T>& realSparse() {
    if (!m_real.p)
      throw Exception("Illegal request for (sparse) real part of scalar - this is bug!");
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<SparseMatrix<T> *>(m_real.p->ptr()));
  }
  template <typename T>
  inline SparseMatrix<T>& imagSparse() {
    if (!m_imag.p) {
      m_imag.p = new SharedObject(m_type, new SparseMatrix<T>(dimensions()));
      m_type.Complex = 1;
    }
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<SparseMatrix<T> *>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constReal() const {
    if (!m_real.p)
      throw Exception("Illegal request for real part of scalar - this is bug!");
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<const BasicArray<T>*>(m_real.p->ptr()));
  }						
  template <typename T>
  inline BasicArray<T>& imag() {
    if (!m_imag.p) {
      m_imag.p = new SharedObject(m_type, new BasicArray<T>(dimensions()));
      m_type.Complex = true;
    }
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<BasicArray<T>*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constImag() const {
    if (!m_imag.p)
      throw Exception("Illegal request for imaginary part of real-only array");
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
    return (*reinterpret_cast<const BasicArray<T>*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline BasicArray<T> fortran() const {
    if (m_type.Class != GetDataClass(T(0))) throw Exception("data type mismatch in array -- bug");
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
  inline bool isEmpty() const {return ((m_type.Class == Invalid) || (length() == 0));}

  bool operator==(const Array &b) const;
  inline bool operator!=(const Array &b) const {return !(*this == b);}
  void addField(QString name);
  void* getVoidPointer();
  const void* getConstVoidPointer() const;
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

#include "ArrayPrivate.hpp"

#endif
