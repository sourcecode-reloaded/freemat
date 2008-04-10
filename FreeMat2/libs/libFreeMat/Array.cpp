#include "Array.hpp"
#include "GetSet.hpp"
#include <QStringList>

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
static inline T Tget_real(const Array*ptr, S ndx) {
  const BasicArray<T>& real(ptr->constReal<T>());
  return real.get(ndx);
}

template <typename S, typename T>
static inline T Tget_imag(const Array*ptr, S ndx) {
  if (ptr->allReal()) return T();
  const BasicArray<T>& imag(ptr->constImag<T>());
  return imag.get(ndx);
}

template <typename S>
static inline Array Tget_struct(const Array*ptr, S ndx) {
  const StructArray &rp(ptr->constStructPtr());
  Array ret(Struct,NTuple(1,1));
  StructArray &lp(ret.structPtr());
  StructArray::const_iterator i=rp.constBegin();
  while (i != rp.constEnd()) {
    lp[i.key()] = Get(i.value(),ndx);
    ++i;
  }
  return ret;
}

template <typename S>
static inline Array Tget_struct_scalar(const Array*ptr, S ndx) {
  const StructArray &rp(ptr->constStructPtr());
  Array ret(Struct,NTuple(1,1));
  StructArray &lp(ret.structPtr());
  StructArray::const_iterator i=rp.constBegin();
  while (i != rp.constEnd()) {
    lp[i.key()].set(1,Get(i.value(),ndx).get(1));
    ++i;
  }
  return ret;
}

template <typename S>
static inline const void Tset_struct(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.type() != Struct)
    throw Exception("Assignment A(I)=B where A is a structure array implies that B is also a structure array.");
  // First loop through the elements
  const StructArray &rp(rhs.constStructPtr());
  StructArray &lp(ptr->structPtr());
  StructArray::const_iterator i=rp.constBegin();
  NTuple newSize(ptr->dimensions());
  while (i!=rp.constEnd()) {
    Set(lp[i.key()],ndx,i.value());
    newSize = lp[i.key()].dimensions();
    ++i;
  }
  // Loop through the output and force all arrays to be the same size
  StructArray::iterator j=lp.begin();
  while (j != lp.end()) {
    j.value().resize(newSize);
    ++j;
  }
}

template <typename S>
static inline const void Tset_struct_scalar_rhs(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.type() != Struct)
    throw Exception("Assignment A(I)=B where A is a structure array implies that B is also a structure array.");
  // First loop through the elements
  const StructArray &rp(rhs.constStructPtr());
  StructArray &lp(ptr->structPtr());
  StructArray::const_iterator i=rp.constBegin();
  while (i!=rp.constEnd()) {
    Set(lp[i.key()],ndx,i.value().get(1));
    ++i;
  }
  // Loop through the output and force all arrays to be the same size
  NTuple newSize;
  i = lp.constBegin();
  while (i!= lp.constEnd()) {
    newSize.cover(i.value().dimensions());
    ++i;
  }
  StructArray::iterator j=lp.begin();
  while (j != lp.end()) {
    j.value().resize(newSize);
    ++j;
  }
}

template <typename S, typename T>
static inline const Array Tget(const Array*ptr, S ndx) {
  if (ptr->allReal()) {
    const BasicArray<T> &real(ptr->constReal<T>());
    return Array(ptr->type(),Get(real,ndx));
  } else {
    const BasicArray<T> &real(ptr->constReal<T>());
    const BasicArray<T> &imag(ptr->constImag<T>());
    return Array(ptr->type(),Get(real,ndx),Get(imag,ndx));
  }
}

template <typename S>
static inline const Array Tget_sparse(const Array*ptr, S ndx) {
  if (ptr->allReal())
    return Array(Get(ptr->constRealSparse(),ndx));
  else
    return Array(Get(ptr->constRealSparse(),ndx),
		   Get(ptr->constImagSparse(),ndx));
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
  case DoubleSparse:
    return DoubleScalar;
  }
}

template <typename S, typename T>
static inline void Tset_scalar_rhs(Array* ptr, S ndx, const Array& data) {
  BasicArray<T> &real(ptr->real<T>());
  Array dataTyped(data.asScalar().toType(ScalarType(ptr->type())));
  real.set(ndx,dataTyped.constRealScalar<T>());
  if (!dataTyped.allReal()) {
    BasicArray<T> &imag(ptr->imag<T>());
    imag.set(ndx,dataTyped.constImagScalar<T>());
  }
}

template <typename S>
static inline void Tset_sparse_scalar_rhs(Array*ptr, S ndx, const Array &rhs) {
  Array dataTyped(rhs.asScalar().toType(ScalarType(ptr->type())));
  ptr->realSparse().set(ndx,dataTyped.constRealScalar<double>());
  if (!dataTyped.allReal()) 
    ptr->imagSparse().set(ndx,dataTyped.constImagScalar<double>());
}

template <typename S>
static inline void Tresize_struct(Array* ptr, S ndx) {
  StructArray &lp(ptr->structPtr());
  StructArray::iterator i=lp.begin();
  while (i != lp.end()) {
    i.value().resize(ndx);
    ++i;
  }
}

template <typename S, typename T>
static inline void Tresize(Array* ptr, S ndx) {
  BasicArray<T> &real(ptr->real<T>());
  real.resize(ndx);
  if (!ptr->allReal()) {
    BasicArray<T> &imag(ptr->imag<T>());
    imag.resize(ndx);
  }
}

template <typename S>
static inline void Tresize_sparse(Array* ptr, S ndx) {
  ptr->realSparse().resize(ndx);
  if (!ptr->allReal()) 
    ptr->imagSparse().resize(ndx);
}


template <typename S>
static inline void Treshape_struct(Array* ptr, S ndx) {
  StructArray &lp(ptr->structPtr());
  StructArray::iterator i=lp.begin();
  while (i != lp.end()) {
    i.value().reshape(ndx);
    ++i;
  }
}

template <typename S>
static inline void Treshape_sparse(Array* ptr, S ndx) {
  ptr->realSparse().reshape(ndx);
  if (!ptr->allReal()) 
    ptr->imagSparse().reshape(ndx);
}

template <typename S, typename T>
static inline void Treshape(Array* ptr, S ndx) {
  BasicArray<T> &real(ptr->real<T>());
  real.reshape(ndx);
  if (!ptr->allReal()) {
    BasicArray<T> &imag(ptr->imag<T>());
    imag.reshape(ndx);
  }
}

template <typename S>
static inline void Tset_sparse(Array* ptr, S ndx, const Array& data) {
  if (data.type() >= BoolScalar && data.type() <= DoubleScalar) {
    Tset_sparse<S>(ptr,ndx,data.asArrayType());
    return;
  }
  Set(ptr->realSparse(),ndx,ToRealSparse(data));
  if (!data.allReal())
    Set(ptr->imagSparse(),ndx,ToImagSparse(data));
}

template <typename S, typename T>
static inline void Tset(Array* ptr, S ndx, const Array& data) {
  if (data.type() >= BoolScalar && data.type() <= DoubleScalar) {
    Tset<S,T>(ptr,ndx,data.asArrayType());
    return;
  }
  BasicArray<T> &real(ptr->real<T>());
  Array dataTyped(data.toType(ptr->type()));
  Set(real,ndx,dataTyped.constReal<T>());
  if (!data.allReal()) {
    BasicArray<T> &imag(ptr->imag<T>());
    Set(imag,ndx,dataTyped.constImag<T>());
  }
}


// S - source type, T - dest type
template <typename S, typename T>
inline static const Array Tcast_case(Type t, const Array *copy) {
  Array retvec(t,copy->dimensions());
  if (copy->allReal()) {
    ConvertBasicArray(copy->constReal<S>(),retvec.real<T>());
  } else {
    ConvertBasicArray(copy->constReal<S>(),retvec.real<T>());
    ConvertBasicArray(copy->constImag<S>(),retvec.imag<T>());
  }
  return retvec;
}


template <typename T>
inline static const Array TcastScalar(Type t, const Array *copy) {
  if (copy->type() == t) return *copy;
  switch (copy->type()) {
  default:
    throw Exception("Unsupported type for conversion");
  case BoolScalar:
    return Array((T)copy->constRealScalar<bool>());
  case Int8Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<int8>(),(T)copy->constImagScalar<int8>());
    else
      return Array((T)copy->constRealScalar<int8>());
  case UInt8Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<uint8>(),(T)copy->constImagScalar<uint8>());
    else
      return Array((T)copy->constRealScalar<uint8>());
  case Int16Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<int16>(),(T)copy->constImagScalar<int16>());
    else
      return Array((T)copy->constRealScalar<int16>());
  case UInt16Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<uint16>(),(T)copy->constImagScalar<uint16>());
    else
      return Array((T)copy->constRealScalar<uint16>());
  case Int32Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<int32>(),(T)copy->constImagScalar<int32>());
    else
      return Array((T)copy->constRealScalar<int32>());
  case UInt32Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<uint32>(),(T)copy->constImagScalar<uint32>());
    else
      return Array((T)copy->constRealScalar<uint32>());
  case Int64Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<int64>(),(T)copy->constImagScalar<int64>());
    else
      return Array((T)copy->constRealScalar<int64>());
  case UInt64Scalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<uint64>(),(T)copy->constImagScalar<uint64>());
    else
      return Array((T)copy->constRealScalar<uint64>());
  case FloatScalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<float>(),(T)copy->constImagScalar<float>());
    else
      return Array((T)copy->constRealScalar<float>());
  case DoubleScalar:
    if (!copy->allReal())
      return Array((T)copy->constRealScalar<double>(),(T)copy->constImagScalar<double>());
    else
      return Array((T)copy->constRealScalar<double>());
  }
}

// T - dest type
template <typename T>
inline static const Array Tcast(Type t, const Array *copy) {
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
  case StringArray:
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
  case CellArray:
    return Tconstruct<BasicArray<Array> >(copy);
  case Struct:
    return Tconstruct<StructArray>(copy);
  case BoolArray:
    return Tconstruct<BasicArray<logical> >(copy);
  case Int8Array:
    return Tconstruct<BasicArray<int8> >(copy);
  case UInt8Array:
    return Tconstruct<BasicArray<uint8> >(copy);
  case Int16Array:
    return Tconstruct<BasicArray<int16> >(copy);
  case StringArray:
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
  case DoubleSparse:
    return Tconstruct<SparseMatrix>(copy);
  }
}

static inline void* construct_sized(Type t, const NTuple &dims) {
  switch (t) {
  default:
    throw Exception("Unsupported constructor");
  case CellArray:
    return Tconstruct_sized<BasicArray<Array> >(dims);
  case Struct:
    return reinterpret_cast<void*>(new StructArray);
  case BoolArray:
    return Tconstruct_sized<BasicArray<logical> >(dims);
  case Int8Array:
    return Tconstruct_sized<BasicArray<int8> >(dims);
  case UInt8Array:
    return Tconstruct_sized<BasicArray<uint8> >(dims);
  case Int16Array:
    return Tconstruct_sized<BasicArray<int16> >(dims);
  case StringArray:
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
  case DoubleSparse:
    return reinterpret_cast<void*>(new SparseMatrix(dims));
  }
}

static inline void destruct(Type t, void *todelete) {
  switch (t) {
  default:
    throw Exception("Unsupported construct");
  case CellArray:
    return Tdestruct<BasicArray<Array> >(todelete);
  case Struct:
    return Tdestruct<StructArray>(todelete);
  case BoolArray:
    return Tdestruct<BasicArray<logical> >(todelete);
  case Int8Array:
    return Tdestruct<BasicArray<int8> >(todelete);
  case UInt8Array:
    return Tdestruct<BasicArray<uint8> >(todelete);
  case Int16Array:
    return Tdestruct<BasicArray<int16> >(todelete);
  case StringArray:
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
  case DoubleSparse:
    return Tdestruct<SparseMatrix>(todelete);
  }
}

static inline bool AllNonBoolScalars(const ArrayVector& index) {
  for (int i=0;i<index.size();i++)
    if (!index[i].isScalar() || (index[i].type() == BoolScalar)) return false;
  return true;
}

SharedObject::SharedObject(Type t, void* p) : m_type(t), m_p(p) {}

SharedObject::SharedObject(const SharedObject& copy) : 
  QSharedData(copy), m_type(copy.m_type) {
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

void Array::print(std::ostream& o) const {
  switch (m_type) {
  default:
    throw Exception("unsupported type");
  case Invalid:
    o << "[] ";
    return;
  case CellArray:
    o << constReal<Array>();
    return;
  case Struct:
    {
      const StructArray &rp(constStructPtr());
      for (StructArray::const_iterator i=rp.constBegin();i!=rp.constEnd();++i) {
	o << "field: " << i.key() << "\n";
	o << "value: " << i.value();
      }
    }
    return;
  case UInt8Scalar:
    if (m_complex)
      o << "(" << (int) m_real.u8 << "," << (int) m_imag.u8 << ")"; 
    else
      o << (int) m_real.u8;
    return;
  case StringArray:
    {
      QString val(string());
      o << val.toStdString().c_str();
      return;
    }
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
  case UInt8Array:
    if (m_complex)
      o << "real\n" << constReal<uint8>() 
	<< "complex\n" << constImag<uint8>();
    else
      o << "real\n" << constReal<uint8>();
    return;
  case UInt16Array:
    if (m_complex)
      o << "real\n" << constReal<uint16>() 
	<< "complex\n" << constImag<uint16>();
    else
      o << "real\n" << constReal<uint16>();
    return;
  case UInt32Array:
    if (m_complex)
      o << "real\n" << constReal<uint32>() 
	<< "complex\n" << constImag<uint32>();
    else
      o << "real\n" << constReal<uint32>();
    return;
  case UInt64Array:
    if (m_complex)
      o << "real\n" << constReal<uint64>() 
	<< "complex\n" << constImag<uint64>();
    else
      o << "real\n" << constReal<uint64>();
    return;
  case Int8Array:
    if (m_complex)
      o << "real\n" << constReal<int8>() 
	<< "complex\n" << constImag<int8>();
    else
      o << "real\n" << constReal<int8>();
    return;
  case Int16Array:
    if (m_complex)
      o << "real\n" << constReal<int16>() 
	<< "complex\n" << constImag<int16>();
    else
      o << "real\n" << constReal<int16>();
    return;
  case Int32Array:
    if (m_complex)
      o << "real\n" << constReal<int32>() 
	<< "complex\n" << constImag<int32>();
    else
      o << "real\n" << constReal<int32>();
    return;
  case Int64Array:
    if (m_complex)
      o << "real\n" << constReal<int64>() 
	<< "complex\n" << constImag<int64>();
    else
      o << "real\n" << constReal<int64>();
    return;
  case FloatArray:
    if (m_complex)
      o << "real\n" << constReal<float>() 
	<< "complex\n" << constImag<float>();
    else
      o << "real\n" << constReal<float>();
    return;
  case DoubleArray:
    if (m_complex)
      o << "real\n" << constReal<double>() 
	<< "complex\n" << constImag<double>();
    else
      o << "real\n" << constReal<double>();
    return;
  case DoubleSparse:
    if (m_complex)
      o << "real\n" << constRealSparse()
	<< "complex\n" << constImagSparse();
    else
      o << "real\n" << constRealSparse();
  }
}

Array::Array(Type t, const NTuple &dims) {
  m_type = t;
  m_real.p = new SharedObject(t,construct_sized(t,dims));
  m_complex = false;
}

Array::Array(const SparseMatrix& real) {
  m_type = DoubleSparse;
  m_real.p = new SharedObject(DoubleSparse,new SparseMatrix(real));
  m_complex = false;
}

Array::Array(const SparseMatrix& real, 
		 const SparseMatrix& imag) {
  m_type = DoubleSparse;
  m_real.p = new SharedObject(DoubleSparse,new SparseMatrix(real));
  m_imag.p = new SharedObject(DoubleSparse,new SparseMatrix(imag));
  m_complex = true;
}

Array::Array(const QString &text) {
  m_type = StringArray;
  m_real.p = new SharedObject(StringArray,
			      construct_sized(StringArray,
					     NTuple(1,text.size())));
  BasicArray<uint16> &p(real<uint16>());
  for (int i=0;i<text.size();i++) 
    p[i+1] = text[i].unicode();
  m_complex = false;
}

const NTuple Array::dimensions() const {
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
  case CellArray:
    return (constReal<Array>().dimensions());
  case Struct:
    {
      const StructArray &rp(constStructPtr());
      StructArray::const_iterator i=rp.constBegin();
      if (i == rp.constEnd()) return NTuple(0,0);
      return i.value().dimensions();
    }
  case BoolArray:
    return (constReal<logical>().dimensions());
  case Int8Array:
    return (constReal<int8>().dimensions());
  case UInt8Array:
    return (constReal<uint8>().dimensions());
  case Int16Array:
    return (constReal<int16>().dimensions());
  case StringArray:
  case UInt16Array:
    return (constReal<uint16>().dimensions());
  case Int32Array:
    return (constReal<int32>().dimensions());
  case UInt32Array:
    return (constReal<uint32>().dimensions());
  case Int64Array:
    return (constReal<int64>().dimensions());
  case UInt64Array:
    return (constReal<uint64>().dimensions());
  case FloatArray:
    return (constReal<float>().dimensions());
  case DoubleArray:
    return (constReal<double>().dimensions());
  case DoubleSparse:
    return (constRealSparse().dimensions());
  };
}

void Array::set(index_t index, const Array& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(index_t)");
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
    *this = asArrayType();
    set(index,data);
    return;
  case CellArray:
    Set(real<Array>(),index,data);
    return;
  case Struct:
    Tset_struct_scalar_rhs<index_t>(this,index,data);
    return;
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
  case StringArray:
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
  case DoubleSparse:
    Tset_sparse_scalar_rhs<index_t>(this,index,data);
    return;
  }
}

void Array::set(const NTuple& index, const Array& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(const NTuple&)"); 
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
    *this = asArrayType();
    set(index,data);
    return;
  case CellArray:
    if (data.type () != CellArray) 
      throw Exception("Assignment A(I)=B where A is a cell array implies that B is also a cell array.");
    Set(real<Array>(),index,data.get((index_t)1));
    return;
  case Struct:
    Tset_struct_scalar_rhs<const NTuple&>(this,index,data);
    return;
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
  case StringArray:
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
  case DoubleSparse:
    Tset_sparse_scalar_rhs<const NTuple&>(this,index,data);
    return;
  }  
}

void Array::set(const IndexArray& index, const Array& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(IndexArray)");
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
    *this = asArrayType();
    set(index,data);
    return;
  case CellArray:
    if (data.type () != CellArray) 
      throw Exception("Assignment A(I)=B where A is a cell array implies that B is also a cell array.");
    Set(real<Array>(),index,data.get((index_t)1));
    return;
  case Struct:
    Tset_struct<const IndexArray&>(this,index,data);
    return;
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
  case StringArray:
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
  case DoubleSparse:
    Tset_sparse<const IndexArray&>(this,index,data);
    return;
  }  
}

void Array::set(const IndexArrayVector& index, const Array& data) {
  switch (m_type) {
  default:
    throw Exception("Unsupported type for set(IndexArray)");
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
    *this = asArrayType();
    set(index,data);
    return;
  case CellArray:
    if (data.type () != CellArray) 
      throw Exception("Assignment A(I)=B where A is a cell array implies that B is also a cell array.");
    Set(real<Array>(),index,data.get((index_t)1));
    return;
  case Struct:
    Tset_struct<const IndexArrayVector&>(this,index,data);
    return;
  case BoolArray:
    Tset<const IndexArrayVector&,logical>(this,index,data);
    return;
  case Int8Array:
    Tset<const IndexArrayVector&,int8>(this,index,data);
    return;
  case UInt8Array:
    Tset<const IndexArrayVector&,uint8>(this,index,data);
    return;
  case Int16Array:
    Tset<const IndexArrayVector&,int16>(this,index,data);
    return;
  case StringArray:
  case UInt16Array:
    Tset<const IndexArrayVector&,uint16>(this,index,data);
    return;
  case Int32Array:
    Tset<const IndexArrayVector&,int32>(this,index,data);
    return;
  case UInt32Array:
    Tset<const IndexArrayVector&,uint32>(this,index,data);
    return;
  case Int64Array:
    Tset<const IndexArrayVector&,int64>(this,index,data);
    return;
  case UInt64Array:
    Tset<const IndexArrayVector&,uint64>(this,index,data);
    return;
  case FloatArray:
    Tset<const IndexArrayVector&,float>(this,index,data);
    return;
  case DoubleArray:
    Tset<const IndexArrayVector&,double>(this,index,data);
    return;
  case DoubleSparse:
    Tset_sparse<const IndexArrayVector&>(this,index,data);
    return;
  }  
}

void Array::reshape(const NTuple &size) {
  switch (m_type) {
  default:
    throw Exception("Unhandled case for reshape");
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
    *this = asArrayType();
    reshape(size);
    return;
  case CellArray:
    return Treshape<const NTuple&,Array>(this,size);
  case Struct:
    return Treshape_struct<const NTuple&>(this,size);
  case BoolArray:
    return Treshape<const NTuple&,logical>(this,size);
  case Int8Array:
    return Treshape<const NTuple&,int8>(this,size);
  case UInt8Array:
    return Treshape<const NTuple&,uint8>(this,size);
  case Int16Array:
    return Treshape<const NTuple&,int16>(this,size);
  case StringArray:
  case UInt16Array:
    return Treshape<const NTuple&,uint16>(this,size);
  case Int32Array:
    return Treshape<const NTuple&,int32>(this,size);
  case UInt32Array:
    return Treshape<const NTuple&,uint32>(this,size);
  case Int64Array:
    return Treshape<const NTuple&,int64>(this,size);
  case UInt64Array:
    return Treshape<const NTuple&,uint64>(this,size);
  case FloatArray:
    return Treshape<const NTuple&,float>(this,size);
  case DoubleArray:
    return Treshape<const NTuple&,double>(this,size);
  case DoubleSparse:
    return Treshape_sparse<const NTuple&>(this,size);
  }  
}

void Array::resize(const NTuple &size) {
  switch (m_type) {
  default:
    throw Exception("Unhandled case for resize");
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
    *this = asArrayType();
    resize(size);
    return;
  case CellArray:
    return Tresize<const NTuple&,Array>(this,size);
  case Struct:
    return Tresize_struct<const NTuple&>(this,size);
  case BoolArray:
    return Tresize<const NTuple&,logical>(this,size);
  case Int8Array:
    return Tresize<const NTuple&,int8>(this,size);
  case UInt8Array:
    return Tresize<const NTuple&,uint8>(this,size);
  case Int16Array:
    return Tresize<const NTuple&,int16>(this,size);
  case StringArray:
  case UInt16Array:
    return Tresize<const NTuple&,uint16>(this,size);
  case Int32Array:
    return Tresize<const NTuple&,int32>(this,size);
  case UInt32Array:
    return Tresize<const NTuple&,uint32>(this,size);
  case Int64Array:
    return Tresize<const NTuple&,int64>(this,size);
  case UInt64Array:
    return Tresize<const NTuple&,uint64>(this,size);
  case FloatArray:
    return Tresize<const NTuple&,float>(this,size);
  case DoubleArray:
    return Tresize<const NTuple&,double>(this,size);
  case DoubleSparse:
    return Tresize_sparse<const NTuple&>(this,size);
  }  
}

void Array::resize(index_t size) {
  switch (m_type) {
  default:
    throw Exception("Unhandled case for resize");
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
    *this = asArrayType();
    resize(size);
    return;
  case CellArray:
    return Tresize<index_t,Array>(this,size);
  case Struct:
    return Tresize_struct<index_t>(this,size);
  case BoolArray:
    return Tresize<index_t,logical>(this,size);
  case Int8Array:
    return Tresize<index_t,int8>(this,size);
  case UInt8Array:
    return Tresize<index_t,uint8>(this,size);
  case Int16Array:
    return Tresize<index_t,int16>(this,size);
  case StringArray:
  case UInt16Array:
    return Tresize<index_t,uint16>(this,size);
  case Int32Array:
    return Tresize<index_t,int32>(this,size);
  case UInt32Array:
    return Tresize<index_t,uint32>(this,size);
  case Int64Array:
    return Tresize<index_t,int64>(this,size);
  case UInt64Array:
    return Tresize<index_t,uint64>(this,size);
  case FloatArray:
    return Tresize<index_t,float>(this,size);
  case DoubleArray:
    return Tresize<index_t,double>(this,size);
  case DoubleSparse:
    return Tresize_sparse<index_t>(this,size);
  }  
}

void Array::set(const QString& field, ArrayVector& data) {
  if (m_type != Struct) throw Exception("Unsupported type for A.field=B");
  StructArray &rp(structPtr());
  if (isEmpty()) 
    rp.insert(field,BasicArray<Array>(NTuple(1,1)));
  if (!rp.contains(field))
    rp.insert(field,BasicArray<Array>(dimensions()));
  if (data.size() < length())
    throw Exception("Not enough right hand values to satisfy left hand side expression.");
  BasicArray<Array> &val(rp[field]);
  for (index_t i=1;i<=length();i++) {
    val.set(i,data.front());
    data.pop_front();
  }
}

const ArrayVector Array::get(const QString& field) const {
  if (m_type != Struct) throw Exception("Unsupported type for get(string)");
  const StructArray &rp(constStructPtr());
  if (!rp.contains(field)) throw Exception("Reference to non-existent field " + field);
  ArrayVector ret;
  const BasicArray<Array> &val(rp.value(field));
  for (index_t i=1;i<=val.length();i++)
    ret.push_back(val.get(i));
  return ret;
}

const Array Array::get(const IndexArray& index) const {
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
    return asArrayType().get(index);
  case CellArray:
    return Tget<const IndexArray&,Array>(this,index);
  case Struct:
    return Tget_struct<const IndexArray&>(this,index);
  case BoolArray:
    return Tget<const IndexArray&,logical>(this,index);
  case Int8Array:
    return Tget<const IndexArray&,int8>(this,index);
  case UInt8Array:
    return Tget<const IndexArray&,uint8>(this,index);
  case Int16Array:
    return Tget<const IndexArray&,int16>(this,index);
  case StringArray:
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
  case DoubleSparse:
    return Tget_sparse<const IndexArray&>(this,index);
  }  
}

const Array Array::get(const IndexArrayVector& index) const {
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
    return asArrayType().get(index);
  case CellArray:
    return Tget<const IndexArrayVector&,Array>(this,index);
  case Struct:
    return Tget_struct<const IndexArrayVector&>(this,index);
  case BoolArray:
    return Tget<const IndexArrayVector&,logical>(this,index);
  case Int8Array:
    return Tget<const IndexArrayVector&,int8>(this,index);
  case UInt8Array:
    return Tget<const IndexArrayVector&,uint8>(this,index);
  case Int16Array:
    return Tget<const IndexArrayVector&,int16>(this,index);
  case StringArray:
  case UInt16Array:
    return Tget<const IndexArrayVector&,uint16>(this,index);
  case Int32Array:
    return Tget<const IndexArrayVector&,int32>(this,index);
  case UInt32Array:
    return Tget<const IndexArrayVector&,uint32>(this,index);
  case Int64Array:
    return Tget<const IndexArrayVector&,int64>(this,index);
  case UInt64Array:
    return Tget<const IndexArrayVector&,uint64>(this,index);
  case FloatArray:
    return Tget<const IndexArrayVector&,float>(this,index);
  case DoubleArray:
    return Tget<const IndexArrayVector&,double>(this,index);
  case DoubleSparse:
    return Tget_sparse<const IndexArrayVector&>(this,index);
  }  
}

const Array Array::toType(const Type t) const {
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
  case StringArray:
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

const Array Array::get(const NTuple& index) const {
  switch (m_type) {
  default: 
    throw Exception("Unsupported type for get(const NTuple&)");
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
    if (!index.isScalar()) throw Exception("A(n) exceeds dimensions of A");
    return *this;
  case CellArray:
    return Tget_real<const NTuple&,Array>(this,index);
  case Struct:
    return Tget_struct_scalar<const NTuple&>(this,index);
  case BoolArray:
    return Array((logical)Tget_real<const NTuple&,logical>(this,index));
  case Int8Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,int8>(this,index),
		     Tget_imag<const NTuple&,int8>(this,index));
    else
      return Array(Tget_real<const NTuple&,int8>(this,index));
  case UInt8Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,uint8>(this,index),
		     Tget_imag<const NTuple&,uint8>(this,index));
    else
      return Array(Tget_real<const NTuple&,uint8>(this,index));
  case Int16Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,int16>(this,index),
		     Tget_imag<const NTuple&,int16>(this,index));
    else
      return Array(Tget_real<const NTuple&,int16>(this,index));
  case StringArray:
  case UInt16Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,uint16>(this,index),
		     Tget_imag<const NTuple&,uint16>(this,index));
    else
      return Array(Tget_real<const NTuple&,uint16>(this,index));
  case Int32Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,int32>(this,index),
		     Tget_imag<const NTuple&,int32>(this,index));
    else
      return Array(Tget_real<const NTuple&,int32>(this,index));
  case UInt32Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,uint32>(this,index),
		     Tget_imag<const NTuple&,uint32>(this,index));
    else
      return Array(Tget_real<const NTuple&,uint32>(this,index));
  case Int64Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,int64>(this,index),
		     Tget_imag<const NTuple&,int64>(this,index));
    else
      return Array(Tget_real<const NTuple&,int64>(this,index));
  case UInt64Array:
    if (m_complex)
      return Array(Tget_real<const NTuple&,uint64>(this,index),
		     Tget_imag<const NTuple&,uint64>(this,index));
    else
      return Array(Tget_real<const NTuple&,uint64>(this,index));
  case FloatArray:
    if (m_complex)
      return Array(Tget_real<const NTuple&,float>(this,index),
		     Tget_imag<const NTuple&,float>(this,index));
    else
      return Array(Tget_real<const NTuple&,float>(this,index));
  case DoubleArray:
    if (m_complex)
      return Array(Tget_real<const NTuple&,double>(this,index),
		     Tget_imag<const NTuple&,double>(this,index));
    else
      return Array(Tget_real<const NTuple&,double>(this,index));
  case DoubleSparse:
    if (m_complex)
      return Array(Get(constRealSparse(),index),
		     Get(constImagSparse(),index));
    else
      return Array(Get(constRealSparse(),index));
  }
}

const Array Array::get(index_t index) const {
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
  case CellArray:
    return Tget_real<index_t,Array>(this,index);
  case Struct:
    return Tget_struct_scalar<index_t>(this,index);
  case BoolArray:
    return Array((logical) Tget_real<index_t,logical>(this,index));
  case Int8Array:
    if (m_complex)
      return Array((int8)Tget_real<index_t,int8>(this,index),
		     (int8)Tget_imag<index_t,int8>(this,index));
    else
      return Array((int8)Tget_real<index_t,int8>(this,index));
  case UInt8Array:
    if (m_complex)
      return Array((uint8)Tget_real<index_t,uint8>(this,index),
		     (uint8)Tget_imag<index_t,uint8>(this,index));
    else
      return Array((uint8)Tget_real<index_t,uint8>(this,index));
  case Int16Array:
    if (m_complex)
      return Array((int16)Tget_real<index_t,int16>(this,index),
		     (int16)Tget_imag<index_t,int16>(this,index));
    else
      return Array((int16)Tget_real<index_t,int16>(this,index));
  case StringArray:
  case UInt16Array:
    if (m_complex)
      return Array((uint16)Tget_real<index_t,uint16>(this,index),
		     (uint16)Tget_imag<index_t,uint16>(this,index));
    else
      return Array((uint16)Tget_real<index_t,uint16>(this,index));
  case Int32Array:
    if (m_complex)
      return Array((int32)Tget_real<index_t,int32>(this,index),
		     (int32)Tget_imag<index_t,int32>(this,index));
    else
      return Array((int32)Tget_real<index_t,int32>(this,index));
  case UInt32Array:
    if (m_complex)
      return Array((uint32)Tget_real<index_t,uint32>(this,index),
		     (uint32)Tget_imag<index_t,uint32>(this,index));
    else
      return Array((uint32)Tget_real<index_t,uint32>(this,index));
  case Int64Array:
    if (m_complex)
      return Array((int64)Tget_real<index_t,int64>(this,index),
		     (int64)Tget_imag<index_t,int64>(this,index));
    else
      return Array((int64)Tget_real<index_t,int64>(this,index));
  case UInt64Array:
    if (m_complex)
      return Array((uint64)Tget_real<index_t,uint64>(this,index),
		     (uint64)Tget_imag<index_t,uint64>(this,index));
    else
      return Array((uint64)Tget_real<index_t,uint64>(this,index));
  case FloatArray:
    if (m_complex)
      return Array((float)Tget_real<index_t,float>(this,index),
		     (float)Tget_imag<index_t,float>(this,index));
    else
      return Array((float)Tget_real<index_t,float>(this,index));
  case DoubleArray:
    if (m_complex)
      return Array((double)Tget_real<index_t,double>(this,index),
		     (double)Tget_imag<index_t,double>(this,index));
    else
      return Array((double)Tget_real<index_t,double>(this,index));
  case DoubleSparse:
    if (m_complex)
      return Array(Get(constRealSparse(),index),
		     Get(constImagSparse(),index));
    else
      return Array(Get(constRealSparse(),index));
  }
}

const Array Array::get(const Array& index) const {
  if (index.isScalar() && (index.type() != BoolScalar)) {
    if (!index.allReal())
     Warn("Complex part of index ignored");
    return get(index.asIndexScalar());
  } else
    return get(IndexArrayFromArray(index));
}

const Array Array::get(const ArrayVector& index) const {
  if (AllNonBoolScalars(index)) {
    NTuple addr(1,1);
    for (int i=0;i<index.size();i++)
      addr[i] = index[i].asIndexScalar();
    return get(addr);
  }
  else {
    IndexArrayVector addr;
    for (int i=0;i<index.size();i++)
      addr.push_back(IndexArrayFromArray(index[i]));
    return get(addr);
  }  
}

void Array::set(const Array& index, const Array& data) {
  if (index.isScalar() && (index.type() != BoolScalar)) {
    if (!index.allReal())
      Warn("Complex part of index ignored");
    set(index.asIndexScalar(),data);
  }
  else
    set(IndexArrayFromArray(index),data);
}

void Array::set(const ArrayVector& index, const Array& data) {
  if (AllNonBoolScalars(index)) {
    NTuple addr(1,1);
    for (int i=0;i<index.size();i++)
      addr[i] = index[i].asIndexScalar();
    set(addr,data);
  }
  else {
    IndexArrayVector addr;
    for (int i=0;i<index.size();i++)
      addr.push_back(IndexArrayFromArray(index[i]));
    set(addr,data);
  }
}

void Array::addField(QString name) {
  if (type() != Struct)
    throw Exception("addField only valid for structure arrays");
  if (!structPtr().contains(name))
    structPtr().insert(name,BasicArray<Array>());
}

const index_t Array::asIndexScalar() const {
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

const Array Array::asScalar() const {
  if ((m_type >= BoolScalar) &&
      (m_type <= DoubleScalar)) return *this;
  if (isArray()) {
    if (dimensions().count() == 1) 
      return get((index_t)1);
    throw Exception("Illegal scalar -- array assignment");
  }
  throw Exception("Unhandled case");
}

// Need to make this more general - how so?
bool Array::operator==(const Array &b) const {
  if (isScalar() && b.isScalar()) {
    Array a_scalar(asScalar());
    Array b_scalar(b.asScalar());
    if (allReal() ^ b.allReal()) return false;
    if (a_scalar.type() != b_scalar.type()) return false;
    switch (a_scalar.type()) {
    default:
      throw Exception("Unhandled scalar case");
    case BoolScalar:
      return ((a_scalar.constRealScalar<logical>() == b_scalar.constRealScalar<logical>()));
    case Int8Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<int8>() == b_scalar.constRealScalar<int8>()));
      else
	return ((a_scalar.constRealScalar<int8>() == b_scalar.constRealScalar<int8>()) &&
		(a_scalar.constImagScalar<int8>() == b_scalar.constImagScalar<int8>()));
    case UInt8Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<uint8>() == b_scalar.constRealScalar<uint8>()));
      else
	return ((a_scalar.constRealScalar<uint8>() == b_scalar.constRealScalar<uint8>()) &&
		(a_scalar.constImagScalar<uint8>() == b_scalar.constImagScalar<uint8>()));
    case Int16Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<int16>() == b_scalar.constRealScalar<int16>()));
      else
	return ((a_scalar.constRealScalar<int16>() == b_scalar.constRealScalar<int16>()) &&
		(a_scalar.constImagScalar<int16>() == b_scalar.constImagScalar<int16>()));
    case UInt16Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<uint16>() == b_scalar.constRealScalar<uint16>()));
      else
	return ((a_scalar.constRealScalar<uint16>() == b_scalar.constRealScalar<uint16>()) &&
		(a_scalar.constImagScalar<uint16>() == b_scalar.constImagScalar<uint16>()));
    case Int32Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<int32>() == b_scalar.constRealScalar<int32>()));
      else
	return ((a_scalar.constRealScalar<int32>() == b_scalar.constRealScalar<int32>()) &&
		(a_scalar.constImagScalar<int32>() == b_scalar.constImagScalar<int32>()));
    case UInt32Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<uint32>() == b_scalar.constRealScalar<uint32>()));
      else
	return ((a_scalar.constRealScalar<uint32>() == b_scalar.constRealScalar<uint32>()) &&
		(a_scalar.constImagScalar<uint32>() == b_scalar.constImagScalar<uint32>()));
    case Int64Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<int64>() == b_scalar.constRealScalar<int64>()));
      else
	return ((a_scalar.constRealScalar<int64>() == b_scalar.constRealScalar<int64>()) &&
		(a_scalar.constImagScalar<int64>() == b_scalar.constImagScalar<int64>()));
    case UInt64Scalar:
      if (allReal())
	return ((a_scalar.constRealScalar<uint64>() == b_scalar.constRealScalar<uint64>()));
      else
	return ((a_scalar.constRealScalar<uint64>() == b_scalar.constRealScalar<uint64>()) &&
		(a_scalar.constImagScalar<uint64>() == b_scalar.constImagScalar<uint64>()));
    case FloatScalar:
      if (allReal())
	return ((a_scalar.constRealScalar<float>() == b_scalar.constRealScalar<float>()));
      else
	return ((a_scalar.constRealScalar<float>() == b_scalar.constRealScalar<float>()) &&
		(a_scalar.constImagScalar<float>() == b_scalar.constImagScalar<float>()));
    case DoubleScalar:
      if (allReal())
	return ((a_scalar.constRealScalar<double>() == b_scalar.constRealScalar<double>()));
      else
	return ((a_scalar.constRealScalar<double>() == b_scalar.constRealScalar<double>()) &&
		(a_scalar.constImagScalar<double>() == b_scalar.constImagScalar<double>()));
    }
    return false;
  }
  if (!(dimensions() == b.dimensions())) return false;
  if (type() != b.type()) return false;
  if (allReal() ^ b.allReal()) return false;
  switch (type()) {
  default:
    throw Exception("Unhandled case as argument to == operator for Array class");
  case CellArray:
    return constReal<Array>() == b.constReal<Array>();
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
  case StringArray:
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
  case DoubleSparse:
    if (allReal())
      return constRealSparse() == b.constRealSparse();
    else
      return ((constRealSparse() == b.constRealSparse()) &&
	      (constImagSparse() == b.constImagSparse()));
  }
}

std::ostream& operator<<(std::ostream& o, const Array &t) {
  t.print(o);
  return o;
}
  
bool IsColonOp(const Array &x) {
  return (x.type() == StringArray) && (x.string() == ":");
}

QString Array::string() const {
  if (m_type != StringArray) throw Exception("Cannot convert array to string");
  const BasicArray<uint16> &p(constReal<uint16>());
  QString ret;
  for (int i=0;i<p.length();i++)
    ret += QChar(p[i+1]);
  return ret;
}

template <typename T>
static inline void Tforce(Array *ptr, Type t) {
  if (ptr->allReal()) {
    ptr->m_type = t;
    ptr->m_real.p = new SharedObject(t, new BasicArray<T>(ptr->constRealScalar<T>()));
  } else {
    ptr->m_type = t;
    ptr->m_real.p = new SharedObject(t, new BasicArray<T>(ptr->constRealScalar<T>()));
    ptr->m_imag.p = new SharedObject(t, new BasicArray<T>(ptr->constImagScalar<T>()));
  }
}

Array Array::asArrayType() const {
  switch (type()) {
  default:
    return *this;
  case BoolScalar:
    return Array(BoolArray,new BasicArray<logical>(constRealScalar<logical>()));
  case Int8Scalar:
    if (allReal())
      return Array(Int8Array,new BasicArray<int8>(constRealScalar<int8>()));
    else
      return Array(Int8Array,new BasicArray<int8>(constRealScalar<int8>()),
		     new BasicArray<int8>(constImagScalar<int8>()));
  case UInt8Scalar:
    if (allReal())
      return Array(UInt8Array,new BasicArray<uint8>(constRealScalar<uint8>()));
    else
      return Array(UInt8Array,new BasicArray<uint8>(constRealScalar<uint8>()),
		     new BasicArray<uint8>(constImagScalar<uint8>()));
  case Int16Scalar:
    if (allReal())
      return Array(Int16Array,new BasicArray<int16>(constRealScalar<int16>()));
    else
      return Array(Int16Array,new BasicArray<int16>(constRealScalar<int16>()),
		     new BasicArray<int16>(constImagScalar<int16>()));
  case UInt16Scalar:
    if (allReal())
      return Array(UInt16Array,new BasicArray<uint16>(constRealScalar<uint16>()));
    else
      return Array(UInt16Array,new BasicArray<uint16>(constRealScalar<uint16>()),
		     new BasicArray<uint16>(constImagScalar<uint16>()));
  case Int32Scalar:
    if (allReal())
      return Array(Int32Array,new BasicArray<int32>(constRealScalar<int32>()));
    else
      return Array(Int32Array,new BasicArray<int32>(constRealScalar<int32>()),
		     new BasicArray<int32>(constImagScalar<int32>()));
  case UInt32Scalar:
    if (allReal())
      return Array(UInt32Array,new BasicArray<uint32>(constRealScalar<uint32>()));
    else
      return Array(UInt32Array,new BasicArray<uint32>(constRealScalar<uint32>()),
		     new BasicArray<uint32>(constImagScalar<uint32>()));
  case Int64Scalar:
    if (allReal())
      return Array(Int64Array,new BasicArray<int64>(constRealScalar<int64>()));
    else
      return Array(Int64Array,new BasicArray<int64>(constRealScalar<int64>()),
		     new BasicArray<int64>(constImagScalar<int64>()));
  case UInt64Scalar:
    if (allReal())
      return Array(UInt64Array,new BasicArray<uint64>(constRealScalar<uint64>()));
    else
      return Array(UInt64Array,new BasicArray<uint64>(constRealScalar<uint64>()),
		     new BasicArray<uint64>(constImagScalar<uint64>()));
  case FloatScalar:
    if (allReal())
      return Array(FloatArray,new BasicArray<float>(constRealScalar<float>()));
    else
      return Array(FloatArray,new BasicArray<float>(constRealScalar<float>()),
		     new BasicArray<float>(constImagScalar<float>()));
  case DoubleScalar:
    if (allReal())
      return Array(DoubleArray,new BasicArray<double>(constRealScalar<double>()));
    else
      return Array(DoubleArray,new BasicArray<double>(constRealScalar<double>()),
		     new BasicArray<double>(constImagScalar<double>()));
  }
}

const IndexArray IndexArrayFromArray(const Array &index) {
  if (index.type() == BoolScalar)
    return Find(index.asArrayType().constReal<logical>());
  if (!index.allReal())
    Warn("Complex part of index ignored");
  if (index.type() == DoubleArray)
    return index.constReal<index_t>();
  if (index.type() == BoolArray)
    return Find(index.constReal<logical>());
  Array index_converted(index.asArrayType().toType(DoubleArray));
  if (!index_converted.allReal())
    Warn("Complex part of index ignored");
  return index_converted.constReal<index_t>();
  if (IsColonOp(index))
    return IndexArray(-1);
  throw Exception("Unsupported index type");
}

const ArrayVector ArrayVectorFromCellArray(const Array &arg) {
  if (arg.type() != CellArray) 
    throw Exception("Unsupported type for call to toArrayVector");
  ArrayVector ret;
  const BasicArray<Array> &rp(arg.constReal<Array>());
  for (index_t i=1;i<=arg.length();i++)
    ret.push_back(rp.get(i));
  return ret;
}

const Array CellArrayFromArrayVector(ArrayVector &arg, index_t cnt) {
  Array ret(CellArray,NTuple(1,arg.size()));
  BasicArray<Array> &rp(ret.real<Array>());
  for (index_t i=1;i<=cnt;i++) {
    rp.set(i,arg.front());
    arg.pop_front();
  }
  return ret;
}

void SetCellContents(Array &cell, const Array& index, 
		     ArrayVector& data) {
  if (cell.type() != CellArray)
    throw Exception("A{B} = C only supported for cell arrays.");
  if (IsColonOp(index)) {
    if (cell.length() > data.size())
      throw Exception("Not enough right hand side values to satisfy left hand side expression.");
    cell.set(index,CellArrayFromArrayVector(data,data.size()));
    data.clear();
    return;
  }
  IndexArray ndx(IndexArrayFromArray(index));
  if (ndx.length() > data.size())
    throw Exception("Not enought right hand side values to satisfy left hand side expression.");
  cell.set(ndx,CellArrayFromArrayVector(data,ndx.length()));
}

void SetCellContents(Array &cell, const ArrayVector& index, 
		     ArrayVector& data) {
  if (cell.type() != CellArray)
    throw Exception("A{B1,B2,...BN} = B only supported for cell arrays.");
  IndexArrayVector addr;
  NTuple dims;
  for (int i=0;i<index.size();i++) {
    addr.push_back(IndexArrayFromArray(index[i]));
    if (IsColonOp(addr[i]))
      dims[i] = cell.dimensions()[i];
    else
      dims[i] = addr[i].length();
  }
  if (data.size() < dims.count())
    throw Exception("Not enough right hand side values to satisfy left hand side expression");
  cell.set(addr,CellArrayFromArrayVector(data,dims.count()));
}

QStringList FieldNames(const Array& arg) {
  if (arg.type() != Struct)
    throw Exception("fieldnames only valid for structure arrays");
  const StructArray &rp(arg.constStructPtr());
  StructArray::const_iterator i=rp.constBegin();
  QStringList ret;
  while (i != rp.constEnd()) {
    ret << i.key(); 
    ++i;
  }
  return ret;
}

SparseMatrix ToRealSparse(const Array& data) {
  if (data.type() == DoubleSparse) return data.constRealSparse();
  Array cdata(data);
  if (cdata.isScalar())
    cdata = data.asArrayType();
  if (!cdata.is2D()) throw Exception("Sparse matrix cannot be created from multidimensional arrays");
  cdata = cdata.toType(DoubleArray);
  return SparseMatrix(cdata.constReal<double>());
}

SparseMatrix ToImagSparse(const Array& data) {
  if (data.type() == DoubleSparse) return data.constImagSparse();
  Array cdata(data);
  if (cdata.isScalar())
    cdata = cdata.asArrayType();
  if (!cdata.is2D()) throw Exception("Sparse matrix cannot be created from multidimensional arrays");
  cdata = cdata.toType(DoubleArray);
  return SparseMatrix(cdata.constImag<double>());
}
