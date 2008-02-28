#ifndef __Variant_hpp__
#define __Variant_hpp__

#include <QSharedData>
#include "Types.hpp"
#include "BasicArray.hpp"
class StructArray;
class CellArray;
class StringArray;

class SharedObject;

typedef union {
  SharedObject *p;
  uint64        u64;
  double        d;
} Data;

class Variant {
public:
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
  Variant();
  Variant(double real, double imag = 0);
  Variant(BasicArray<double> *r);
  Variant& operator=(const Variant &v);
  inline Variant(const Variant &ref);
  inline ~Variant();
  inline void clear();
  inline const Type type() const { return m_type; }
  inline bool isArray() const {return m_type >= BoolArray;}
  template <typename T>
  inline BasicArray<T>& real() const;
  template <typename T>
  inline BasicArray<T>& imag() const;
private:
  Data m_real;
  Data m_imag;
  Type m_type;
};

class SharedObject : public QSharedData {
  Variant::Type m_type;
  void *m_p;
public:
  SharedObject(Variant::Type t, void* p) : m_type(t), m_p(p) {}
  ~SharedObject();
  inline void* ptr() const {return m_p;}
};

inline Variant::Variant(const Variant &ref) :
  m_real(ref.m_real), m_imag(ref.m_imag), m_type(ref.m_type) {
  if (isArray()) {
    if (m_real.p) m_real.p->ref.ref();
    if (m_imag.p) m_imag.p->ref.ref();
  }
}

inline Variant::~Variant() {
  clear();
}

inline void Variant::clear() {
  if (isArray()) {
    if (m_real.p && !m_real.p->ref.deref()) delete m_real.p;
    if (m_imag.p && !m_imag.p->ref.deref()) delete m_imag.p;
  }
}

template <typename T>
inline BasicArray<T>& Variant::real() const {
  return *(reinterpret_cast<BasicArray<T>*>(m_real.p->ptr()));
}

template <typename T>
inline BasicArray<T>& Variant::imag() const {
  return *(reinterpret_cast<BasicArray<T>*>(m_imag.p->ptr()));    
}

#endif
