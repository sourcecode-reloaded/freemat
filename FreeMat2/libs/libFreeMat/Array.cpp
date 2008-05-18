#include "Array.hpp"
#include "GetSet.hpp"
#include <QStringList>
#include "Struct.hpp"

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
    if (!index[i].isScalar() || (index[i].dataClass() == Bool)) return false;
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
      if (rp.fieldCount() == 0) return NTuple(0,0);
      return rp[0].dimensions();
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
  if (rhs.dataClass() != Struct)
    throw Exception("Assignment A(I)=B where A is a structure array implies that B is also a structure array.");
  // First loop through the elements
  const StructArray &rp(rhs.constStructPtr());
  StructArray &lp(ptr->structPtr());
  for (int i=0;i<rp.fieldCount();i++) 
    Set(lp[rp.fieldName(i)],ndx,rp[i].get(1));
  // Loop through the output and force all arrays to be the same size
  NTuple newSize;
  for (int i=0;i<lp.fieldCount();i++)
    newSize.cover(lp[i].dimensions());
  for (int i=0;i<lp.fieldCount();i++)
    lp[i].resize(newSize);
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
  Array dataTyped(data.toClass(ptr->dataClass()));
  Set(ptr->real<T>(),ndx,dataTyped.constReal<T>());
  if (!data.allReal()) {
    Set(ptr->imag<T>(),ndx,dataTyped.constImag<T>());
  }
}

template <typename S>
static inline const void Tset_struct(Array*ptr, S ndx, const Array &rhs) {
  if (rhs.dataClass() != Struct)
    throw Exception("Assignment A(I)=B where A is a structure array implies that B is also a structure array.");
  // First loop through the elements
  const StructArray &rp(rhs.constStructPtr());
  StructArray &lp(ptr->structPtr());
  NTuple newSize(ptr->dimensions());
  for (int i=0;i<rp.fieldCount();i++) {
    Set(lp[rp.fieldName(i)],ndx,rp[i]);
    newSize = lp[rp.fieldName(i)].dimensions();
  }
  // Loop through the output and force all arrays to be the same size
  for (int j=0;j<lp.fieldCount();j++) 
    lp[j].resize(newSize);
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
  for (int i=0;i<lp.fieldCount();i++)
    lp[i].reshape(ndx);
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
  for (int i=0;i<lp.fieldCount();i++)
    lp[i].resize(ndx);
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
  const BasicArray<Array> &val(rp[field]);
  for (index_t i=1;i<=val.length();i++)
    ret.push_back(val.get(i));
  return ret;
}

template <typename S>
static inline Array Tget_struct(const Array*ptr, S ndx) {
  const StructArray &rp(ptr->constStructPtr());
  Array ret(Struct,NTuple(1,1));
  StructArray &lp(ret.structPtr());
  for (int i=0;i<rp.fieldCount();i++)
    lp[rp.fieldName(i)] = Get(rp[i],ndx);
  return ret;
}

template <typename S, typename T>
static inline const Array Tget(const Array *ptr, S ndx) {
  if (ptr->isSparse()) {
    if (ptr->allReal())
      return Array(Get(ptr->constRealSparse<T>(),ndx));
    else
      return Array(Get(ptr->constRealSparse<T>(),ndx),
		   Get(ptr->constImagSparse<T>(),ndx));
  } else {
    if (ptr->allReal())
      return Array(Get(ptr->constReal<T>(),ndx));
    else
      return Array(Get(ptr->constReal<T>(),ndx),
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
      return Array(ConvertSparseArray<S,T>(ptr->constRealSparse<S>()));
    else
      return Array(ConvertSparseArray<S,T>(ptr->constRealSparse<S>()),
		   ConvertSparseArray<S,T>(ptr->constImagSparse<S>()));
  }
  if (ptr->allReal())
    return Array(ConvertBasicArray<S,T>(ptr->constReal<S>()));
  else
    return Array(ConvertBasicArray<S,T>(ptr->constReal<S>()),
		 ConvertBasicArray<S,T>(ptr->constImag<S>()));
}

#define MacroClassName(ctype,cls) \
  case cls: return QString(#cls).toLower();

QString Array::className() const {
  if ((dataClass() == Struct) && (constStructPtr().isUserClass()))
    return constStructPtr().className();
  else {
    switch (dataClass()) {
    default: throw Exception("Unknown class?!");
    case CellArray: return QString("cell");
    case Struct: return QString("struct");
    case StringArray: return QString("string");
    case Bool: return QString("logical");
      MacroExpandCasesNoBool(MacroClassName);
    }
  }
}

#undef MacroClassName

bool Array::isUserClass() const {
  return ((dataClass() == Struct) && constStructPtr().isUserClass());;
}

#define MacroTcast(ctype,cls) \
  case cls: return Tcast<ctype,T>(t,ptr);

template <typename T>
inline static const Array TcastCase(DataClass t, const Array *ptr) {
  switch (ptr->dataClass()) {
  default:
    throw Exception("Cannot perform type conversions with this type");
    MacroExpandCasesSimple(MacroTcast);
  }
}

#undef MacroTcast

#define MacroTcastCase(ctype,cls) \
  case cls: return TcastCase<ctype>(t,this);

const Array Array::toClass(DataClass t) const {
  if (dataClass() == t) return *this;
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
  for (int i=0;i<rp.fieldCount();i++)
    lp[rp.fieldName(i)].set(1,Get(rp[i],ndx).get(1));
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
  if (index.isScalar() && (index.dataClass() != Bool)) {
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
  if (index.isScalar() && (index.dataClass() != Bool)) {
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
  if (dataClass() != Struct)
    throw Exception("addField only valid for structure arrays");
  if (!structPtr().contains(name))
    structPtr().insert(name,BasicArray<Array>());
}

#define MacroAsIndexScalar(ctype,cls) \
  case cls:			      \
  return index_t(constRealScalar<ctype>());

const index_t Array::asIndexScalar() const {
  switch (dataClass()) {
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
  switch (dataClass()) {
  default:
    return;
    MacroExpandCasesNoBool(MacroForceComplex);
  }
}


#define MacroAsComplex(ctype,cls) \
  case cls: return T_as_complex<ctype>(this);

const Array Array::asComplex() const {
  if (!allReal()) return *this;
  Array tmp(*this);
  tmp.forceComplex();
  return tmp;
}

static inline bool Tequals_struct(const Array *pA, const Array *pB) {
  const StructArray &ap(pA->constStructPtr());
  const StructArray &bp(pB->constStructPtr());
  if (ap.fieldNames() != bp.fieldNames()) return false;
  for (int i=0;i<ap.fieldCount();i++)
    if (!(ap[i] == bp[i])) return false;
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
    if (dataClass() != b.dataClass()) return false;
    switch (dataClass()) {
    default:
      throw Exception("Unhandled scalar case");
      MacroExpandCasesAll(MacroScalarEquals);
    }
    return false;
  }
  if (!(dimensions() == b.dimensions())) return false;
  if (dataClass() != b.dataClass()) return false;
  if (allReal() ^ b.allReal()) return false;
  switch (dataClass()) {
  default:
    throw Exception("Unhandled case as argument to == operator for Array class");
    MacroExpandCasesAll(MacroArrayEquals);
  case Struct: return Tequals_struct(this,&b);
  }
  return false;
}

#undef MacroArrayEquals

#undef MacroScalarEquals

bool IsColonOp(const Array &x) {
  return (x.dataClass() == StringArray) && (x.asString() == ":");
}

template <typename T>
static inline bool Tis_negative(const Array &x) {
  if (x.isSparse())
    return IsNonNegative(x.constRealSparse<T>());
  else if (x.isScalar())
    return IsNonNegative(x.constRealScalar<T>());
  else
    return IsNonNegative(x.constReal<T>());
}

#define MacroIsNonNegative(ctype, cls) \
  case cls: return Tis_negative<ctype>(x);

bool IsNonNegative(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.dataClass()) {
  default:
    return false;
  case Bool:
  case UInt8:
  case UInt16:
  case StringArray:
  case UInt32:
  case UInt64:
    return true;
    MacroExpandCasesSigned(MacroIsNonNegative);
  }
}
#undef MacroIsNonNegative


bool IsUnsigned(const Array &x) {
  switch (x.dataClass()) {
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
  } else if (x.isScalar()) {
    return IsInteger(x.constRealScalar<T>());
  } else {
    return IsInteger(x.constReal<T>());
  }
}

bool IsInteger(const Array &x) {
  if (!x.allReal()) return false;
  switch (x.dataClass()) {
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

int32 Array::asInteger() const {
  return (this->toClass(Int32).constRealScalar<int32>());
}

double Array::asDouble() const {
  return (this->toClass(Double).constRealScalar<double>());
}

QString Array::asString() const {
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
    return Array(new BasicArray<T>(ptr->constRealScalar<T>()));
  else {
    return Array(new BasicArray<T>(ptr->constRealScalar<T>()),
		 new BasicArray<T>(ptr->constImagScalar<T>()));
  }
}

template <typename T>
static inline Array Tsparse_to_dense(const Array *ptr) {
  if (ptr->allReal())
    return Array(ptr->constRealSparse<T>().asDense());
  else
    return Array(ptr->constRealSparse<T>().asDense(),
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
  if ((index.dataClass() == Bool) && (index.type().Scalar == 1))
    return Find(index.asDenseArray().constReal<bool>());
  if (index.type().Sparse == 1)
    throw Exception("Sparse indexing not supported currently");
  if (!index.allReal())
    Warn("Complex part of index ignored");
  if (index.dataClass() == Double)
    return index.constReal<index_t>();
  if (index.dataClass() == Bool)
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
  if (arg.dataClass() != CellArray) 
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
  if (cell.dataClass() != CellArray)
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
  if (cell.dataClass() != CellArray)
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
  if (arg.dataClass() != Struct)
    throw Exception("fieldnames only valid for structure arrays");
  const StructArray &rp(arg.constStructPtr());
  return rp.fieldNames();
}

static inline void do_single_sided_algo_double(double a, double b,double *pvec, 
					       int adder, int count) {
  double d = a;
  for (int i=0;i<count;i++) {
    pvec[i*adder] = (double) d;
    d += b;
  }
}
  
static inline void do_double_sided_algo_double(double a, double b, double c, double *pvec, 
					       int adder, int count) {
  if (count%2) {
    do_single_sided_algo_double(a,b,pvec,adder,count/2);
    do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2+1);
  } else {
    do_single_sided_algo_double(a,b,pvec,adder,count/2);
    do_single_sided_algo_double(c,-b,pvec+(count-1)*adder,-adder,count/2);
  }
}

Array RangeConstructor(double minval, double stepsize, double maxval, bool vert) {
  NTuple dim;
  if (stepsize == 0) 
    throw Exception("step size must be nonzero in colon expression");
  //ideally, n = (c-a)/b
  // But this really defines an interval... we let
  // n_min = min(c-a)/max(b)
  // n_max = max(c-a)/min(b)
  // where min(x) = {y \in fp | |y| is max, |y| < |x|, sign(y) = sign(x)}
  //       max(x) = {y \in fp | |y| is min, |y| > |x|, sign(y) = sign(x)}
  double ntest_min = nextafter(maxval-minval,0)/nextafter(stepsize,stepsize+stepsize);
  double ntest_max = nextafter(maxval-minval,maxval-minval+stepsize)/nextafter(stepsize,0);
  index_t npts = (index_t) floor(ntest_max);
  bool use_double_sided = (ntest_min <= npts) && (npts <= ntest_max);
  npts++;
  if (npts < 0) npts = 0;
  if (vert) {
    dim = NTuple(npts,1);
  } else {
    dim = NTuple(1,npts);
  }
  BasicArray<double> rp(dim);
  if (use_double_sided)
    do_double_sided_algo_double(minval,stepsize,maxval,rp.data(),int(1),int(npts));
  else
    do_single_sided_algo_double(minval,stepsize,rp.data(),int(1),int(npts));
  return Array(rp);
}

template <typename T>
static inline Array T_Transpose(const Array &x) {
  if (x.isScalar())
    return x;
  if (x.isSparse()) {
    if (x.allReal())
      return Array(Transpose(x.constRealSparse<T>()));
    else
      return Array(Transpose(x.constRealSparse<T>()),
		   Transpose(x.constImagSparse<T>()));
  } else {
    if (x.allReal())
      return Array(Transpose(x.constReal<T>()));
    else
      return Array(Transpose(x.constReal<T>()),
		   Transpose(x.constImag<T>()));
  }
}

#define MacroTranspose(ctype,cls) \
  case cls: return T_Transpose<ctype>(A);

Array Transpose(const Array &A) {
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by transpose operator");
    MacroExpandCasesAll(MacroTranspose);
  }
}

#undef MacroTranspose

template <typename T>
static inline Array T_Hermitian(const Array &x) {
  if (x.isScalar())
    if (x.allReal())
      return x;
    else 
      return Array(x.constRealScalar<T>(),(T)-x.constImagScalar<T>());
  if (x.isSparse()) {
    if (x.allReal())
      return Array(Transpose(x.constRealSparse<T>()));
    else
      return Array(Transpose(x.constRealSparse<T>()),
		   Negate(Transpose(x.constImagSparse<T>())));
  } else {
    if (x.allReal())
      return Array(Transpose(x.constReal<T>()));
    else
      return Array(Transpose(x.constReal<T>()),
		   Negate(Transpose(x.constImag<T>())));
  }
}

#define MacroHermitian(ctype,cls) \
  case cls: return T_Hermitian<ctype>(A);

Array Hermitian(const Array &A) {
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by transpose operator");
    MacroExpandCasesSigned(MacroHermitian);
  }
}

#undef MacroHermitian

template <typename T>
static inline Array T_Negate(const Array &x) {
  if (x.isScalar())
    if (x.allReal())
      return Array((T)(-x.constRealScalar<T>()));
    else
      return Array((T)(-x.constRealScalar<T>()),(T)(-x.constImagScalar<T>()));
  if (x.isSparse()) 
    if (x.allReal())
      return Array(Negate(x.constRealSparse<T>()));
    else
      return Array(Negate(x.constRealSparse<T>()),Negate(x.constImagSparse<T>()));
  if (x.allReal())
    return Array(Negate(x.constReal<T>()));
  else
    return Array(Negate(x.constReal<T>()),Negate(x.constImag<T>()));
}

#define MacroNegate(ctype,cls) \
  case cls: return T_Negate<ctype>(A);

Array Negate(const Array &A) {
  switch (A.dataClass()) {
  default:
    throw Exception("Type not supported by negate operator");
    MacroExpandCasesSigned(MacroNegate);
  }
}

#undef MacroNegate

BasicArray<Array> ArrayVectorToBasicArray(const ArrayVector& a) {
  BasicArray<Array> retvec(NTuple(a.size(),1));
  for (int i=0;i<a.size();i++) 
    retvec.set(index_t(i+1),a.at(i));
  return retvec;
}

