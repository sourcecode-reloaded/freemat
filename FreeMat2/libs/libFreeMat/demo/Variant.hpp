#ifndef __Variant_hpp__
#define __Variant_hpp__

#include <QSharedData>
#include "Types.hpp"
#include "BasicArray.hpp"
#include "FastList.hpp"
#include "SparseMatrix.hpp"
class CellArray;
class StringArray;

class Variant;

typedef FastList<Variant> VariantList;
typedef QMap<QString,BasicArray<Variant> > StructArray;

enum Type {
  Invalid = 0,
  CellArray = 1,
  Struct = 2,
  StringArray = 3,
  BoolScalar = 4,
  Int8Scalar = 5,
  UInt8Scalar = 6,
  Int16Scalar = 7,
  UInt16Scalar = 8,
  Int32Scalar = 9,
  UInt32Scalar = 10,
  Int64Scalar = 11,
  UInt64Scalar = 12,
  FloatScalar = 13,
  DoubleScalar = 14,
  BoolArray = 15,
  Int8Array = 16,
  UInt8Array = 17,
  Int16Array = 18,
  UInt16Array = 19,
  Int32Array = 20,
  UInt32Array = 21,
  Int64Array = 22,
  UInt64Array = 23,
  FloatArray = 24,
  DoubleArray = 25,
  DoubleSparse = 26
};

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
    logical l;
    int8 i8;
    uint8 u8;
    int16 i16;
    uint16 u16;
    int32 i32;
    uint32 u32;
    int64 i64;
    uint64 u64;
    float f;
    double d;
  };
} Data;

class Variant {
public:
  inline Variant() {}
  // Defined in VariantPrivate
  template <typename T> inline explicit Variant(T real); 
  template <typename T> inline explicit Variant(T real, T imag); 
  template <typename T> 
  inline Variant(Type t, const BasicArray<T> &r) {
    m_type = t;
    m_real.p = new SharedObject(t, new BasicArray<T>(r));
    m_complex = false;
  }
  template <typename T> 
  inline Variant(Type t, const BasicArray<T> &r, const BasicArray<T> &i) {
    m_type = t;
    m_real.p = new SharedObject(t, new BasicArray<T>(r));
    m_imag.p = new SharedObject(t, new BasicArray<T>(i));
    m_complex = true;
  }
  template <typename T>
  inline Variant(Type t, BasicArray<T> *r) {
    m_type = t;
    m_real.p = new SharedObject(t,r);
    m_complex = false;
  }
  template <typename T>
  inline Variant(Type t, BasicArray<T> *r, BasicArray<T> *i) {
    m_type = t;
    m_real.p = new SharedObject(t,r);
    m_imag.p = new SharedObject(t,i);
    m_complex = true;
  }
  Variant(Type t, const NTuple &dims = NTuple(0,0));
  Variant(const QString &text);
  Variant(const SparseMatrix& real);
  Variant(const SparseMatrix& real, const SparseMatrix& imag);
  const NTuple dimensions() const;
  const index_t length() const {return dimensions().count();}
  const index_t rows() const {return dimensions()[0];}
  const index_t columns() const {return dimensions()[1];}
  inline const Type type() const { return m_type; }
  inline bool isArray() const {return m_type >= BoolArray;}
  inline bool isVector() const {return dimensions().isVector();}
  inline bool isColumnVector() const {return dimensions().isColumnVector();}
  inline bool isRowVector() const {return dimensions().isRowVector();}
  inline bool is2D() const {return dimensions().is2D();}
  QString string() const;
  inline bool isScalar() const {
    return (((m_type >= BoolScalar) && (m_type <= DoubleScalar)) ||
	    dimensions().isScalar());
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
  inline const SparseMatrix& constRealSparse() const {
    return (*reinterpret_cast<const SparseMatrix*>(m_real.p->ptr()));
  }
  inline const SparseMatrix& constImagSparse() const {
    if (!m_imag.p)
      throw Exception("Illegal request for imaginary part of real-only array");
    return (*reinterpret_cast<const SparseMatrix*>(m_imag.p->ptr()));
  }
  inline SparseMatrix& realSparse() {
    return (*reinterpret_cast<SparseMatrix*>(m_real.p->ptr()));
  }
  inline SparseMatrix& imagSparse() {
    if (!m_imag.p) {
      m_imag.p = new SharedObject(m_type, new SparseMatrix(dimensions()));
      m_complex = true;
    }
    return (*reinterpret_cast<SparseMatrix*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constReal() const {
    return (*reinterpret_cast<const BasicArray<T>*>(m_real.p->ptr()));
  }						
  template <typename T>
  inline BasicArray<T>& imag() {
    if (!m_imag.p) {
      m_imag.p = new SharedObject(m_type, new BasicArray<T>(dimensions()));
      m_complex = true;
    }
    return (*reinterpret_cast<BasicArray<T>*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constImag() const {
    if (!m_imag.p)
      throw Exception("Illegal request for imaginary part of real-only array");
    return (*reinterpret_cast<const BasicArray<T>*>(m_imag.p->ptr()));
  }						
  template <typename T> inline T constRealScalar() const;
  template <typename T> inline T constImagScalar() const;
  template <typename T> inline T& realScalar();
  template <typename T> inline T& imagScalar();
  inline bool allReal() const {
    return (!m_complex);
  }
  const Variant asScalar() const;
  const index_t asIndexScalar() const; 
  const Variant toType(const Type t) const;

  const Variant get(const IndexArray& index) const;
  const Variant get(const Variant& index) const;
  const Variant get(index_t index) const;
  const VariantList get(const QString& field) const;

  const Variant get(const NTuple& index) const;
  const Variant get(const IndexArrayList& indices) const;
  const Variant get(const VariantList& indices) const;

  void set(const Variant& index, const Variant& data);
  void set(const IndexArray& index, const Variant& data);
  void set(index_t index, const Variant& data);

  void set(const VariantList& index, const Variant& data);
  void set(const IndexArrayList& index, const Variant& data);
  void set(const NTuple& index, const Variant& data);
  void set(const QString& field, VariantList& data);

  void print(std::ostream& o) const;
  void resize(const NTuple &size);
  void resize(index_t size);
  void reshape(const NTuple &size);

  Variant asArrayType() const;
  inline bool isEmpty() const {return length() == 0;}

  bool operator==(const Variant &b) const;
  inline bool operator!=(const Variant &b) const {return !(*this == b);}
  void addField(QString name);
private:
  Data m_real;
  Data m_imag;
  Type m_type;
  bool m_complex;
};

class VariantIterator : public BaseIterator<Variant, Variant> {
public:
  VariantIterator(Variant *ptr, int dim) : 
    BaseIterator<Variant,Variant>(ptr,dim) {}
};

class ConstVariantIterator : public ConstBaseIterator<Variant, Variant> {
public:
  ConstVariantIterator(const Variant *ptr, int dim) :
    ConstBaseIterator<Variant,Variant>(ptr,dim) {}
};

std::ostream& operator<<(std::ostream& o, const Variant &t);

bool IsColonOp(const Variant &arg);

const IndexArray IndexArrayFromVariant(const Variant &index);

const VariantList VariantListFromCellArray(const Variant &arg);
const Variant CellArrayFromVariantList(VariantList &arg, index_t cnt);
void SetCellContents(Variant &cell, const Variant& index, VariantList& data);
void SetCellContents(Variant &cell, const VariantList& index, VariantList& data);
QStringList FieldNames(const Variant& arg);

SparseMatrix ToRealSparse(const Variant& data);
SparseMatrix ToImagSparse(const Variant& data);

// Suppose we support a get/set interface:
// And we support slicing through the iterators
// themselves.  For example, consider
// p = A(1:2:end,1:5:end);
// We can capture this as an iterator:
//  V = SliceIterator(A,1:2:end,1:5:end);
// Or in a variant itself.
//  B = Variant(A,1:2:end,1:5:end)
// In which case, as long as we have get/set support
// for the variant class, we are OK.  We do need a
// sideband for the imaginary/real part of the equation


// How do we want to ultimately implement something like:
// A(1,:,3) = B(4,:)
// Where A and B are arbitrary variants.  In the name of 
// clarity, we have given up some of the performance.  One idea
// is to do something like this:

#include "VariantPrivate.hpp"

#endif
