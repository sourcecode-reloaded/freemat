#ifndef __Variant_hpp__
#define __Variant_hpp__

#include <QSharedData>
#include "Types.hpp"
#include "BasicArray.hpp"
class StructArray;
class CellArray;
class StringArray;
class VariantList;

enum Type {
  Invalid = 0,
  Cell = 1,
  Struct = 2,
  String = 3,
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
  DoubleArray = 25
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
  template <typename T>
  inline Variant(T real, T imag = 0);
  Variant(const BasicArray<double> &r);
  Variant(Type t, const NTuple &dims);
  Variant(const QString &text);
  const NTuple dimensions() const;
  const index_t length() const {return dimensions().count();}
  const index_t rows() const {return dimensions()[0];}
  const index_t columns() const {return dimensions()[1];}
  inline const Type type() const { return m_type; }
  inline bool isArray() const {return m_type >= BoolArray;}
  inline bool isVector() const {return dimensions().isVector();}
  inline bool isColumnVector() const {return dimensions().isColumnVector();}
  inline bool isRowVector() const {return dimensions().isRowVector();}
  QString string() const;
  inline bool isScalar() const {
    return ((m_type >= BoolScalar) && (m_type <= DoubleScalar));
  }
  template <typename T>
  inline BasicArray<T>& real() {
    return (*reinterpret_cast<BasicArray<T>*>(m_real.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constReal() const {
    return (*reinterpret_cast<const BasicArray<T>*>(m_real.p->ptr()));
  }						
  template <typename T>
  inline BasicArray<T>& imag() {
    if (!m_imag.p)
      m_imag.p = new SharedObject(m_type, new BasicArray<T>(dimensions()));
    return (*reinterpret_cast<BasicArray<T>*>(m_imag.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constImag() const {
    if (!m_imag.p)
      throw Exception("Illegal request for imaginary part of real-only array");
    return (*reinterpret_cast<const BasicArray<T>*>(m_imag.p->ptr()));
  }						
  template <typename T>
  inline T realScalar() const;
  template <typename T>
  inline T imagScalar() const;
  void set(index_t pos, const Variant& val);
  const Variant get(index_t pos) const;
  inline bool allReal() const {return (!m_imag.p);}
  const Variant asScalar() const;
  const Variant getVectorSubset(const Variant& index) const;
  const Variant getNDimSubset(const VariantList& indices) const;
  void setVectorSubset(const Variant& index, const Variant& data);
  void setNDimSubset(const VariantList& index, const Variant& data);
  void deleteVectorSubset(const Variant& index);
  void deleteNDimSubset(const VariantList& index);
  void print(std::ostream& o) const;
  void resize(const NTuple &size);
  void resize(index_t size);
  void reshape(const NTuple &size);
  inline bool isEmpty() const {return length() == 0;}
private:
  Data m_real;
  Data m_imag;
  Type m_type;
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

BasicArray<index_t> IndexTypeFromVariant(const Variant &index, index_t len);
index_t IndexTypeFromVariantScalar(const Variant &index);

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
