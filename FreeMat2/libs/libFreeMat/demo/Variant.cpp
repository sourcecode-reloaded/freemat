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

template <typename T>
static inline double Tget_real(const Variant*ptr, index_t ndx) {
  const BasicArray<T>& real(ptr->constReal<T>());
  return real.get(ndx);
}

template <typename T>
static inline double Tget_imag(const Variant*ptr, index_t ndx) {
  if (ptr->allReal()) return T();
  const BasicArray<T>& imag(ptr->constImag<T>());
  return imag.get(ndx);
}

template <typename T>
static inline const Variant Tget_vector(const Variant*ptr, const IndexArray &ndx) {
  if (ptr->allReal()) {
    const BasicArray<T> &real(ptr->constReal<T>());
    return Variant(ptr->type(),real.getVectorSubset(ndx));
  } else {
    const BasicArray<T> &real(ptr->constReal<T>());
    const BasicArray<T> &imag(ptr->constImag<T>());
    return Variant(ptr->type(),real.getVectorSubset(ndx),imag.getVectorSubset(ndx));
  }
}

template <typename T>
static void Tset(Variant *ptr, index_t pos, T vreal, T vimag) {
  BasicArray<T>& real(ptr->real<T>());
  real.set(pos,vreal);
  if (vimag != 0) {
    BasicArray<T>& imag(ptr->imag<T>());
    imag.set(pos,vimag);
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
    o << "(" << m_real.u8 << "," << m_imag.u8 << ")"; 
    return;
  case UInt16Scalar:
    o << "(" << m_real.u16 << "," << m_imag.u16 << ")";
    return;
  case UInt32Scalar:
    o << "(" << m_real.u32 << "," << m_imag.u32 << ")";
    return;
  case UInt64Scalar:
    o << "(" << m_real.u64 << "," << m_imag.u64 << ")";
    return;
  case Int8Scalar:
    o << "(" << m_real.i8 << "," << m_imag.i8 << ")";
    return;
  case Int16Scalar:
    o << "(" << m_real.i16 << "," << m_imag.i16 << ")";
    return;
  case Int32Scalar:
    o << "(" << m_real.i32 << "," << m_imag.i32 << ")";
    return;
  case Int64Scalar:
    o << "(" << m_real.i64 << "," << m_imag.i64 << ")";
    return;
  case FloatScalar:
    o << "(" << m_real.f << "," << m_imag.f << ")";
    return;
  case DoubleScalar:
    o << "(" << m_real.d << "," << m_imag.d << ")";
    return;
  }
}

Variant::Variant(Type t, const NTuple &dims) {
  m_type = t;
  m_real.p = new SharedObject(t,construct_sized(t,dims));
}

Variant::Variant(const QString &text) {
  m_type = String;
  m_real.p = new SharedObject(String,
			      construct_sized(String,
					     NTuple(1,text.size())));
  BasicArray<uint16> &p(real<uint16>());
  for (int i=0;i<text.size();i++) 
    p[i+1] = text[i].unicode();
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

const Variant Variant::getVectorSubset(const IndexArray& index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for getVectorSubset(index_t)");
  case BoolArray:
    return Tget_vector<logical>(this,index);
  case Int8Array:
    return Tget_vector<int8>(this,index);
  case UInt8Array:
    return Tget_vector<uint8>(this,index);
  case Int16Array:
    return Tget_vector<int16>(this,index);
  case UInt16Array:
    return Tget_vector<uint16>(this,index);
  case Int32Array:
    return Tget_vector<int32>(this,index);
  case UInt32Array:
    return Tget_vector<uint32>(this,index);
  case Int64Array:
    return Tget_vector<int64>(this,index);
  case UInt64Array:
    return Tget_vector<uint64>(this,index);
  case FloatArray:
    return Tget_vector<float>(this,index);
  case DoubleArray:
    return Tget_vector<double>(this,index);
  }  
}

const Variant Variant::toType(const Type t) const {
  switch (t) {
  default:
    throw Exception("Unsupported type for toType");
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

const Variant Variant::getVectorSubset(index_t index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for getVectorSubset(index_t)");
  case BoolArray:
    return Variant(Tget_real<logical>(this,index),Tget_imag<logical>(this,index));
  case Int8Array:
    return Variant(Tget_real<int8>(this,index),Tget_imag<int8>(this,index));
  case UInt8Array:
    return Variant(Tget_real<uint8>(this,index),Tget_imag<uint8>(this,index));
  case Int16Array:
    return Variant(Tget_real<int16>(this,index),Tget_imag<int16>(this,index));
  case UInt16Array:
    return Variant(Tget_real<uint16>(this,index),Tget_imag<uint16>(this,index));
  case Int32Array:
    return Variant(Tget_real<int32>(this,index),Tget_imag<int32>(this,index));
  case UInt32Array:
    return Variant(Tget_real<uint32>(this,index),Tget_imag<uint32>(this,index));
  case Int64Array:
    return Variant(Tget_real<int64>(this,index),Tget_imag<int64>(this,index));
  case UInt64Array:
    return Variant(Tget_real<uint64>(this,index),Tget_imag<uint64>(this,index));
  case FloatArray:
    return Variant(Tget_real<float>(this,index),Tget_imag<float>(this,index));
  case DoubleArray:
    return Variant(Tget_real<double>(this,index),Tget_imag<double>(this,index));
  }
}

const Variant Variant::getVectorSubset(const Variant& index) const {
  if (index.isScalar()) {
    if (!index.allReal())
      Warn("Complex part of index ignored");
    return getVectorSubset(index.asIndexScalar());
  }
  else 
    return getVectorSubset(IndexArrayFromVariant(index));
}

const index_t Variant::asIndexScalar() const {
  switch (type()) {
  default:
    throw Exception("Unsupported type called on asIndexScalar");
  case BoolScalar:
    return (index_t) m_real.l;
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
  if (isScalar()) return *this;
  if (isArray()) {
    if (dimensions().count() == 1) 
      return getVectorSubset((index_t)1);
    throw Exception("Illegal scalar -- array assignment");
  }
  throw Exception("Unhandled case");
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
  Variant index_converted(index.toType(DoubleArray));
  if (!index_converted.allReal())
    Warn("Complex part of index ignored");
  return index_converted.constReal<index_t>();
  throw Exception("Unsupported index type");
}
