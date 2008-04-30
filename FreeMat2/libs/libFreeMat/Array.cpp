#include "Array.hpp"
#include "GetSet.hpp"
#include <QStringList>

void Warn(const char *msg) {
  std::cout << "Warning:" << msg;
}

template <typename T>
static inline void* Tconstruct(Type t, const void *copy) {
  if (t.Scalar == 1)
    throw Exception("Tconstruct called for scalar object!");
  if (t.Sparse == 1) {
    const SparseMatrix<T> *p = reinterpret_cast<const SparseMatrix<T>*>(copy);
    return reinterpret_cast<void*>(new SparseMatrix<T>(*p));
  }
  const BasicArray<T> *p = reinterpret_cast<const BasicArray<T>*>(copy);
  return reinterpret_cast<void*>(new BasicArray<T>(*p));
}

#define MacroTConstructCopy(ctype,cls)		\
  case cls: return Tconstruct<ctype>(t,copy); 

static inline void* construct(Type t, const void *copy) {
  switch (t.Class) {
  default:
    throw Exception("Unsupported construct");
    MacroExpandCasesAll(MacroTConstructCopy);
  case Struct:
    return reinterpret_cast<void*>(new StructArray(*(reinterpret_cast<const StructArray*>(copy))));
  }
}

#undef MacroTConstructCopy

template <typename T>
static inline void* Tconstruct_sized(Type t, const NTuple &dims) {
  if (t.Scalar == 1)
    throw Exception("Tconstruct_sized called for scalar object");
  if (t.Sparse == 1) {
    return reinterpret_cast<void*>(new SparseMatrix<T>(dims));
  }
  return reinterpret_cast<void*>(new BasicArray<T>(dims));
}

#define MacroTConstructSized(ctype,cls) \
  case cls: return Tconstruct_sized<ctype>(t,dims);

static inline void* construct_sized(Type t, const NTuple &dims) {
  switch (t.Class) {
  default:
    throw Exception("Unsupported constructor");
    MacroExpandCasesAll(MacroTConstructSized);
  case Struct:
    return reinterpret_cast<void*>(new StructArray);
  }
}

#undef MacroTConstructSized

template <typename T>
static inline void Tdestruct(Type t, void *todelete) {
  if (t.Scalar == 1)
    throw Exception("Tdestruct called on scalar");
  if (t.Sparse == 1) {
    delete reinterpret_cast<SparseMatrix<T>*>(todelete);
    return;
  }
  delete reinterpret_cast<BasicArray<T>*>(todelete);
  return;
}

#define MacroTDestruct(ctype,cls) \
  case cls: return Tdestruct<ctype>(t,todelete);

static inline void destruct(Type t, void *todelete) {
  switch (t.Class) {
  default:
    throw Exception("Unsupported construct");
    MacroExpandCasesAll(MacroTDestruct);
  case Struct:
    return Tdestruct<StructArray>(t,todelete);
  }
}

#undef MacroTDestruct

static inline bool AllNonBoolScalars(const ArrayVector& index) {
  for (int i=0;i<index.size();i++)
    if (!index[i].isScalar() || (index[i].type().Class == Bool)) return false;
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

template <typename T>
static inline void Tprint(std::ostream& o, const Array *ptr) {
  if (ptr->type().Scalar == 1) {
    if (ptr->allReal()) {
      o << double(ptr->constRealScalar<T>());
    } else {
      o << "(" << double(ptr->constRealScalar<T>()) << "," <<
	double(ptr->constImagScalar<T>()) << ")";
    }
  } else if (ptr->type().Sparse == 1) {
    if (ptr->allReal()) {
      o << "real:\n" << ptr->constRealSparse<T>();
    } else {
      o << "real:\n" << ptr->constRealSparse<T>();
      o << "imag:\n" << ptr->constImagSparse<T>();
    }
  } else {
    if (ptr->allReal()) {
      o << "real:\n" << ptr->constReal<T>();
    } else {
      o << "real:\n" << ptr->constReal<T>();
      o << "imag:\n" << ptr->constImag<T>();
    }
  }
}

#define MacroPrint(ctype,cls) \
  case cls: return Tprint<ctype>(o,this);

void Array::print(std::ostream& o) const {
  switch (m_type.Class) {
  default:
    throw Exception("unsupported type");
  case Invalid:
    o << "[] ";
    return;
    MacroExpandCases(MacroPrint);
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
  case StringArray:
    {
      QString val(string());
      o << val.toStdString().c_str();
      return;
    }
  }
}

#undef MacroPrint

Array::Array(DataClass t, const NTuple &dims) {
  m_type.Class = t;
  m_type.Complex = 0;
  m_type.Sparse = 0;
  m_type.Scalar = 0;
  m_real.p = new SharedObject(m_type,construct_sized(m_type,dims));
}


Array::Array(const QString &text) {
  m_type.Class = StringArray;
  m_type.Complex = 0;
  m_type.Sparse = 0;
  m_type.Scalar = 0;
  m_real.p = new SharedObject(m_type,
			      construct_sized(m_type,
					      NTuple(1,text.size())));
  BasicArray<uint16> &p(real<uint16>());
  for (int i=0;i<text.size();i++) 
    p[i+1] = text[i].unicode();
}

template <typename T>
static inline const NTuple Tdim(const Array *ptr) {
  if (ptr->type().Sparse == 1)
    return ptr->constRealSparse<T>().dimensions();
  else
    return ptr->constReal<T>().dimensions();
}

#define MacroDimensions(ctype,cls)			\
  case cls: return Tdim<ctype>(this);

const NTuple Array::dimensions() const {
  if (m_type.Scalar == 1) return NTuple(1,1);
  switch (m_type.Class) {
  default:
    throw Exception("unhandled type");
  case Struct:
    {
      const StructArray &rp(constStructPtr());
      StructArray::const_iterator i=rp.constBegin();
      if (i == rp.constEnd()) return NTuple(0,0);
      return i.value().dimensions();
    }
    MacroExpandCasesAll(MacroDimensions);
  }
}

#undef MacroDimensions

template <typename S, typename T>
static inline void Tset_scalar(Array *ptr, S ndx, const Array& data) {
  if (ptr->isSparse()) {
    ptr->realSparse<T>().set(ndx,data.constRealScalar<T>());
    if (!data.allReal())
      ptr->imagSparse<T>().set(ndx,data.constImagScalar<T>());
    return;
  }
  ptr->real<T>().set(ndx,data.constRealScalar<T>());
  if (!data.allReal())
    ptr->imag<T>().set(ndx,data.constImagScalar<T>());
}

template <typename S>
static inline const void Tset_struct_scalar(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.type().Class != Struct)
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


#define MacroSetIndexT(ctype,cls)		\
  case cls: Tset_scalar<index_t,ctype>(this,index,data); return;

void Array::set(index_t index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetIndexT);
  case Struct: Tset_struct_scalar<index_t>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n) = B");
  }
}

#undef MacroSetIndexT

#define MacroSetNTuple(ctype,cls)		\
  case cls: Tset_scalar<const NTuple&,ctype>(this,index,data); return;

void Array::set(const NTuple& index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetNTuple);
  case Struct: Tset_struct_scalar<const NTuple&>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n1,..,nm) = B");
  }
}

#undef MacroSetNTuple

template <typename S, typename T>
static inline void Tset(Array* ptr, S ndx, const Array& data) {
  if (ptr->isSparse()) {
    Set(ptr->realSparse<T>(),ndx,ToRealSparse<T>(data));
    if (!data.allReal())
      Set(ptr->imagSparse<T>(),ndx,ToImagSparse<T>(data));
    return;
  }
  Array dataTyped(data.toClass(ptr->type().Class));
  Set(ptr->real<T>(),ndx,dataTyped.constReal<T>());
  if (!data.allReal()) {
    Set(ptr->imag<T>(),ndx,dataTyped.constImag<T>());
  }
}

template <typename S>
static inline const void Tset_struct(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.type().Class != Struct)
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

#define MacroSetIndexArray(ctype,cls)		\
  case cls: Tset<const IndexArray&,ctype>(this,index,data);

void Array::set(const IndexArray& index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetIndexArray);
  case Struct: Tset_struct<const IndexArray&>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n) = B");
  }  
}

#undef MacroSetIndexArray

#define MacroSetIndexArrayVector(ctype,cls) \
  case cls: Tset<const IndexArrayVector&,ctype>(this,index,data);

void Array::set(const IndexArrayVector& index, const Array& data) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroSetIndexArrayVector);
  case Struct: Tset_struct<const IndexArrayVector&>(this,index,data); return;
  default:
    throw Exception("Unhandled case for A(n1,...,nm) = B");
  }  
}

#undef MacroSetIndexArrayVector

template <typename S>
static inline void Treshape_struct(Array* ptr, S ndx) {
  StructArray &lp(ptr->structPtr());
  StructArray::iterator i=lp.begin();
  while (i != lp.end()) {
    i.value().reshape(ndx);
    ++i;
  }
}

template <typename S, typename T>
static inline void Treshape(Array* ptr, S ndx) {
  if (ptr->isSparse()) {
    ptr->realSparse<T>().reshape(ndx);
    if (!ptr->allReal())
      ptr->imagSparse<T>().reshape(ndx);
  } else {
    ptr->real<T>().reshape(ndx);
    if (!ptr->allReal())
      ptr->imag<T>().reshape(ndx);
  }
}

#define MacroReshapeNTuple(ctype,cls) \
  case cls: Treshape<const NTuple&, ctype>(this,size);

void Array::reshape(const NTuple &size) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroReshapeNTuple);
  case Struct: return Treshape_struct<const NTuple&>(this,size);
  default:
    throw Exception("Unhandled case for reshape");
  }
}

#undef MacroReshapeNTuple

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
  if (ptr->isSparse()) {
    ptr->realSparse<T>().resize(ndx);
    if (!ptr->allReal())
      ptr->imagSparse<T>().resize(ndx);
  } else {
    ptr->real<T>().resize(ndx);
    if (!ptr->allReal())
      ptr->imag<T>().resize(ndx);
  }
}

#define MacroResizeNTuple(ctype,cls) \
  case cls: Tresize<const NTuple&,ctype>(this,size);

void Array::resize(const NTuple &size) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroResizeNTuple);
  case Struct: return Tresize_struct<const NTuple&>(this,size);
  default:
    throw Exception("Unhandled case for resize");
  }
}
#undef MacroResizeNTuple

#define MacroResizeIndex(ctype,cls) \
  case cls: Tresize<index_t,ctype>(this,size);

void Array::resize(index_t size) {
  ensureNotScalarEncoded();
  switch (m_type.Class) {
    MacroExpandCasesAll(MacroResizeIndex);
  case Struct: return Tresize_struct<index_t>(this,size);
  default:
    throw Exception("Unhandled case for resize");
  }
}

#undef MacroResizeIndex

void Array::set(const QString& field, ArrayVector& data) {
  if (m_type.Class != Struct) throw Exception("Unsupported type for A.field=B");
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
  if (m_type.Class != Struct) throw Exception("Unsupported type for get(string)");
  const StructArray &rp(constStructPtr());
  if (!rp.contains(field)) throw Exception("Reference to non-existent field " + field);
  ArrayVector ret;
  const BasicArray<Array> &val(rp.value(field));
  for (index_t i=1;i<=val.length();i++)
    ret.push_back(val.get(i));
  return ret;
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

template <typename S, typename T>
static inline const Array Tget(const Array *ptr, S ndx) {
  if (ptr->isSparse()) {
    if (ptr->allReal())
      return Array(ptr->type().Class,
		   Get(ptr->constRealSparse<T>(),ndx));
    else
      return Array(ptr->type().Class,
		   Get(ptr->constRealSparse<T>(),ndx),
		   Get(ptr->constImagSparse<T>(),ndx));
  } else {
    if (ptr->allReal())
      return Array(ptr->type().Class,
		   Get(ptr->constReal<T>(),ndx));
    else
      return Array(ptr->type().Class,
		   Get(ptr->constReal<T>(),ndx),
		   Get(ptr->constImag<T>(),ndx));
  }
}

#define MacroGetIndexArray(ctype,cls) \
  case cls: return Tget<const IndexArray&,ctype>(this,index);

const Array Array::get(const IndexArray& index) const {
  if (m_type.Scalar == 1)
    return asDenseArray().get(index);
  switch (m_type.Class) {
  default: 
    throw Exception("Unsupported type for get(index_t)");
  MacroExpandCasesAll(MacroGetIndexArray);
  case Struct: return Tget_struct<const IndexArray&>(this,index);
  }
}

#undef MacroGetIndexArray


#define MacroGetIndexArrayVector(ctype,cls) \
  case cls: return Tget<const IndexArrayVector&,ctype>(this,index);

const Array Array::get(const IndexArrayVector& index) const {
  if (m_type.Scalar == 1)
    return asDenseArray().get(index);
  switch (m_type.Class) {
  default:
    throw Exception("Unsupported type for get(indexarrayvector)");
  MacroExpandCasesAll(MacroGetIndexArrayVector);
  case Struct: return Tget_struct<const IndexArrayVector&>(this,index);
  }
}

#undef MacroGetIndexArrayVector

template <typename S, typename T>
inline static const Array Tcast(DataClass t, const Array *ptr) {
  if (ptr->isScalar()) {
    if (ptr->allReal())
      return Array(T(ptr->constRealScalar<S>()));
    else
      return Array(T(ptr->constRealScalar<S>()),
		   T(ptr->constImagScalar<S>()));
  }
  if (ptr->isSparse()) {
    if (ptr->allReal())
      return Array(t,ConvertSparseArray<S,T>(ptr->constRealSparse<S>()));
    else
      return Array(t,
		   ConvertSparseArray<S,T>(ptr->constRealSparse<S>()),
		   ConvertSparseArray<S,T>(ptr->constImagSparse<S>()));
  }
  if (ptr->allReal())
    return Array(t,ConvertBasicArray<S,T>(ptr->constReal<S>()));
  else
    return Array(t,
		 ConvertBasicArray<S,T>(ptr->constReal<S>()),
		 ConvertBasicArray<S,T>(ptr->constImag<S>()));
}

#define MacroTcast(ctype,cls) \
  case cls: return Tcast<ctype,T>(t,ptr);

template <typename T>
inline static const Array TcastCase(DataClass t, const Array *ptr) {
  switch (ptr->type().Class) {
  default:
    throw Exception("Cannot perform type conversions with this type");
    MacroExpandCasesSimple(MacroTcast);
  }
}

#undef MacroTcast

#define MacroTcastCase(ctype,cls) \
  case cls: return TcastCase<ctype>(t,this);

const Array Array::toClass(DataClass t) const {
  if (type().Class == t) return *this;
  switch (t) {
  default:
    throw Exception("Cannot perform type conversions with this type.");
    MacroExpandCasesSimple(MacroTcastCase);
  }
}

#undef MacroTcastCase

template <typename S, typename T>
static inline Array Tget_scalar(const Array *ptr, S ndx) {
  if (ptr->isSparse()) {
    if (ptr->allReal()) 
      return Array(ptr->constRealSparse<T>().get(ndx));
    else
      return Array(ptr->constRealSparse<T>().get(ndx),
		   ptr->constImagSparse<T>().get(ndx));
  }
  if (ptr->allReal()) 
    return Array(ptr->constReal<T>().get(ndx));
  else
    return Array(ptr->constReal<T>().get(ndx),
		 ptr->constImag<T>().get(ndx));
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

#define MacroGetNTuple(ctype,cls) \
  case cls: return Tget_scalar<const NTuple&,ctype>(this,index);

const Array Array::get(const NTuple& index) const {
  if ((m_type.Scalar == 1) && index.isScalar())
    return *this;
  switch (m_type.Class) {
  default:
    throw Exception("Unsupported type for get(const NTuple&)");
    MacroExpandCasesAll(MacroGetNTuple);
  case Struct: return Tget_struct_scalar<const NTuple&>(this,index);
  }
}

#undef MacroGetNTuple

#define MacroGetIndexT(ctype,cls) \
  case cls: return Tget_scalar<index_t,ctype>(this,index);

const Array Array::get(index_t index) const {
  if ((m_type.Scalar == 1) && (index == 1))
    return *this;
  switch (m_type.Class) {
  default:
    throw Exception("Unhandled case for get(index)");
    MacroExpandCasesAll(MacroGetIndexT);
  case Struct: return Tget_struct_scalar<index_t>(this,index);
  }
}

#undef MacroGetIndexT

const Array Array::get(const Array& index) const {
  if (index.isScalar() && (index.type().Class != Bool)) {
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
  if (index.isScalar() && (index.type().Class != Bool)) {
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
  if (type().Class != Struct)
    throw Exception("addField only valid for structure arrays");
  if (!structPtr().contains(name))
    structPtr().insert(name,BasicArray<Array>());
}

#define MacroAsIndexScalar(ctype,cls) \
  case cls:			      \
  return index_t(constRealScalar<ctype>());

const index_t Array::asIndexScalar() const {
  switch (type().Class) {
  default:
    throw Exception("Unsupported type called on asIndexScalar");
  case Bool:
    if (constRealScalar<bool>()) return (index_t) 1;
    return (index_t) 0;
  MacroExpandCasesNoBool(MacroAsIndexScalar);    
  }
}

#undef MacroAsIndexScalar

template <typename T>
static inline void T_force_complex(Array *ptr) {
  if (!ptr->allReal()) return;
  if (ptr->type().Scalar == 1) {
    ptr->imagScalar<T>() = 0;
  } else if (ptr->type().Sparse == 1) {
    ptr->imagSparse<T>();
  } else {
    ptr->imag<T>();
  }
}

#define MacroForceComplex(ctype,cls) \
  case cls: return T_force_complex<ctype>(this);

void Array::forceComplex() {
  switch (type().Class) {
  default:
    return;
    MacroExpandCasesNoBool(MacroForceComplex);
  }
}

static inline bool Tequals_struct(const Array *pA, const Array *pB) {
  const StructArray &ap(pA->constStructPtr());
  const StructArray &bp(pB->constStructPtr());
  StructArray::const_iterator i=ap.constBegin();
  StructArray::const_iterator j=bp.constBegin();
  while ((i!= ap.constEnd()) && (j != bp.constEnd())) {
    if (!(i.key() == j.key())) return false;
    if (!(i.value() == j.value())) return false;
    ++i; ++j;
  }
  if ((i != ap.constEnd()) || (j != bp.constEnd()))
    return false;
  return true;
}

template <typename T>
static inline bool Tequals_scalar(const Array *pA, const Array *pB) {
  if (pA->allReal())
    return (pA->constRealScalar<T>() == pB->constRealScalar<T>());
  else
    return ((pA->constRealScalar<T>() == pB->constRealScalar<T>()) &&
	    (pA->constImagScalar<T>() == pB->constImagScalar<T>()));
}

template <typename T>
static inline bool Tequals_array(const Array *pA, const Array *pB) {
  if (pA->isSparse() && pB->isSparse()) {
    if (pA->allReal())
      return (pA->constRealSparse<T>() == pB->constRealSparse<T>());
    else
      return ((pA->constRealSparse<T>() == pB->constRealSparse<T>()) &&
	      (pA->constImagSparse<T>() == pB->constImagSparse<T>()));
  } else {
    const Array &dA(pA->asDenseArray());
    const Array &dB(pB->asDenseArray());
    if (dA.allReal())
      return (dA.constReal<T>() == dB.constReal<T>());
    else 
      return ((dA.constReal<T>() == dB.constReal<T>()) &&
	      (dA.constImag<T>() == dB.constImag<T>()));
  }
}

#define MacroScalarEquals(ctype,cls) \
  case cls: return Tequals_scalar<ctype>(this,&b);

#define MacroArrayEquals(ctype,cls) \
  case cls: return Tequals_array<ctype>(this,&b);

// Need to make this more general - how so?
bool Array::operator==(const Array &b) const {
  if (isScalar() && b.isScalar()) {
    if (allReal() ^ b.allReal()) return false;
    if (type().Class != b.type().Class) return false;
    switch (type().Class) {
    default:
      throw Exception("Unhandled scalar case");
      MacroExpandCasesAll(MacroScalarEquals);
    }
    return false;
  }
  if (!(dimensions() == b.dimensions())) return false;
  if (type().Class != b.type().Class) return false;
  if (allReal() ^ b.allReal()) return false;
  switch (type().Class) {
  default:
    throw Exception("Unhandled case as argument to == operator for Array class");
    MacroExpandCasesAll(MacroArrayEquals);
  case Struct: return Tequals_struct(this,&b);
  }
  return false;
}

#undef MacroArrayEquals

#undef MacroScalarEquals

std::ostream& operator<<(std::ostream& o, const Array &t) {
  t.print(o);
  return o;
}
  
bool IsColonOp(const Array &x) {
  return (x.type().Class == StringArray) && (x.string() == ":");
}

template <typename T>
static inline bool Tis_negative(const Array &x) {
  if (x.isSparse())
    return IsNonNegative(x.constRealSparse<T>());
  else
    return IsNonNegative(x.constReal<T>());
}

bool IsNonNegative(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.type().Class) {
  default:
    return false;
  case Bool:
  case UInt8:
  case UInt16:
  case StringArray:
  case UInt32:
  case UInt64:
    return true;
  case Int8: return Tis_negative<int8>(x);
  case Int16: return Tis_negative<int16>(x);
  case Int32: return Tis_negative<int32>(x);
  case Int64: return Tis_negative<int64>(x);
  case Float: return Tis_negative<float>(x);
  case Double: return Tis_negative<double>(x);
  }
}

bool IsUnsigned(const Array &x) {
  switch (x.type().Class) {
  default:
    return false;
  case Bool:
  case UInt8:
  case StringArray:
  case UInt16:
  case UInt32:
  case UInt64:
    return true;
  }
}

template <typename T>
static inline bool Tis_integer(const Array &x) {
  if (x.isSparse()) {
    return IsInteger(x.constRealSparse<T>());
  } else {
    return IsInteger(x.constReal<T>());
  }
}

bool IsInteger(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.type().Class) {
  default:
    return false;
  case Int8:
  case UInt8:
  case Int16:
  case UInt16:
  case Int32:
  case UInt32:
  case Int64:
  case UInt64:
    return true;
  case Float: return Tis_integer<float>(x);
  case Double: return Tis_integer<double>(x);
  }
}

int32 Array::integer() const {
  return (this->toClass(Int32).constRealScalar<int32>());
}

QString Array::string() const {
  if (m_type.Class != StringArray) throw Exception("Cannot convert array to string");
  const BasicArray<uint16> &p(constReal<uint16>());
  QString ret;
  for (int i=0;i<p.length();i++)
    ret += QChar(p[i+1]);
  return ret;
}

template <typename T>
static inline Array Tscalar_to_dense(const Array *ptr) {
  if (ptr->allReal())
    return Array(ptr->type().Class, new BasicArray<T>(ptr->constRealScalar<T>()));
  else {
    return Array(ptr->type().Class, 
		 new BasicArray<T>(ptr->constRealScalar<T>()),
		 new BasicArray<T>(ptr->constImagScalar<T>()));
  }
}

template <typename T>
static inline Array Tsparse_to_dense(const Array *ptr) {
  if (ptr->allReal())
    return Array(ptr->type().Class,ptr->constRealSparse<T>().asDense());
  else
    return Array(ptr->type().Class,
		 ptr->constRealSparse<T>().asDense(),
		 ptr->constImagSparse<T>().asDense());
}

#define MacroScalarToDense(ctype,cls) \
  case cls: return Tscalar_to_dense<ctype>(this);

#define MacroSparseToDense(ctype,cls) \
  case cls: return Tsparse_to_dense<ctype>(this);

Array Array::asDenseArray() const {
  if ((m_type.Scalar == 0) && (m_type.Sparse == 0))
    return *this;
  if (m_type.Scalar == 1) {
    switch (m_type.Class) {
    default:
      throw Exception("Cannot convert supplied array to dense type");
      MacroExpandCasesAll(MacroScalarToDense);
    }
  }
  switch (m_type.Class) {
  default:
    throw Exception("Cannot convert supplied sparse array to dense type");
    MacroExpandCasesAll(MacroSparseToDense);
  }
}

#undef MacroSparseToDense
#undef MacroScalarToDense

const IndexArray IndexArrayFromArray(const Array &index) {
  if ((index.type().Class == Bool) && (index.type().Scalar == 1))
    return Find(index.asDenseArray().constReal<bool>());
  if (index.type().Sparse == 1)
    throw Exception("Sparse indexing not supported currently");
  if (!index.allReal())
    Warn("Complex part of index ignored");
  if (index.type().Class == Double)
    return index.constReal<index_t>();
  if (index.type().Class == Bool)
    return Find(index.constReal<logical>());
  Array index_converted(index.toClass(Double));
  if (!index_converted.allReal())
    Warn("Complex part of index ignored");
  return index_converted.constReal<index_t>();
  if (IsColonOp(index))
    return IndexArray(-1);
  throw Exception("Unsupported index type");
}

const ArrayVector ArrayVectorFromCellArray(const Array &arg) {
  if (arg.type().Class != CellArray) 
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
  if (cell.type().Class != CellArray)
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
  if (cell.type().Class != CellArray)
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
  if (arg.type().Class != Struct)
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
