#include "Variant.hpp"

template <typename T>
inline void* Tconstruct(const void *copy) {
  const T* p = reinterpret_cast<const T*>(copy);
  return reinterpret_cast<void*>(new T(*p));
}

template <typename T>
inline void* Tconstruct_sized(const NTuple &dims) {
  return reinterpret_cast<void*>(new T(dims));
}

template <typename T>
inline void Tdestruct(void *todelete) {
  T* p = reinterpret_cast<T*>(todelete);
  delete p;
}

static void* construct(Type t, const void *copy) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case DoubleArray:
    return Tconstruct<BasicArray<double> >(copy);
  }
}

static void* construct_sized(Type t, const NTuple &dims) {
  switch (t) {
  default:
    throw Exception("Unsupported sized construct");
  case DoubleArray:
    return Tconstruct_sized<BasicArray<double> >(dims);
  }
}

static void destruct(Type t, void *todelete) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case DoubleArray:
    return Tdestruct<BasicArray<double> >(todelete);
  }
}

SharedObject::SharedObject(Type t, void* p) : m_type(t), m_p(p) {}

SharedObject::SharedObject(const SharedObject& copy) : 
  QSharedData(copy), m_type(copy.m_type) {
  qDebug() << "SO copy";
  m_p = construct(m_type,copy.m_p);
}

SharedObject& SharedObject::operator=(const SharedObject& copy) {
  if (this == &copy) return *this;
  destruct(m_type,m_p);
  m_type = copy.m_type;
  m_p = construct(m_type,copy.m_p);
  return *this;
}

SharedObject::~SharedObject() {
  destruct(m_type,m_p);
}

Variant::Variant() {
  m_type = Invalid;
}

Variant::Variant(double real, double imag) {
  m_type = Double;
  m_real.d = real;
  m_imag.d = imag;
}

Variant::Variant(const BasicArray<double> &r) {
  m_type = DoubleArray;
  m_real.p = new SharedObject(DoubleArray, new BasicArray<double>(r));
}

Variant::Variant(Type t, const NTuple &dims) {
  m_type = t;
  m_real.p = new SharedObject(t,construct_sized(t,dims));
}
