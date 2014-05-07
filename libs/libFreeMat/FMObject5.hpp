#ifndef __FMObject3_hpp__
#define __FMObject3_hpp__

#include "FMPool.hpp"

// Priority list:
// double
// complex
// single
// scomplex
// integer
// icomplex
// sparse ?

typedef unsigned ref_t;
typedef uint64_t dim_t;
typedef int64_t ndx_t;

// The ArrayBase is really a bare, reference counted data chunk.
// But unlike most reference counted objects, it doesn't know
// anything about the type of the data block it has, so it cannot
// be a true object.
struct FMData {
  ref_t refcnt;
  void *ptr;
};

class FMObject3Base;


enum DataCode {
  TypeInvalid = 0,
  TypeCellArray = 1,
  TypeStruct = 2,
  TypeStringArray = 3,
  TypeBool = 4,
  TypeInt8 = 5,
  TypeUInt8 = 6,
  TypeInt16 = 7,
  TypeUInt16 = 8,
  TypeInt32 = 9,
  TypeUInt32 = 10,
  TypeInt64 = 11,
  TypeUInt64 = 12,
  TypeSingle = 13,
  TypeDouble = 14,
  TypeIndex = 15 
};

class FMObject3;

class FMType3 {				       
public:
  virtual DataCode code() const = 0;
  virtual void destroyObject(FMObject3Base* p) = 0;
  virtual FMObject3 add(const FMObject3 &a, const FMObject3 &b) = 0;
};

template <class T>
class FMPODArrayType;

const int MAXDIMS = 8;

class FMTuple {
  //! Stubbornly refusing to allow arbitrary dimensions of
  //data.  But it can be changed easily.
  dim_t dims[MAXDIMS];
  //! The number of dimensions actually used.  Performance
  //tests indicate its better to know than to assume its
  //just MAXDIMS
  unsigned ndims;
  //! The element count described by the tuple (the product of the dimensions)
  dim_t elements;
public:
  inline dim_t rows() const {
    if (ndims == 0) return 0;
    return dims[0];
  }
  inline dim_t cols() const {
    if (ndims < 2) return 0;
    return dims[1];
  }
  inline dim_t elementCount() const {
    return elements;
  }
  inline void setRows(dim_t rows) {
    if (ndims == 0) ndims = 1;
    dims[0] = rows;
    updateElementCount();
  }
  inline void setCols(dim_t cols) {
    if (ndims < 2) ndims = 2;
    dims[1] = cols;
    updateElementCount();
  }
  inline void updateElementCount() {
    if (ndims == 0) 
      elements = 0;
    else
      {
	elements = 1;
	for (unsigned i=0;i<ndims;i++)
	  elements *= dims[i];
      }
  }
  inline bool operator==(const FMTuple &other) const {
    if (this->elementCount() != other.elementCount()) return false;
    if (this->ndims == other.ndims)
      {
	for (unsigned i=0;i<ndims;i++)
	  if (this->dims[i] != other.dims[i]) return false;
	return true;
      }
    return FMTuple::canonicalForm(*this) == FMTuple::canonicalForm(other);
  }
  static FMTuple canonicalForm(const FMTuple &other) {
    FMTuple ret(other);
    while (ret.ndims > 2 && ret.dims[ret.ndims-1] == 1) ret.ndims--;
    return ret;
  }
  // Potential performance issue!
  static FMTuple computeDotOpSize(const FMTuple &a, const FMTuple &b) {
    if ((a.elementCount() == 1) && (b.elementCount() == 1)) return a;
    if (a.elementCount() == 1) return b;
    if (b.elementCount() == 1) return a;
    if (a == b) return a;
    throw Exception("Incompatible arguments to operation");
  }
};


// Flags like complexity must be carried at this level, since the 
// type does not indicate complexity.
const int FMOBJECT_EMPTY_FLAG = 1;
const int FMOBJECT_SCALAR_FLAG = 2;
const int FMOBJECT_COMPLEX_FLAG = 4;

class FMObject3Base {
private:
  FMObject3Base& operator=(const FMObject3Base& t);
  //! The pointer to the actual data.  Must not be NULL.
  FMData *data;
  //! Reference count.  The FMObject is reference counted,
  //but also contains a reference counting pointer to the
  //underlying data.  This dual level of reference counting
  //seems inelegant, but is required by the need for slicing.
  //Slicing means that one object has an internal (read-only)
  //view into another objects data.
  ref_t refcnt;
  //! Pointer to the base metatype object that contains the 
  //method pointers for the object of interest.
  FMType3 *type;
  //! The offset into the data vector.  This is 
  //used for slicing.
  size_t offset;
  //! The dimensions of the array
  FMTuple dims;
  //! The flags for the object
  int flags;
public:
  FMObject3Base(FMData*p) : data(p) {
    data->refcnt++;
  }
  FMObject3Base(const FMObject3Base& t) {
    type = t.type;
    refcnt = 0;
    dims = t.dims;
    offset = t.offset;
    data = t.data;
    data->refcnt++;
    flags = t.flags;
  }
  friend class FMObject3;
  friend class FMType3;
  template <class T>  friend class FMPODArrayType;
  friend class FMPODType;
  friend class FMDoubleType;
  //  friend class FMPODArrayType<double>;
};

class FMObject3
{
  FMObject3Base *d;
public:
  inline FMObject3() : d(0) {}
  inline FMObject3(FMObject3Base *p) : d(p) {
    if (d) d->refcnt++;
  }
  inline FMObject3(const FMObject3 &copy) : d(copy.d) {
    if (d) d->refcnt++;
  }
  inline ~FMObject3() {
    if (d && ((--d->refcnt) == 0))
      d->type->destroyObject(d);
  }
  inline const FMTuple& dims() const {
    return d->dims;
  }
  inline dim_t rows() const {
    if (!d) return 0;
    return d->dims.rows();
  }
  inline dim_t cols() const {
    if (!d) return 0;
    return d->dims.cols();
  }
  inline bool isEmpty() const {
    if (!d) return false;
    return ((d->flags & FMOBJECT_EMPTY_FLAG) != 0);
  }
  inline bool isComplex() const {
    if (!d) return false;
    return ((d->flags & FMOBJECT_COMPLEX_FLAG) != 0);
  }
  inline bool isScalar() const {
    if (!d) return false;
    return ((d->flags & FMOBJECT_SCALAR_FLAG) != 0);
  }
  inline int flags() const {
    if (!d) return 0;
    return d->flags;
  }
  inline dim_t elementCount() const {
    return d->dims.elementCount();
  }
  FMObject3& operator=(const FMObject3& copy) {
    if (this == &copy) return *this;
    if (d && ((--d->refcnt) == 0))
      d->type->destroyObject(d);
    d = copy.d;
    d->refcnt++;
  }
  inline void detach() 
  {
    if (!d || (d && d->refcnt == 1)) return;
    d = new FMObject3Base(*d);
    d->refcnt++;
  }
  inline FMType3* type() const
  {
    return d->type;
  }
  friend class FMPODArrayType<double>;
  friend class FMPODType;
  friend class FMDoubleType;
};

template <class T>
class FMFixedPool 
{
  T* pool;
  unsigned fill;
  unsigned size;
public:
  FMFixedPool(unsigned _size) 
  {
    pool = new T[_size];
    size = _size;
    fill = 0;
  }
  ~FMFixedPool()
  {
    std::cout << "Pool tear down fill is " << fill << "\n";
    delete[] pool;
  }
  bool push(T x)
  {
    if (fill >= size) return false;
    pool[fill++] = x;
    return true;
  }
  bool isEmpty() const
  {
    return (fill == 0);
  }
  bool isFull() const
  {
    return (fill == size);
  }
  T pop()
  {
    return pool[--fill];
  }
};


// Manages N-dimensional arrays of blocks of opaque data - 
// requires a real type object to provide the type interface
// to the data.
//
// There is a small (<2%) penalty associated with elsize
// not being available at compile time.
class FMPODType : public FMType3 {
  FMFixedPool<FMObject3Base*> *pool;
  size_t elsize;
public:
  FMPODType(size_t elementSize) {
    pool = new FMFixedPool<FMObject3Base*>(1000);
    elsize = elementSize;
  }
  ~FMPODType() {
    FMFixedPool<FMObject3Base*> *q = pool;
    pool = 0;
    delete q;
  }
  virtual FMType3* typeInstance() = 0;
  virtual FMData* duplicateData(const FMObject3Base *p) const {
    FMData *q = new FMData;
    q->refcnt = 1;
    q->ptr = calloc(p->dims.elementCount(),elsize);
    const char *pd = static_cast<const char*>(p->data->ptr) + p->offset*elsize;
    memcpy(q->ptr,pd,p->dims.elementCount()*elsize);
    return q;
  }
  virtual void destroyObject(FMObject3Base* p) {
    if (pool && (p->dims.elementCount() == 1) && (p->data->refcnt == 1) && pool->push(p)) return;
    if (--p->data->refcnt == 0) {
      free(p->data->ptr);
      delete p->data;
    }
    delete p;
  }
  FMObject3 zeroArrayOfSize(const FMTuple & dims, bool isComplex) {
    dim_t elementCount = dims.elementCount();
    if ((elementCount == 1) && !isComplex) return zeroScalar();
    FMData *q = new FMData;
    q->refcnt = 0;
    if (isComplex) elementCount *= 2;
    q->ptr = calloc(elementCount,elsize);
    FMObject3Base *p = new FMObject3Base(q);
    p->type = typeInstance();
    p->refcnt = 0;
    p->dims = dims;
    p->offset = 0;
    if (isComplex) 
      p->flags = FMOBJECT_COMPLEX_FLAG;
    return FMObject3(p);    
  }
  FMObject3 makeMatrix(dim_t rows, dim_t cols) {
    FMData *q = new FMData;
    q->refcnt = 0;
    q->ptr = calloc(rows*cols,elsize);
    FMObject3Base *p = new FMObject3Base(q);
    p->type = typeInstance();
    p->refcnt = 0;
    p->dims.setRows(rows);
    p->dims.setCols(cols);
    p->offset = 0;
    return FMObject3(p);
  }
  FMObject3Base *getScalarObject() {
    if (pool->isEmpty()) {
      std::cout << "Fill\n";
      while (!pool->isFull()) {
	FMData *q = new FMData;
	q->refcnt = 0;
	q->ptr = calloc(1,elsize);
	FMObject3Base* p = new FMObject3Base(q);
	p->type = this;
	p->refcnt = 0;
	p->dims.setRows(1);
	p->dims.setCols(1);
	p->offset = 0;
	p->flags = FMOBJECT_SCALAR_FLAG;
	pool->push(p);
      }
    }
    return pool->pop();
  }
  FMObject3 zeroScalar() {
    FMObject3Base* p = getScalarObject();
    memset(p->data->ptr,0,elsize);
    return FMObject3(p);
  }
  FMObject3 makeScalar(void *data) {
    FMObject3Base* p = getScalarObject();
    memcpy(p->data->ptr,data,elsize);
    return FMObject3(p);
  }
  inline const void* readOnlyData(const FMObject3 &p) const {
    return static_cast<const void *>(static_cast<const char *>(p.d->data->ptr) + elsize*p.d->offset);
  }
  inline void* readWriteData(FMObject3 &p) const {
    p.detach();
    if (p.d->data->refcnt > 1) {
      p.d->data = duplicateData(p.d);
      p.d->offset = 0;
    }
    return static_cast<void*>(p.d->data->ptr);
  }
  FMObject3 sliceColumn(FMObject3 &p, ndx_t col) {
    // Check!
    FMObject3Base *q = new FMObject3Base(p.d->data);
    q->type = p.d->type;
    q->refcnt = 0;
    q->dims.setRows(p.d->dims.rows());
    q->dims.setCols(1);
    q->offset = (col-1)*p.d->dims.rows();
    return FMObject3(q);
  }  
};


// What we want is to have underlying types for each object.
//
// So, A --> typeA
// and B --> typeB
//
// That means that each combination of (A,B) must be handled.
// However, we can use the left-most 
//
// So Add should do something like this:
//
// if (B->type()->order() > A->type()->order())
//   B->type()->dothis(A,B)
// else
//   A->type()->dothis(A,B)
//

// For the built in objects, we can then ignore order, as all built-in objects
// have the same precedence.

const bool RealCase = false;
const bool ComplexCase = true;

template <class elem>
struct FMComplex
{
  elem r;
  elem i;
  FMComplex() : r(0), i(0) {}
  FMComplex(elem real) : r(real), i(0) {}
  template <class elem2>
  FMComplex(FMComplex<elem2> other)
  {
    r = other.r;
    i = other.i;
  }
};

template <class elem>
static FMComplex<elem> operator+(const FMComplex<elem> &a, const FMComplex<elem> &b)
{
  FMComplex<elem> c;
  c.r = a.r + b.r;
  c.i = a.i + b.i;
  return c;
}

template <class elem>
static FMComplex<elem> operator+(const elem &a, const FMComplex<elem> &b)
{
  FMComplex<elem> c;
  c.r = a + b.r;
  c.i = b.i;
  return c;
}

template <class elem>
static FMComplex<elem> operator+(const FMComplex<elem> &a, const elem &b)
{
  FMComplex<elem> c;
  c.r = a.r + b;
  c.i = a.i;
  return c;
}

struct OpAdd
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) + static_cast<vtype>(*b);
  }
};

template <class ctype, class atype, class btype, class vtype, class Op>
inline FMObject3 dispatch_binop_lev2(const FMObject3 &a, const FMObject3 &b, FMPODType *otype, bool isComplex)
{
  FMObject3 c = otype->zeroArrayOfSize(FMTuple::computeDotOpSize(a.dims(),b.dims()),isComplex);
  ctype *cptr = static_cast<ctype*>(static_cast<FMPODType*>(c.type())->readWriteData(c));
  const atype *aptr = static_cast<const atype*>(static_cast<FMPODType*>(a.type())->readOnlyData(a));
  const btype *bptr = static_cast<const btype*>(static_cast<FMPODType*>(b.type())->readOnlyData(b));
  size_t aincr = (a.isScalar() ? 0 : 1);
  size_t bincr = (b.isScalar() ? 0 : 1);
  size_t elcnt = c.elementCount();
  for (size_t i=0;i<elcnt;++i)
    Op::template func<ctype,atype,btype,vtype>(cptr+i,aptr+i*aincr,bptr+i*bincr);
  return c;
}

template <class ctype, class atype, class btype, class vtype, class Op>
static inline FMObject3 dispatch_binop(const FMObject3 &a, const FMObject3 &b, FMPODType *otype)
{
  if (!a.isComplex() && !b.isComplex())
    return dispatch_binop_lev2<ctype,atype,btype,vtype,Op>(a,b,otype,RealCase);
  else if (a.isComplex() && !b.isComplex())
    return dispatch_binop_lev2<FMComplex<ctype>,FMComplex<atype>,btype,FMComplex<vtype>,Op>(a,b,otype,ComplexCase);
  else if (!a.isComplex() && b.isComplex())
    return dispatch_binop_lev2<FMComplex<ctype>,atype,FMComplex<btype>,FMComplex<vtype>,Op>(a,b,otype,ComplexCase);
  else
    return dispatch_binop_lev2<FMComplex<ctype>,FMComplex<atype>,FMComplex<btype>,FMComplex<vtype>,Op>(a,b,otype,ComplexCase);
}

class FMNumericPODType : public FMPODType {
public:
  FMNumericPODType(size_t elsize) : FMPODType(elsize) {}
};

class FMDoubleType : public FMNumericPODType {
public:
  FMDoubleType() : FMNumericPODType(sizeof(double)) {};
  virtual DataCode code() const {return TypeDouble;}
  virtual FMType3* typeInstance() {return this;}
  template <class Op>
  inline FMObject3 binop(const FMObject3 &a, const FMObject3 &b)
  {
    if (a.isScalar() && b.isScalar() && !a.isComplex() && !b.isComplex() && b.type()->code() == TypeDouble) {
      const double * ap = static_cast<const double*>(a.d->data->ptr);
      const double * bp = static_cast<const double*>(b.d->data->ptr);
      double p;
      Op::template func<double,double,double,double>(&p,ap,bp);
      return makeScalar(&p);
    }
    FMPODType *btype = static_cast<FMPODType*>(b.type());
    switch (b.type()->code())
      {
      case TypeDouble:
	return dispatch_binop<double,double,double,double,Op>(a,b,btype);
      case TypeSingle:
	return dispatch_binop<float,double,float,float,Op>(a,b,btype);
      case TypeInt64:
	return dispatch_binop<int64_t,double,int64_t,double,Op>(a,b,btype);
      case TypeUInt64:
	return dispatch_binop<uint64_t,double,uint64_t,double,Op>(a,b,btype);
      case TypeInt32:
	return dispatch_binop<int32_t,double,int32_t,double,Op>(a,b,btype);
      case TypeUInt32:
	return dispatch_binop<uint32_t,double,uint32_t,double,Op>(a,b,btype);
      case TypeInt16:
	return dispatch_binop<int16_t,double,int16_t,double,Op>(a,b,btype);
      case TypeUInt16:
	return dispatch_binop<uint16_t,double,uint16_t,double,Op>(a,b,btype);
      case TypeInt8:
	return dispatch_binop<int8_t,double,int8_t,double,Op>(a,b,btype);
      case TypeUInt8:
	return dispatch_binop<uint8_t,double,uint8_t,double,Op>(a,b,btype);
      }
  }
  virtual FMObject3 add(const FMObject3 &a, const FMObject3 &b)
  {
    return binop<OpAdd>(a,b);
  }
};

/* To be added later...
template <class Op, int IntCode, class IntType>
FMObject3 IntegerType::binop(const FMObject3 &a, const FMObject3 &b)
{
  switch (b.type()->code())
    {
    case Double:
      return dispatch_binop<IntType,IntType,double,double,Op>(a,b,a.type());
    case IntCode:
      return dispatch_binop<IntType,IntType,IntType,double,Op>(a,b,a.type());
    default:
      throw Exception("Unsupported type combination of " + a.type()->name() + " and " + b.type()->name());
    }
}

template <class Op>
FMObject3 SingleType::binop(const FMObject3 &a, const FMObject3 &b)
{
  switch (b.type()->code())
    {
    case Double:
      return dispatch_binop<float,float,double,double,Op>(a,b,a.type());
    case Single:
      return dispatch_binop<float,float,float,float,Op>(a,b,a.type());
    default:
      throw Exception("Unsupported combination of " + a.type()->name() + " and " + b.type()->name());
    }
}
*/


#endif
