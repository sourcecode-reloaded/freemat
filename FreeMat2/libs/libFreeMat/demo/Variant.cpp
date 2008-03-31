#include "Variant.hpp"

void Warn(const char *msg) {
  std::cout << "Warning:" << msg;
}

template <typename T>
static inline void* Tconstruct(const void *copy) {
  const T* p = reinterpret_cast<const T*>(copy);
  return reinterpret_cast<void*>(new T(*p));
}

template <typename T>
static inline void* Tconstruct_sized(const NTuple &dims) {
  return reinterpret_cast<void*>(new T(dims));
}

template <typename T>
static inline void Tdestruct(void *todelete) {
  T* p = reinterpret_cast<T*>(todelete);
  delete p;
}

template <typename S, typename T>
static inline double Tget_real(const Variant*ptr, S ndx) {
  const BasicArray<T>& real(ptr->constReal<T>());
  return real.get(ndx);
}

template <typename S, typename T>
static inline double Tget_imag(const Variant*ptr, S ndx) {
  if (ptr->allReal()) return T();
  const BasicArray<T>& imag(ptr->constImag<T>());
  return imag.get(ndx);
}

template <typename S, typename T>
static inline const Variant Tget(const Variant*ptr, S ndx) {
  if (ptr->allReal()) {
    const BasicArray<T> &real(ptr->constReal<T>());
    return Variant(ptr->type(),real.get(ndx));
  } else {
    const BasicArray<T> &real(ptr->constReal<T>());
    const BasicArray<T> &imag(ptr->constImag<T>());
    return Variant(ptr->type(),real.get(ndx),imag.get(ndx));
  }
}

static inline Type ScalarType(Type x) {
  switch (x) {
  default:
    return x;
  case BoolArray:
    return BoolScalar;
  case Int8Array:
    return Int8Scalar;
  case UInt8Array:
    return UInt8Scalar;
  case Int16Array:
    return Int16Scalar;
  case UInt16Array:
    return UInt16Scalar;
  case Int32Array:
    return Int32Scalar;
  case UInt32Array:
    return UInt32Scalar;
  case Int64Array:
    return Int64Scalar;
  case UInt64Array:
    return UInt64Scalar;
  case FloatArray:
    return FloatScalar;
  case DoubleArray:
    return DoubleScalar;
  }
}

template <typename S, typename T>
static inline void Tset_scalar_rhs(Variant* ptr, S ndx, const Variant& data) {
  BasicArray<T> &real(ptr->real<T>());
  Variant dataTyped(data.toType(ScalarType(ptr->type())));
  real.set(ndx,dataTyped.realScalar<T>());
  if (!data.allReal()) {
    BasicArray<T> &imag(ptr->imag<T>());
    imag.set(ndx,dataTyped.imagScalar<T>());
  }
}

template <typename S, typename T>
static inline void Tset(Variant* ptr, S ndx, const Variant& data) {
  if (data.isScalar()) {
    Tset_scalar_rhs<S, T>(ptr,ndx,data);
    return;
  }
  BasicArray<T> &real(ptr->real<T>());
  Variant dataTyped(data.toType(ptr->type()));
  real.set(ndx,dataTyped.constReal<T>());
  if (!data.allReal()) {
    BasicArray<T> &imag(ptr->imag<T>());
    imag.set(ndx,dataTyped.constImag<T>());
  }
}


// S - source type, T - dest type
template <typename S, typename T>
inline static const Variant Tcast_case(Type t, const Variant *copy) {
  Variant retvec(t,copy->dimensions());
  if (copy->allReal()) {
    ConvertBasicArray(copy->constReal<S>(),retvec.real<T>());
  } else {
    ConvertBasicArray(copy->constReal<S>(),retvec.real<T>());
    ConvertBasicArray(copy->constImag<S>(),retvec.imag<T>());
  }
  return retvec;
}


template <typename T>
inline static const Variant TcastScalar(Type t, const Variant *copy) {
  if (copy->type() == t) return *copy;
  switch (copy->type()) {
  default:
    throw Exception("Unsupported type for conversion");
  case BoolScalar:
    return Variant((T)copy->realScalar<bool>());
  case Int8Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<int8>(),(T)copy->imagScalar<int8>());
    else
      return Variant((T)copy->realScalar<int8>());
  case UInt8Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<uint8>(),(T)copy->imagScalar<uint8>());
    else
      return Variant((T)copy->realScalar<uint8>());
  case Int16Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<int16>(),(T)copy->imagScalar<int16>());
    else
      return Variant((T)copy->realScalar<int16>());
  case UInt16Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<uint16>(),(T)copy->imagScalar<uint16>());
    else
      return Variant((T)copy->realScalar<uint16>());
  case Int32Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<int32>(),(T)copy->imagScalar<int32>());
    else
      return Variant((T)copy->realScalar<int32>());
  case UInt32Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<uint32>(),(T)copy->imagScalar<uint32>());
    else
      return Variant((T)copy->realScalar<uint32>());
  case Int64Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<int64>(),(T)copy->imagScalar<int64>());
    else
      return Variant((T)copy->realScalar<int64>());
  case UInt64Scalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<uint64>(),(T)copy->imagScalar<uint64>());
    else
      return Variant((T)copy->realScalar<uint64>());
  case FloatScalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<float>(),(T)copy->imagScalar<float>());
    else
      return Variant((T)copy->realScalar<float>());
  case DoubleScalar:
    if (!copy->allReal())
      return Variant((T)copy->realScalar<double>(),(T)copy->imagScalar<double>());
    else
      return Variant((T)copy->realScalar<double>());
  }
}

// T - dest type
template <typename T>
inline static const Variant Tcast(Type t, const Variant *copy) {
  if (copy->type() == t) return *copy;
  switch(copy->type()) {
  default:
    throw Exception("Unsupported type for conversion");
  case BoolArray:
    return Tcast_case<bool,T>(t,copy);
  case Int8Array:
    return Tcast_case<int8,T>(t,copy);
  case UInt8Array:
    return Tcast_case<uint8,T>(t,copy);
  case Int16Array:
    return Tcast_case<int16,T>(t,copy);
  case UInt16Array:
    return Tcast_case<uint16,T>(t,copy);
  case Int32Array:
    return Tcast_case<int32,T>(t,copy);
  case UInt32Array:
    return Tcast_case<uint32,T>(t,copy);
  case Int64Array:
    return Tcast_case<int64,T>(t,copy);
  case UInt64Array:
    return Tcast_case<uint64,T>(t,copy);
  case FloatArray:
    return Tcast_case<float,T>(t,copy);
  case DoubleArray:
    return Tcast_case<double,T>(t,copy);
  }
}

static inline void* construct(Type t, const void *copy) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case String:
    return Tconstruct<BasicArray<uint16> >(copy);
  case BoolArray:
    return Tconstruct<BasicArray<logical> >(copy);
  case Int8Array:
    return Tconstruct<BasicArray<int8> >(copy);
  case UInt8Array:
    return Tconstruct<BasicArray<uint8> >(copy);
  case Int16Array:
    return Tconstruct<BasicArray<int16> >(copy);
  case UInt16Array:
    return Tconstruct<BasicArray<uint16> >(copy);
  case Int32Array:
    return Tconstruct<BasicArray<int32> >(copy);
  case UInt32Array:
    return Tconstruct<BasicArray<uint32> >(copy);
  case Int64Array:
    return Tconstruct<BasicArray<int64> >(copy);
  case UInt64Array:
    return Tconstruct<BasicArray<uint64> >(copy);
  case FloatArray:
    return Tconstruct<BasicArray<float> >(copy);
  case DoubleArray:
    return Tconstruct<BasicArray<double> >(copy);
  }
}

static inline void* construct_sized(Type t, const NTuple &dims) {
  switch (t) {
  default:
  case String:
    return Tconstruct_sized<BasicArray<uint16> >(dims);
  case BoolArray:
    return Tconstruct_sized<BasicArray<logical> >(dims);
  case Int8Array:
    return Tconstruct_sized<BasicArray<int8> >(dims);
  case UInt8Array:
    return Tconstruct_sized<BasicArray<uint8> >(dims);
  case Int16Array:
    return Tconstruct_sized<BasicArray<int16> >(dims);
  case UInt16Array:
    return Tconstruct_sized<BasicArray<uint16> >(dims);
  case Int32Array:
    return Tconstruct_sized<BasicArray<int32> >(dims);
  case UInt32Array:
    return Tconstruct_sized<BasicArray<uint32> >(dims);
  case Int64Array:
    return Tconstruct_sized<BasicArray<int64> >(dims);
  case UInt64Array:
    return Tconstruct_sized<BasicArray<uint64> >(dims);
  case FloatArray:
    return Tconstruct_sized<BasicArray<float> >(dims);
  case DoubleArray:
    return Tconstruct_sized<BasicArray<double> >(dims);
  }
}

static inline void destruct(Type t, void *todelete) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case String:
    return Tdestruct<BasicArray<uint16> >(todelete);
  case BoolArray:
    return Tdestruct<BasicArray<logical> >(todelete);
  case Int8Array:
    return Tdestruct<BasicArray<int8> >(todelete);
  case UInt8Array:
    return Tdestruct<BasicArray<uint8> >(todelete);
  case Int16Array:
    return Tdestruct<BasicArray<int16> >(todelete);
  case UInt16Array:
    return Tdestruct<BasicArray<uint16> >(todelete);
  case Int32Array:
    return Tdestruct<BasicArray<int32> >(todelete);
  case UInt32Array:
    return Tdestruct<BasicArray<uint32> >(todelete);
  case Int64Array:
    return Tdestruct<BasicArray<int64> >(todelete);
  case UInt64Array:
    return Tdestruct<BasicArray<uint64> >(todelete);
  case FloatArray:
    return Tdestruct<BasicArray<float> >(todelete);
  case DoubleArray:
    return Tdestruct<BasicArray<double> >(todelete);
  }
}

static inline bool AllScalars(const VariantList& index) {
  for (int i=0;i<index.size();i++)
    if (!index[i].isScalar()) return false;
  return true;
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

#if 0
  switch(m_type) {
  default:
    throw Exception("Unhandle scalar type");
  case Bool:
    m_real.b = (real != 0);
    m_imag.b = (imag != 0);
    return;
  case Int8:
    m_real.i8 = Bound((int8) SCHAR_MIN, (int8) SCHAR_MAX, real);
    m_imag.i64 = Bound((int8) SCHAR_MIN, (int8) SCHAR_MAX, imag);
    return;
  case UInt8:
    m_real.u8 = Bound((uint8) 0, (uint8) UCHAR_MAX, real);
    m_imag.u8 = Bound((uint8) 0, (uint8) UCHAR_MAX, imag);
    return;
  case Int16:
    m_real.i16 = Bound((int16) SHRT_MIN, (int16) USHRT_MAX, real);
    m_imag.i16 = Bound((int16) SHRT_MIN, (int16) USHRT_MAX, imag);
    return;
  case UInt16:
    m_real.u16 = Bound((uint16) 0, (uint16) USHRT_MAX, real);
    m_imag.u16 = Bound((uint16) 0, (uint16) USHRT_MAX, imag);
    return;
  case Int32:
    m_real.i32 = Bound((int32) INT_MIN, (int32) INT_MAX, real);
    m_imag.i32 = Bound((int32) INT_MIN, (int32) INT_MAX, imag);
    return;
  case UInt32:
    m_real.u32 = Bound((uint32) 0, (uint32) UINT_MAX, real);
    m_imag.u32 = Bound((uint32) 0, (uint32) UINT_MAX, imag);
    return;
  case Int64:
    m_real.i64 = Bound((int64) LLONG_MIN, (int64) LLONG_MAX, real);
    m_imag.i64 = Bound((int64) LLONG_MIN, (int64) LLONG_MAX, imag);
    return;
  case UInt64:
    m_real.u64 = Bound((uint64) 0, (uint64) ULLONG_MAX, real);
    m_imag.u64 = Bound((uint64) 0, (uint64) ULLONG_MAX, imag);
    return;
  case Float:
    m_real.f = real; m_imag.f = imag;
    return;
  }
#endif

void Variant::print(std::ostream& o) const {
  switch (m_type) {
  default:
    throw Exception("unsupported type");
  case UInt8Scalar:
    if (m_complex)
      o << "(" << (int) m_real.u8 << "," << (int) m_imag.u8 << ")"; 
    else
      o << (int) m_real.u8;
    return;
  case UInt16Scalar:
    if (m_complex)
      o << "(" << m_real.u16 << "," << m_imag.u16 << ")";
    else
      o << m_real.u16;
    return;
  case UInt32Scalar:
    if (m_complex)
      o << "(" << m_real.u32 << "," << m_imag.u32 << ")";
    else
      o << m_real.u32;
    return;
  case UInt64Scalar:
    if (m_complex)
      o << "(" << m_real.u64 << "," << m_imag.u64 << ")";
    else
      o << m_real.u64;
    return;
  case Int8Scalar:
    if (m_complex)
      o << "(" << (int) m_real.i8 << "," << (int) m_imag.i8 << ")";
    else
      o << (int) m_real.i8;
    return;
  case Int16Scalar:
    if (m_complex)
      o << "(" << m_real.i16 << "," << m_imag.i16 << ")";
    else
      o << m_real.i16;
    return;
  case Int32Scalar:
    if (m_complex)
      o << "(" << m_real.i32 << "," << m_imag.i32 << ")";
    else
      o << m_real.i32;
    return;
  case Int64Scalar:
    if (m_complex)
      o << "(" << m_real.i64 << "," << m_imag.i64 << ")";
    else
      o << m_real.i64;
    return;
  case FloatScalar:
    if (m_complex)
      o << "(" << m_real.f << "," << m_imag.f << ")";
    else
      o << m_real.f;
    return;
  case DoubleScalar:
    if (m_complex)
      o << "(" << m_real.d << "," << m_imag.d << ")";
    else
      o << m_real.d;
    return;
  }
}

Variant::Variant(Type t, const NTuple &dims) {
  m_type = t;
  m_real.p = new SharedObject(t,construct_sized(t,dims));
  m_complex = false;
}

Variant::Variant(const QString &text) {
  m_type = String;
  m_real.p = new SharedObject(String,
			      construct_sized(String,
					     NTuple(1,text.size())));
  BasicArray<uint16> &p(real<uint16>());
  for (int i=0;i<text.size();i++) 
    p[i+1] = text[i].unicode();
  m_complex = false;
}

const NTuple Variant::dimensions() const {
  switch (m_type) {
  default:
    throw Exception("unhandled type");
  case BoolScalar:
  case Int8Scalar:
  case UInt8Scalar:
  case Int16Scalar:
  case UInt16Scalar:
  case Int32Scalar:
  case UInt32Scalar:
  case Int64Scalar:
  case UInt64Scalar:
  case FloatScalar:
  case DoubleScalar:
    return NTuple(1,1);
  case BoolArray:
    return (reinterpret_cast<const BasicArray<logical>*>(m_real.p->ptr())->dimensions());
  case Int8Array:
    return (reinterpret_cast<const BasicArray<int8>*>(m_real.p->ptr())->dimensions());
  case UInt8Array:
    return (reinterpret_cast<const BasicArray<uint8>*>(m_real.p->ptr())->dimensions());
  case Int16Array:
    return (reinterpret_cast<const BasicArray<int16>*>(m_real.p->ptr())->dimensions());
  case String:
  case UInt16Array:
    return (reinterpret_cast<const BasicArray<uint16>*>(m_real.p->ptr())->dimensions());
  case Int32Array:
    return (reinterpret_cast<const BasicArray<int32>*>(m_real.p->ptr())->dimensions());
  case UInt32Array:
    return (reinterpret_cast<const BasicArray<uint32>*>(m_real.p->ptr())->dimensions());
  case Int64Array:
    return (reinterpret_cast<const BasicArray<int64>*>(m_real.p->ptr())->dimensions());
  case UInt64Array:
    return (reinterpret_cast<const BasicArray<uint64>*>(m_real.p->ptr())->dimensions());
  case FloatArray:
    return (reinterpret_cast<const BasicArray<float>*>(m_real.p->ptr())->dimensions());
  case DoubleArray:
    return (reinterpret_cast<const BasicArray<double>*>(m_real.p->ptr())->dimensions());
  };
}

void Variant::set(index_t index, const Variant& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(index_t)");
  case BoolArray:
    Tset_scalar_rhs<index_t,logical>(this,index,data);
    return;
  case Int8Array:
    Tset_scalar_rhs<index_t,int8>(this,index,data);
    return;
  case UInt8Array:
    Tset_scalar_rhs<index_t,uint8>(this,index,data);
    return;
  case Int16Array:
    Tset_scalar_rhs<index_t,int16>(this,index,data);
    return;
  case UInt16Array:
    Tset_scalar_rhs<index_t,uint16>(this,index,data);
    return;
  case Int32Array:
    Tset_scalar_rhs<index_t,int32>(this,index,data);
    return;
  case UInt32Array:
    Tset_scalar_rhs<index_t,uint32>(this,index,data);
    return;
  case Int64Array:
    Tset_scalar_rhs<index_t,int64>(this,index,data);
    return;
  case UInt64Array:
    Tset_scalar_rhs<index_t,uint64>(this,index,data);
    return;
  case FloatArray:
    Tset_scalar_rhs<index_t,float>(this,index,data);
    return;
  case DoubleArray:
    Tset_scalar_rhs<index_t,double>(this,index,data);
    return;
  }  
}

void Variant::set(const NTuple& index, const Variant& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(const NTuple&)");
  case BoolArray:
    Tset_scalar_rhs<const NTuple&,logical>(this,index,data);
    return;
  case Int8Array:
    Tset_scalar_rhs<const NTuple&,int8>(this,index,data);
    return;
  case UInt8Array:
    Tset_scalar_rhs<const NTuple&,uint8>(this,index,data);
    return;
  case Int16Array:
    Tset_scalar_rhs<const NTuple&,int16>(this,index,data);
    return;
  case UInt16Array:
    Tset_scalar_rhs<const NTuple&,uint16>(this,index,data);
    return;
  case Int32Array:
    Tset_scalar_rhs<const NTuple&,int32>(this,index,data);
    return;
  case UInt32Array:
    Tset_scalar_rhs<const NTuple&,uint32>(this,index,data);
    return;
  case Int64Array:
    Tset_scalar_rhs<const NTuple&,int64>(this,index,data);
    return;
  case UInt64Array:
    Tset_scalar_rhs<const NTuple&,uint64>(this,index,data);
    return;
  case FloatArray:
    Tset_scalar_rhs<const NTuple&,float>(this,index,data);
    return;
  case DoubleArray:
    Tset_scalar_rhs<const NTuple&,double>(this,index,data);
    return;
  }  
}

void Variant::set(const IndexArray& index, const Variant& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(IndexArray)");
  case BoolArray:
    Tset<const IndexArray&,logical>(this,index,data);
    return;
  case Int8Array:
    Tset<const IndexArray&,int8>(this,index,data);
    return;
  case UInt8Array:
    Tset<const IndexArray&,uint8>(this,index,data);
    return;
  case Int16Array:
    Tset<const IndexArray&,int16>(this,index,data);
    return;
  case UInt16Array:
    Tset<const IndexArray&,uint16>(this,index,data);
    return;
  case Int32Array:
    Tset<const IndexArray&,int32>(this,index,data);
    return;
  case UInt32Array:
    Tset<const IndexArray&,uint32>(this,index,data);
    return;
  case Int64Array:
    Tset<const IndexArray&,int64>(this,index,data);
    return;
  case UInt64Array:
    Tset<const IndexArray&,uint64>(this,index,data);
    return;
  case FloatArray:
    Tset<const IndexArray&,float>(this,index,data);
    return;
  case DoubleArray:
    Tset<const IndexArray&,double>(this,index,data);
    return;
  }  
}

void Variant::set(const IndexArrayList& index, const Variant& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(IndexArray)");
  case BoolArray:
    Tset<const IndexArrayList&,logical>(this,index,data);
    return;
  case Int8Array:
    Tset<const IndexArrayList&,int8>(this,index,data);
    return;
  case UInt8Array:
    Tset<const IndexArrayList&,uint8>(this,index,data);
    return;
  case Int16Array:
    Tset<const IndexArrayList&,int16>(this,index,data);
    return;
  case UInt16Array:
    Tset<const IndexArrayList&,uint16>(this,index,data);
    return;
  case Int32Array:
    Tset<const IndexArrayList&,int32>(this,index,data);
    return;
  case UInt32Array:
    Tset<const IndexArrayList&,uint32>(this,index,data);
    return;
  case Int64Array:
    Tset<const IndexArrayList&,int64>(this,index,data);
    return;
  case UInt64Array:
    Tset<const IndexArrayList&,uint64>(this,index,data);
    return;
  case FloatArray:
    Tset<const IndexArrayList&,float>(this,index,data);
    return;
  case DoubleArray:
    Tset<const IndexArrayList&,double>(this,index,data);
    return;
  }  
}

const Variant Variant::get(const IndexArray& index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for get(index_t)");
  case BoolArray:
    return Tget<const IndexArray&,logical>(this,index);
  case Int8Array:
    return Tget<const IndexArray&,int8>(this,index);
  case UInt8Array:
    return Tget<const IndexArray&,uint8>(this,index);
  case Int16Array:
    return Tget<const IndexArray&,int16>(this,index);
  case UInt16Array:
    return Tget<const IndexArray&,uint16>(this,index);
  case Int32Array:
    return Tget<const IndexArray&,int32>(this,index);
  case UInt32Array:
    return Tget<const IndexArray&,uint32>(this,index);
  case Int64Array:
    return Tget<const IndexArray&,int64>(this,index);
  case UInt64Array:
    return Tget<const IndexArray&,uint64>(this,index);
  case FloatArray:
    return Tget<const IndexArray&,float>(this,index);
  case DoubleArray:
    return Tget<const IndexArray&,double>(this,index);
  }  
}

const Variant Variant::get(const IndexArrayList& index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for get(index_t)");
  case BoolArray:
    return Tget<const IndexArrayList&,logical>(this,index);
  case Int8Array:
    return Tget<const IndexArrayList&,int8>(this,index);
  case UInt8Array:
    return Tget<const IndexArrayList&,uint8>(this,index);
  case Int16Array:
    return Tget<const IndexArrayList&,int16>(this,index);
  case UInt16Array:
    return Tget<const IndexArrayList&,uint16>(this,index);
  case Int32Array:
    return Tget<const IndexArrayList&,int32>(this,index);
  case UInt32Array:
    return Tget<const IndexArrayList&,uint32>(this,index);
  case Int64Array:
    return Tget<const IndexArrayList&,int64>(this,index);
  case UInt64Array:
    return Tget<const IndexArrayList&,uint64>(this,index);
  case FloatArray:
    return Tget<const IndexArrayList&,float>(this,index);
  case DoubleArray:
    return Tget<const IndexArrayList&,double>(this,index);
  }  
}

const Variant Variant::toType(const Type t) const {
  if (type() == t) return *this;
  switch (t) {
  default:
    throw Exception("Unsupported type for toType");
  case BoolScalar:
    return TcastScalar<bool>(t,this);
  case Int8Scalar:
    return TcastScalar<int8>(t,this);
  case UInt8Scalar:
    return TcastScalar<uint8>(t,this);
  case Int16Scalar:
    return TcastScalar<int16>(t,this);
  case UInt16Scalar:
    return TcastScalar<uint16>(t,this);
  case Int32Scalar:
    return TcastScalar<int32>(t,this);
  case UInt32Scalar:
    return TcastScalar<uint32>(t,this);
  case Int64Scalar:
    return TcastScalar<int64>(t,this);
  case UInt64Scalar:
    return TcastScalar<uint64>(t,this);
  case FloatScalar:
    return TcastScalar<float>(t,this);
  case DoubleScalar:
    return TcastScalar<double>(t,this);
  case BoolArray:
    return Tcast<bool>(t,this);
  case Int8Array:
    return Tcast<int8>(t,this);
  case UInt8Array:
    return Tcast<uint8>(t,this);
  case Int16Array:
    return Tcast<int16>(t,this);
  case UInt16Array:
    return Tcast<uint16>(t,this);
  case Int32Array:
    return Tcast<int32>(t,this);
  case UInt32Array:
    return Tcast<uint32>(t,this);
  case Int64Array:
    return Tcast<int64>(t,this);
  case UInt64Array:
    return Tcast<uint64>(t,this);
  case FloatArray:
    return Tcast<float>(t,this);
  case DoubleArray:
    return Tcast<double>(t,this);
  }
}

const Variant Variant::get(const NTuple& index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for get(index_t)");
  case BoolArray:
    return Variant((logical)Tget_real<const NTuple&,logical>(this,index));
  case Int8Array:
    if (m_complex)
      return Variant((int8)Tget_real<const NTuple&,int8>(this,index),
		     (int8)Tget_imag<const NTuple&,int8>(this,index));
    else
      return Variant((int8)Tget_real<const NTuple&,int8>(this,index));
  case UInt8Array:
    if (m_complex)
      return Variant((uint8)Tget_real<const NTuple&,uint8>(this,index),
		     (uint8)Tget_imag<const NTuple&,uint8>(this,index));
    else
      return Variant((uint8)Tget_real<const NTuple&,uint8>(this,index));
  case Int16Array:
    if (m_complex)
      return Variant((int16)Tget_real<const NTuple&,int16>(this,index),
		     (int16)Tget_imag<const NTuple&,int16>(this,index));
    else
      return Variant((int16)Tget_real<const NTuple&,int16>(this,index));
  case UInt16Array:
    if (m_complex)
      return Variant((uint16)Tget_real<const NTuple&,uint16>(this,index),
		     (uint16)Tget_imag<const NTuple&,uint16>(this,index));
    else
      return Variant((uint16)Tget_real<const NTuple&,uint16>(this,index));
  case Int32Array:
    if (m_complex)
      return Variant((int32)Tget_real<const NTuple&,int32>(this,index),
		     (int32)Tget_imag<const NTuple&,int32>(this,index));
    else
      return Variant((int32)Tget_real<const NTuple&,int32>(this,index));
  case UInt32Array:
    if (m_complex)
      return Variant((uint32)Tget_real<const NTuple&,uint32>(this,index),
		     (uint32)Tget_imag<const NTuple&,uint32>(this,index));
    else
      return Variant((uint32)Tget_real<const NTuple&,uint32>(this,index));
  case Int64Array:
    if (m_complex)
      return Variant((int64)Tget_real<const NTuple&,int64>(this,index),
		     (int64)Tget_imag<const NTuple&,int64>(this,index));
    else
      return Variant((int64)Tget_real<const NTuple&,int64>(this,index));
  case UInt64Array:
    if (m_complex)
      return Variant((uint64)Tget_real<const NTuple&,uint64>(this,index),
		     (uint64)Tget_imag<const NTuple&,uint64>(this,index));
    else
      return Variant((uint64)Tget_real<const NTuple&,uint64>(this,index));
  case FloatArray:
    if (m_complex)
      return Variant((float)Tget_real<const NTuple&,float>(this,index),
		     (float)Tget_imag<const NTuple&,float>(this,index));
    else
      return Variant((float)Tget_real<const NTuple&,float>(this,index));
  case DoubleArray:
    if (m_complex)
      return Variant((double)Tget_real<const NTuple&,double>(this,index),
		     (double)Tget_imag<const NTuple&,double>(this,index));
    else
      return Variant((double)Tget_real<const NTuple&,double>(this,index));
  }
}

const Variant Variant::get(index_t index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for get(index_t)");
  case BoolScalar:
  case Int8Scalar:
  case UInt8Scalar:
  case Int16Scalar:
  case UInt16Scalar:
  case Int32Scalar:
  case UInt32Scalar:
  case Int64Scalar:
  case UInt64Scalar:
  case FloatScalar:
  case DoubleScalar:
    if (index != 1) throw Exception("A(n) exceeds dimensions of A");
    return *this;
  case BoolArray:
    return Variant((logical) Tget_real<index_t,logical>(this,index));
  case Int8Array:
    if (m_complex)
      return Variant((int8)Tget_real<index_t,int8>(this,index),
		     (int8)Tget_imag<index_t,int8>(this,index));
    else
      return Variant((int8)Tget_real<index_t,int8>(this,index));
  case UInt8Array:
    if (m_complex)
      return Variant((uint8)Tget_real<index_t,uint8>(this,index),
		     (uint8)Tget_imag<index_t,uint8>(this,index));
    else
      return Variant((uint8)Tget_real<index_t,uint8>(this,index));
  case Int16Array:
    if (m_complex)
      return Variant((int16)Tget_real<index_t,int16>(this,index),
		     (int16)Tget_imag<index_t,int16>(this,index));
    else
      return Variant((int16)Tget_real<index_t,int16>(this,index));
  case UInt16Array:
    if (m_complex)
      return Variant((uint16)Tget_real<index_t,uint16>(this,index),
		     (uint16)Tget_imag<index_t,uint16>(this,index));
    else
      return Variant((uint16)Tget_real<index_t,uint16>(this,index));
  case Int32Array:
    if (m_complex)
      return Variant((int32)Tget_real<index_t,int32>(this,index),
		     (int32)Tget_imag<index_t,int32>(this,index));
    else
      return Variant((int32)Tget_real<index_t,int32>(this,index));
  case UInt32Array:
    if (m_complex)
      return Variant((uint32)Tget_real<index_t,uint32>(this,index),
		     (uint32)Tget_imag<index_t,uint32>(this,index));
    else
      return Variant((uint32)Tget_real<index_t,uint32>(this,index));
  case Int64Array:
    if (m_complex)
      return Variant((int64)Tget_real<index_t,int64>(this,index),
		     (int64)Tget_imag<index_t,int64>(this,index));
    else
      return Variant((int64)Tget_real<index_t,int64>(this,index));
  case UInt64Array:
    if (m_complex)
      return Variant((uint64)Tget_real<index_t,uint64>(this,index),
		     (uint64)Tget_imag<index_t,uint64>(this,index));
    else
      return Variant((uint64)Tget_real<index_t,uint64>(this,index));
  case FloatArray:
    if (m_complex)
      return Variant((float)Tget_real<index_t,float>(this,index),
		     (float)Tget_imag<index_t,float>(this,index));
    else
      return Variant((float)Tget_real<index_t,float>(this,index));
  case DoubleArray:
    if (m_complex)
      return Variant((double)Tget_real<index_t,double>(this,index),
		     (double)Tget_imag<index_t,double>(this,index));
    else
      return Variant((double)Tget_real<index_t,double>(this,index));
  }
}

const Variant Variant::get(const Variant& index) const {
  if (index.isScalar()) {
    if (!index.allReal())
     Warn("Complex part of index ignored");
    if (index.type() == BoolScalar && !index.realScalar<logical>())
      return Variant(type(),NTuple(0,0));
    return get(index.asIndexScalar());
  }
  else 
    return get(IndexArrayFromVariant(index));
}

const Variant Variant::get(const VariantList& index) const {
  if (AllScalars(index)) {
    NTuple addr(1,1);
    for (int i=0;i<index.size();i++)
      addr[i] = index[i].asIndexScalar();
    return get(addr);
  }
  else {
    IndexArrayList addr;
    for (int i=0;i<index.size();i++)
      addr.push_back(IndexArrayFromVariant(index[i]));
    return get(addr);
  }  
}

void Variant::set(const Variant& index, const Variant& data) {
  if (index.isScalar()) {
    if (!index.allReal())
      Warn("Complex part of index ignored");
    set(index.asIndexScalar(),data);
  }
  else
    set(IndexArrayFromVariant(index),data);
}

void Variant::set(const VariantList& index, const Variant& data) {
  if (AllScalars(index)) {
    NTuple addr(1,1);
    for (int i=0;i<index.size();i++)
      addr[i] = index[i].asIndexScalar();
    set(addr,data);
  }
  else {
    IndexArrayList addr;
    for (int i=0;i<index.size();i++)
      addr[i] = IndexArrayFromVariant(index[i]);
    set(addr,data);
  }
}


const index_t Variant::asIndexScalar() const {
  switch (type()) {
  default:
    throw Exception("Unsupported type called on asIndexScalar");
  case BoolScalar:
    if (m_real.l) return (index_t) 1;
    return (index_t) 0;
  case Int8Scalar:
    return (index_t) m_real.i8;
  case UInt8Scalar:
    return (index_t) m_real.u8;
  case Int16Scalar:
    return (index_t) m_real.i16;
  case UInt16Scalar:
    return (index_t) m_real.u16;
  case Int32Scalar:
    return (index_t) m_real.i32;
  case UInt32Scalar:
    return (index_t) m_real.u32;
  case Int64Scalar:
    return (index_t) m_real.i64;
  case UInt64Scalar:
    return (index_t) m_real.u64;
  case FloatScalar:
    return (index_t) m_real.f;
  case DoubleScalar:
    return m_real.d;
  }
}

const Variant Variant::asScalar() const {
  if ((m_type >= BoolScalar) &&
      (m_type <= DoubleScalar)) return *this;
  if (isArray()) {
    if (dimensions().count() == 1) 
      return get((index_t)1);
    throw Exception("Illegal scalar -- array assignment");
  }
  throw Exception("Unhandled case");
}

#error Need to make this more general.
bool Variant::operator==(const Variant &b) const {
  if (isScalar() && b.isScalar()) {
    Variant a_scalar(asScalar());
    Variant b_scalar(b.asScalar());
    if (a_scalar.type() != b_scalar.type()) return false;
    switch (a_scalar.type()) {
    default:
      throw Exception("Unhandled scalar case");
    case BoolScalar:
      return ((a_scalar.realScalar<logical>() == b_scalar.realScalar<logical>()));
    case Int8Scalar:
      return ((a_scalar.realScalar<int8>() == b_scalar.realScalar<int8>()) &&
	      (a_scalar.imagScalar<int8>() == b_scalar.imagScalar<int8>()));
    case UInt8Scalar:
      return ((a_scalar.realScalar<uint8>() == b_scalar.realScalar<uint8>()) &&
	      (a_scalar.imagScalar<uint8>() == b_scalar.imagScalar<uint8>()));
    case Int16Scalar:
      return ((a_scalar.realScalar<int16>() == b_scalar.realScalar<int16>()) &&
	      (a_scalar.imagScalar<int16>() == b_scalar.imagScalar<int16>()));
    case UInt16Scalar:
      return ((a_scalar.realScalar<uint16>() == b_scalar.realScalar<uint16>()) &&
	      (a_scalar.imagScalar<uint16>() == b_scalar.imagScalar<uint16>()));
    case Int32Scalar:
      return ((a_scalar.realScalar<int32>() == b_scalar.realScalar<int32>()) &&
	      (a_scalar.imagScalar<int32>() == b_scalar.imagScalar<int32>()));
    case UInt32Scalar:
      return ((a_scalar.realScalar<uint32>() == b_scalar.realScalar<uint32>()) &&
	      (a_scalar.imagScalar<uint32>() == b_scalar.imagScalar<uint32>()));
    case Int64Scalar:
      return ((a_scalar.realScalar<int64>() == b_scalar.realScalar<int64>()) &&
	      (a_scalar.imagScalar<int64>() == b_scalar.imagScalar<int64>()));
    case UInt64Scalar:
      return ((a_scalar.realScalar<uint64>() == b_scalar.realScalar<uint64>()) &&
	      (a_scalar.imagScalar<uint64>() == b_scalar.imagScalar<uint64>()));
    case FloatScalar:
      return ((a_scalar.realScalar<float>() == b_scalar.realScalar<float>()) &&
	      (a_scalar.imagScalar<float>() == b_scalar.imagScalar<float>()));
    case DoubleScalar:
      return ((a_scalar.realScalar<double>() == b_scalar.realScalar<double>()) &&
	      (a_scalar.imagScalar<double>() == b_scalar.imagScalar<double>()));
    }
    return false;
  }
  if (!(dimensions() == b.dimensions())) return false;
  if (type() != b.type()) return false;
  if (allReal() ^ b.allReal()) return false;
  switch (type()) {
  default:
    throw Exception("Unhandled case as argument to == operator for Variant class");
  case BoolArray:
    return constReal<logical>() == b.constReal<logical>();
  case Int8Array:
    if (allReal())
      return constReal<int8>() == b.constReal<int8>();
    else
      return ((constReal<int8>() == b.constReal<int8>()) &&
	      (constImag<int8>() == b.constImag<int8>()));
  case UInt8Array:
    if (allReal())
      return constReal<uint8>() == b.constReal<uint8>();
    else
      return ((constReal<uint8>() == b.constReal<uint8>()) &&
	      (constImag<uint8>() == b.constImag<uint8>()));
  case Int16Array:
    if (allReal())
      return constReal<int16>() == b.constReal<int16>();
    else
      return ((constReal<int16>() == b.constReal<int16>()) &&
	      (constImag<int16>() == b.constImag<int16>()));
  case UInt16Array:
    if (allReal())
      return constReal<uint16>() == b.constReal<uint16>();
    else
      return ((constReal<uint16>() == b.constReal<uint16>()) &&
	      (constImag<uint16>() == b.constImag<uint16>()));
  case Int32Array:
    if (allReal())
      return constReal<int32>() == b.constReal<int32>();
    else
      return ((constReal<int32>() == b.constReal<int32>()) &&
	      (constImag<int32>() == b.constImag<int32>()));
  case UInt32Array:
    if (allReal())
      return constReal<uint32>() == b.constReal<uint32>();
    else
      return ((constReal<uint32>() == b.constReal<uint32>()) &&
	      (constImag<uint32>() == b.constImag<uint32>()));
  case Int64Array:
    if (allReal())
      return constReal<int64>() == b.constReal<int64>();
    else
      return ((constReal<int64>() == b.constReal<int64>()) &&
	      (constImag<int64>() == b.constImag<int64>()));
  case UInt64Array:
    if (allReal())
      return constReal<uint64>() == b.constReal<uint64>();
    else
      return ((constReal<uint64>() == b.constReal<uint64>()) &&
	      (constImag<uint64>() == b.constImag<uint64>()));
  case FloatArray:
    if (allReal())
      return constReal<float>() == b.constReal<float>();
    else
      return ((constReal<float>() == b.constReal<float>()) &&
	      (constImag<float>() == b.constImag<float>()));
  case DoubleArray:
    if (allReal())
      return constReal<double>() == b.constReal<double>();
    else
      return ((constReal<double>() == b.constReal<double>()) &&
	      (constImag<double>() == b.constImag<double>()));
  }
}

// void Variant::set(index_t pos, const Variant& val) {
//   switch (m_type) {
//   default:
//     throw Exception("Unsupported type for set");
//   case BoolArray:  {
//     Variant valp(val.asScalar());
//     Tset<logical>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case Int8Array:  {
//     Variant valp(val.asScalar());
//     Tset<int8>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case UInt8Array:  {
//     Variant valp(val.asScalar());
//     Tset<uint8>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case Int16Array:  {
//     Variant valp(val.asScalar());
//     Tset<int16>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case UInt16Array:  {
//     Variant valp(val.asScalar());
//     Tset<uint16>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case Int32Array:   {
//     Variant valp(val.asScalar());
//     Tset<int32>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case UInt32Array:    {
//     Variant valp(val.asScalar());
//     Tset<uint32>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case Int64Array:   {
//     Variant valp(val.asScalar());
//     Tset<int64>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case UInt64Array:    {
//     Variant valp(val.asScalar());
//     Tset<uint64>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case FloatArray:    {
//     Variant valp(val.asScalar());
//     Tset<float>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   case DoubleArray:    {
//     Variant valp(val.asScalar());
//     Tset<double>(this,pos,valp.m_real.d,valp.m_imag.d);
//     return;
//   }
//   }
// }

std::ostream& operator<<(std::ostream& o, const Variant &t) {
  t.print(o);
  return o;
}
  
bool IsColonOp(const Variant &x) {
  return (x.type() == String) && (x.string() == ":");
}

QString Variant::string() const {
  if (m_type != String) throw Exception("Cannot convert array to string");
  const BasicArray<uint16> &p(constReal<uint16>());
  QString ret;
  for (int i=0;i<p.length();i++)
    ret += QChar(p[i+1]);
  return ret;
}

const IndexArray IndexArrayFromVariant(const Variant &index) {
  if (!index.allReal())
    Warn("Complex part of index ignored");
  if (index.type() == DoubleArray)
    return index.constReal<index_t>();
  if (index.type() == BoolArray)
    return Find(index.constReal<logical>());
  Variant index_converted(index.toType(DoubleArray));
  if (!index_converted.allReal())
    Warn("Complex part of index ignored");
  return index_converted.constReal<index_t>();
  if (IsColonOp(index))
    return IndexArray(-1);
  throw Exception("Unsupported index type");
}
