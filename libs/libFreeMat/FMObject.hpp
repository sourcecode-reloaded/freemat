#ifndef __FMObject_hpp__
#define __FMObject_hpp__

#include <complex>

#if 0

class FMObject;
class FMObjectList;

typedef const FMObject & (*MethodFunc)(const FMObjectList &);

#define METHOD_UNDEFINED(x) throw Exception(FMString("Method ") + x + " undefined for class " + m_name);

class FMObjectPool;

class FMEnvironment {
  FMObjectPool *_scalarPool;
  FMObjectPool *_emptyPool;
public:
  const FMObject & MakeCellArray();
  const FMObject & MakeStructArray();
  template <class T>
  const FMObject & MakeNumericArray();
  template <class T>
  const FMObject & MakeSparseMatrix();
  const FMObject & MakeScalar(double t);
  const FMObject & MakeEmpty();
};

class FMType {
  FMString m_name;
  int32_t m_precedence;
  FMMap<FMString, MethodFunc> m_methods;
public:
  virtual const FMObject & Add(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("plus")};
  virtual const FMObject & Subtract(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("minus")};
  virtual const FMObject & Multiply(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("mtimes")};
  virtual const FMObject & RightDivide(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("mrdivide")};
  virtual const FMObject & LeftDivide(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("mldivide")};
  virtual const FMObject & Or(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("or")};
  virtual const FMObject & And(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("and")};
  virtual const FMObject & LessThan(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("lt")};
  virtual const FMObject & LessEquals(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("le")};
  virtual const FMObject & GreaterThan(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("gt")};
  virtual const FMObject & Equals(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("eq")};
  virtual const FMObject & NotEquals(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("neq")};
  virtual const FMObject & DotMultiply(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("times")};
  virtual const FMObject & DotRightDivide(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("rdivide")};
  virtual const FMObject & DotLeftDivide(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("ldivide")};
  virtual const FMObject & Negate(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("uminus")};
  virtual const FMObject & Plus(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("uplus")};
  virtual const FMObject & Not(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("not")};
  virtual const FMObject & Power(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("mpower")};
  virtual const FMObject & DotPower(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("power")};
  virtual const FMObject & Hermitian(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("ctranspose")};
  virtual const FMObject & Transpose(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("transpose")};
  virtual const FMObject & Colon(const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("colon")};
  virtual const FMObject & Colon(const FMObject &, const FMObject &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("colon")};
  virtual const FMObject & Display(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("display")};
  virtual const FMObject & Horzcat(const FMObjectList &, FMEnvironment *){METHOD_UNDEFINED("horzcat")};
  virtual const FMObject & Vertcat(const FMObjectList &, FMEnvironment *){METHOD_UNDEFINED("vertcat")};
  virtual const FMObject & Subsref(const FMObject &, const FMObjectList &, FMEnvironment *){METHOD_UNDEFINED("subsref")};
  virtual const FMObject & Subsasgn(const FMObject &, const FMObjectList &, const FMObject &, FMEnvironment *){METHOD_UNDEFINED("subsasgn")};
  virtual const FMObject & Subsindex(const FMObject &, FMEnvironment *){METHOD_UNDEFINED("subsindex")};
  virtual const FMObject & Duplicate(const FMObject &, FMEnvironment *)= 0;
};


// Generic operator:
//
//
// Scalar function


// Consider all the permutations for each operator.  Such as:
// <double,int32>
//
// If we were to write this one manually, we would have
//
//Add<Double,Int32>
//This should map to an inner loop
//
//


/*
double *ap;
int32_t *bp;
int32_t *cp;
size_t ap_incr = 1;
size_t bp_incr = 0;
size_t count = 512; // For example
size_t iap = 0;
size_t ibp = 0;
for (size_t i=0;i<count;i++)
  {
    cp[i] = fm_saturate<int32_t>(ap[iap] + bp[ibp]);
    iap += ap_incr;
    ibp += bp_incr;
  }


template <class A, class B, class C, class D>
void add(const A*ap, const B*bp, C*cp, size_t ap_incr, size_t bp_incr, size_t count)
{
  size_t iap = 0;
  size_t ibp = 0;
  for (size_t i=0;i<count;i++)
    {
      cp[i] = fm_saturate<C>(D(ap[iap]) + D(bp[ibp]));
      iap += ap_incr;
      ibp += bp_incr;
    }
}

FMObject DenseArrayType<double>::Add(const FMObject & a, const FMObject &b)
{
}
*/


// In the case of octave, a pointer to the underlying data and an offset are stored by the outer pointer


// Octave uses a model very similar to FM4.2
//
// octave_value
//   octave_base_value *rep
//
// rep is reference counted
//
// 

// Performance of the stashed scalar is _worse_ than the
// reference counted 


class FMObjectBase {
public:
  FMType *_type;
  uint32_t _flags;
  unsigned _refcnt;
  NTuple _dims;
  unsigned _offset;
  void *_data;
};

class FMCellArray : public FMObjectBase {
public:
  DenseArray<FMObject> _data;
};

// Struct array is an 
class FMStructArray : public FMObjectBase {
public:
  FMVector<DenseArray<FMObject> > _data;
  StringVector _fields;
  FMMap<FMString,int> _fieldMap;
};

template <class T>
class FMNumericArray : public FMObjectBase {
public:
  DenseArray<T> _data;
};

template <class T>
class FMSparseMatrix : public FMObjectBase {
public:
  SparseMatrix<T> _data;
}

typedef FMRefPtrCOW<FMObjectBase> FMObject;

// const int32_t FM_FLAG_SCALAR = 1;

// class FMType
// {
// };


#endif

class ArrayData
{
public:
  unsigned _refcnt;
};

class FMType
{
};

const int FM_FLAG_SCALAR = 1;

class FMObject {
  FMType *m_type;
  uint32_t m_flags;
  complex<double> m_scalar;
  ArrayData *m_baseptr;
public:
  inline FMObject() : m_type(0), m_flags(0), m_scalar(0,0), m_baseptr(0) {}
  inline FMObject(const FMObject& copy) : m_type(copy.m_type), m_flags(copy.m_flags)
  {
    if (m_flags & FM_FLAG_SCALAR)
      m_scalar = copy.m_scalar;
    else
      {
 	m_baseptr = copy.m_baseptr;
 	if (m_baseptr) ++m_baseptr->_refcnt;
      }
  }
  explicit inline FMObject(double t)
  {
    m_type = 0;
    m_flags = FM_FLAG_SCALAR;
    m_scalar = complex<double>(t);
  }
  explicit inline FMObject(const complex<double> &s)
  {
    m_type = 0;
    m_flags = FM_FLAG_SCALAR;
    m_scalar = s;
  }
  inline ~FMObject()
  {
    if (!(m_flags & FM_FLAG_SCALAR))
      if (m_baseptr && ((--m_baseptr->_refcnt) == 0))
 	delete m_baseptr;
  }
  inline FMObject& operator=(const FMObject& sp)
  {
    if (this != &sp)
      {
 	if (!(m_flags & FM_FLAG_SCALAR))
 	  if (m_baseptr && (--m_baseptr->_refcnt) == 0)
 	    delete m_baseptr;
 	m_type = sp.m_type;
 	m_flags = sp.m_flags;
 	if (m_flags & FM_FLAG_SCALAR)
 	  m_scalar = sp.m_scalar;
 	else
 	  {
 	    m_baseptr = sp.m_baseptr;
 	    if (m_baseptr) ++m_baseptr->_refcnt;
 	  }
      }
    return *this;
  }
  inline void ensureUnique()
  {
    if (m_flags & FM_FLAG_SCALAR) return;
    if (!m_baseptr) return;
    if (m_baseptr->_refcnt == 1) return;
    --m_baseptr->_refcnt;
    m_baseptr = new ArrayData(*m_baseptr);
    ++m_baseptr->_refcnt;
  }
  inline ArrayData& operator*()
  {
    ensureUnique();
    return *m_baseptr;
  }
  inline ArrayData* operator->()
  {
    ensureUnique();
    return m_baseptr;
  }
  inline const ArrayData* operator->() const
  {
    return m_baseptr;
  }
  inline const ArrayData& operator*() const
  {
    return *m_baseptr;
  }
  inline const complex<double> & scalarValue() const
  {
    return m_scalar;
  }
  inline complex<double>& scalarValue() 
  {
    return m_scalar;
  }
};


#endif
