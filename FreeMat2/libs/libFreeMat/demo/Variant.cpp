#include "Variant.hpp"

template <typename T>
inline SharedObject* Tconstruct(Variant::Type t,const void *copy) {
  const T* p = reinterpret_cast<const T*>(copy);
  return new SharedObject(t,reinterpret_cast<void*>(new T(*p)));
}

template <typename T>
inline void Tdestruct(void *todelete) {
  T* p = reinterpret_cast<T*>(todelete);
  delete p;
}

static SharedObject* construct(Variant::Type t, const void *copy) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case Variant::DoubleArray:
    return Tconstruct<BasicArray<double> >(t,copy);
  }
}

static void destruct(Variant::Type t, void *todelete) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case Variant::DoubleArray:
    return Tdestruct<BasicArray<double> >(todelete);
  }
}

Variant::Variant() {
  m_type = Invalid;
  m_real.u64 = 0;
  m_imag.u64 = 0;
}

Variant::Variant(double real, double imag) {
  m_type = Double;
  m_real.d = real;
  m_imag.d = imag;
}

Variant::Variant(const BasicArray<double> &r) {
  m_type = DoubleArray;
  m_real.p = construct(DoubleArray, &r);
  m_real.p->ref.ref();
  m_imag.p = NULL;
}

Variant& Variant::operator=(const Variant &v) {
  if (this == &v)
    return *this;
  clear();
  m_real = v.m_real;
  m_imag = v.m_imag;
  m_type = v.m_type;
  if (v.isArray()) {
    if (m_real.p) m_real.p->ref.ref();
    if (m_imag.p) m_imag.p->ref.ref();
  }
  return *this;
}


SharedObject::~SharedObject() {
  destruct(Variant::DoubleArray,m_p);
}

