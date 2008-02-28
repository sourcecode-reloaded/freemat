#ifndef __Variant_hpp__
#define __Variant_hpp__

#include <QSharedData>
#include "Types.hpp"
#include "BasicArray.hpp"
class StructArray;
class CellArray;
class StringArray;

enum Type {
  Invalid = 0,
  Cell = 1,
  Struct = 2,
  String = 3,
  Bool = 4,
  Int8 = 5,
  UInt8 = 6,
  Int16 = 7,
  UInt16 = 8,
  Int32 = 9,
  UInt32 = 10,
  Int64 = 11,
  UInt64 = 12,
  Float = 13,
  Double = 14,
  BoolArray = 18,
  Int8Array = 19,
  UInt8Array = 20,
  Int16Array = 21,
  UInt16Array = 22,
  Int32Array = 23,
  UInt32Array = 24,
  Int64Array = 25,
  UInt64Array = 26,
  FloatArray = 27,
  DoubleArray = 28,
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
    uint64        u64;
    double        d;
  };
} Data;

class Variant {
public:
  Variant();
  Variant(double real, double imag = 0);
  Variant(const BasicArray<double> &r);
  Variant(Type t, const NTuple &dims);
  inline const Type type() const { return m_type; }
  inline bool isArray() const {return m_type >= BoolArray;}
  template <typename T>
  inline BasicArray<T>& real() {
    return (*reinterpret_cast<BasicArray<T>*>(m_real.p->ptr()));
  }
  template <typename T>
  inline const BasicArray<T>& constReal() const {
    return (*reinterpret_cast<const BasicArray<T>*>(m_real.p->ptr()));
  }
private:
  Data m_real;
  Data m_imag;
  Type m_type;
};

#endif
