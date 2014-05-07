#ifndef __FMArray_hpp__
#define __FMArray_hpp__

#include "NTuple.hpp"
#include "BasicArray.hpp"

namespace FM
{
  

  // Suppose the Array was actually an object itself.  Then you would have something like

  //  Array --> NumericArray --> 

  // What are the methods required on an array
  //
  // And
  // Colon

  //  Operation          Method to Define       Description
  //    a + b            plus(a,b)              Binary addition
  //    a - b            minus(a,b)             Binary subtraction
  //    -a               uminus(a)              Unary minus
  //    +a               uplus(a)               Unary plus
  //    a.*b             times(a,b)             Element-wise multiplication
  //    a*b              mtimes(a,b)            Matrix multiplication
  //    a./b             rdivide(a,b)           Right element-wise division
  //    a.\b             ldivide(a,b)           Left element-wise division
  //    a/b              mrdivide(a,b)          Matrix right division
  //    a\b              mldivide(a,b)          Matrix left division
  //    a.^b             power(a,b)             Element-wise power
  //    a^b              mpower(a,b)            Matrix power
  //    a < b            lt(a,b)                Less than
  //    a > b            gt(a,b)                Greater than
  //    a <= b           le(a,b)                Less than or equal to
  //    a >= b           ge(a,b)                Greater than or equal to
  //    a ~= b           ne(a,b)                Not equal to
  //    a == b           eq(a,b)                Equality
  //    a & b            and(a,b)               Logical AND
  //    a | b            or(a,b)                Logical OR
  //    ~a               not(a)                 Logical NOT
  //    a:d:b            colon(a,d,b)
  //    a:b              colon(a,b)             Colon operator                 
  //    a'               ctranspose(a)          Complex conjugate transpose                 
  //    a.'              transpose(a)           Matrix transpose                 
  //    display(a)       Display method         command window output
  //    [a b]            horzcat(a,b,...)       Horizontal concatenation
  //    [a; b]           vertcat(a,b,...)       Vertical concatenation
  //    a(s1,s2,...sn)   subsref(a,s)           Subscripted reference                 
  //    a(s1,...,sn) = b subsasgn(a,s,b)        Subscripted assignment 
  //    b(a)             subsindex(a)           Subscript index                 


  // So the base object is not an array... it's a class.  Classes can even be 
  // passed by reference.  Not necessarily value.  That means that the copy-on-write
  // optimization goes where?
  //
  // Make the core object an FMObject - FMObjects are immutable
  //
  // FMObject
  //   const FMObject & add(const FMObject & other)
  // 
  // THe PyObject 
  //
  //
  // FMObjectWrapper
  //   void *_ptr;
  //   unsigned _refcnt;
  //  

  // Suppose we have 
  //
  // FMObject -> has a pointer to TypeObject
  //
  // TypeObject 
  //  - has details on the class
  //  - has non-static methods for the various operations...
  //  - these are virtual methods - subclasses must implement them
  //  const FMObject& add(const FMObject& first, const FMObject& second) = 0;
  //  etc...
  //
  // So, for example:
  //
  // TypeDouble
  //   const FMObject& add(const FMObject& first, const FMObject& second);
  //
  // TypeFloat
  //   const FMObject& add(const FMObject& first, const FMObject& second);
  //
  // TypeInt
  //   const FMObject& add(const FMObject& first, const FMObject& second);
  //
  // Each instance of FMObject holds a pointer to the base TypeObject
  //
  // class FMObject {
  //   TypeObject* _type;
  //   TypeSpecificDataMembers...
  // }
  //
  // The reason to have a base type object is because we cannot guarantee that
  // a given object participates in a function call, or even survives it.
  //
  // For example,
  //
  //   c = a + b
  //
  // will call add(a,b).  If B is of a dominant class over A (e.g, suppose B
  // is complex, and A is real), then we will call the complex add.  Without
  // a type baseclass, it will not make any sense (B->add(A)??).
  //
  //
  //


#define METHOD_UNDEFINED(x) throw Exception("Method " + x + " undefined for class " + m_name);

  typedef const FMObject & (*MethodFunc)(const FMObjectList &);

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

  // DotOp behavior:
  //
  // Sparse, Sparse --> Op(Sparse,Sparse)
  // 
  //
  // The python model seems a bit primitive.  Considering all of the cases, such as
  //
  //  Sparse, Scalar
  //  Scalar, Sparse
  // 
  // How does it decide?
  //
  // A lookup by name would work, but it is very expensive.
  // 


  // Class structure
  //  Scalar (double)
  //  ComplexScalar (double)
  //  DenseArray (Any)
  //  ComplexDenseArray (Any)
  //  SparseArray (Any)
  //  ComplexSparseArray (Any)
  //

  // Can simplify by making complex a flag for each type
  //  Scalar (double/complex)
  //  DenseArray (Any/complex<Any>)
  //  SparseArray (Any/complex<Any>)


  // Why do all objects have to be refcounted?  Can't that be decided at the object level.
  // For example, can't 

  //  FMObject be a pointer to FMObjectData

  class FMObjectData {
    FMTypeObject *m_type;
    unsigned m_refcnt;
    void *m_data;
  };


  // A slice object would be nice.  Right now, that is built into the dense array case.


  // The problem with a separate scalar class is that you must take care of all the corner cases
  // For example, an array of size 1 and a scalar are not the same thing.  So if I take something
  // like 
  //   a = int8([1,2,3,4,5])
  //  a(1):a(end) must convert a(1) into a scalar.  That's not a strict downside, but it requires
  // some additional work.  

  enum DataClass {
    Invalid = 0,
    CellArray = 1,
    Struct = 2,
    StringArray = 3,
    NumericArray = 4,
    SliceArray = 5
  };

  enum DataType {
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
    Index = 15
  };

  template <typename V>
  inline DataClass GetDataClass(V c);

  template <>
  inline DataClass GetDataClass(double) {
    return Double;
  }

  inline size_t ByteSizeOfDataClass(DataClass t)
  {
    switch (t)
      {
      case Invalid:
	return 0;
      case CellArray:
	return sizeof(void*);
      case Struct:
	return sizeof(void*);
      case StringArray:
	return sizeof(char);
      case Bool:
	return sizeof(bool);
      case Int8:
      case UInt8:
	return sizeof(uint8_t);
      case Int16:
      case UInt16:
	return sizeof(uint16_t);
      case Int32:
      case UInt32:
	return sizeof(uint32_t);
      case Int64:
      case UInt64:
	return sizeof(uint64_t);
      case Float:
	return sizeof(float);
      case Double:
	return sizeof(double);
      case Index:
	return sizeof(size_t);
      }
    return 0;
  }

  class ArrayData;

  class ArrayData** _scalar_pool;
  class ArrayData** _empty_pool;
  static int _scalar_pool_fill = 0;
  static int _empty_pool_fill = 0;

  static int _allocate_count = 0;
  static int _buildscalar_count = 0;
  static int _duplicate_count = 0;
  static int _copy_count = 0;
  static bool _use_pool = true;


  // This is a constructor function

  class ArrayDataPool;

  // These can be made per-thread
  typedef ArrayData* (*ArrayMakerFunc)(ArrayDataPool*);

  class ArrayData
  {
  public:
    DataClass _class;
    void *_real;
    void *_imag;
    NTuple _dims;
    size_t _length;
    size_t _offset;
    unsigned _refcnt;
    ArrayDataPool *_pool;

    ~ArrayData()
    {
      if (_real) free(_real);
      if (_imag) free(_imag);
    }

    ArrayData(){ }

    ArrayData(const ArrayData &orig) : _class(orig._class),
				       _dims(orig._dims),
				       _length(orig._length),
				       _offset(orig._offset)
    {
      size_t bytes = orig._length*ByteSizeOfDataClass(orig._class);
      if (orig._real)
	{
	  _real = malloc(bytes);
	  memcpy(_real,orig._real,bytes);
	}
      if (orig._imag)
	{
	  _imag = malloc(bytes);
	  memcpy(_imag,orig._imag,bytes);
	}
    }

  };


  class ArrayDataPool
  {
    ArrayData** _pool;
    int _pool_fill;
    int _max_fill;
    ArrayMakerFunc _maker;

    void fillPool()
    {
      while (_pool_fill < _max_fill)
	_pool[_pool_fill++] = _maker(this);
      printf("Fill!\n");
    }
    void drainPool()
    {
      while (_pool_fill > 0)
	delete _pool[--_pool_fill];
    }
  public:
    ArrayDataPool(int poolsize, ArrayMakerFunc maker)
    {
      _pool = (ArrayData**) malloc(sizeof(ArrayData*)*poolsize);
      _pool_fill = 0;
      _max_fill = poolsize;
      _maker = maker;
      fillPool();
    }
    ArrayData* pop()
    {
      if (_pool_fill == 0)
	fillPool();
      return _pool[--_pool_fill];
    }
    void push(ArrayData* p)
    {
      if (_pool_fill == _max_fill)
	delete p;
      p->_refcnt = 0;
      _pool[_pool_fill++] = p;
    }
  };


  class Array
  {
    ArrayData* _baseptr;
  public:
    inline Array() : _baseptr(0) {}
    inline Array(ArrayData *pData) : _baseptr(pData)
    {
      ++_baseptr->_refcnt;
    }
    inline Array(const Array& copy) : _baseptr(copy._baseptr)
    {
      _copy_count++;
      if (_baseptr) ++_baseptr->_refcnt;
    }
    inline ~Array()
    {
      if (_baseptr && ((--_baseptr->_refcnt) == 0))
	{
	  if (!_baseptr->_pool)
	    delete _baseptr;
	  else
	    _baseptr->_pool->push(_baseptr);
	}
    }
    inline Array& operator=(const Array& sp)
    {
      if (this != &sp)
	{
	  _copy_count++;
	  if (_baseptr && (--_baseptr->_refcnt) == 0)
	    {
	      if (!_baseptr->_pool)
		delete _baseptr;
	      else
		_baseptr->_pool->push(_baseptr);
	    }
	  _baseptr = sp._baseptr;
	  if (_baseptr) ++_baseptr->_refcnt;
	}
      return *this;
    }
    inline bool operator!() const
    {
      return _baseptr == 0;
    }
    inline operator bool() const
    {
      return _baseptr != 0;
    }
    inline void ensureUnique()
    {
      if (!_baseptr) return;
      if (_baseptr->_refcnt == 1) return;
      --_baseptr->_refcnt;
      _baseptr = new ArrayData(*_baseptr);
      ++_baseptr->_refcnt;
    }
    inline ArrayData& operator*()
    {
      ensureUnique();
      return *_baseptr;
    }
    inline ArrayData* operator->()
    {
      ensureUnique();
      return _baseptr;
    }
    inline const ArrayData* operator->() const
    {
      return _baseptr;
    }
    inline const ArrayData& operator*() const
    {
      return *_baseptr;
    }
  };

  ArrayData* ScalarBuilder(ArrayDataPool *pool)
  {
    ArrayData *p = new ArrayData;
    p->_real = malloc(sizeof(double));
    //    p->_real = new BasicArray<double>(0);
    p->_dims = NTuple(1,1);
    p->_length = 1;
    p->_pool = pool;
    return p;
  }

  ArrayData* EmptyBuilder(ArrayDataPool *pool)
  {
    ArrayData *p = new ArrayData;
    p->_pool = pool;
    return p;
  }


  // Shutdown is difficult... What happens with destructing?
  class ArrayFactory
  {
    ArrayDataPool *_scalars;
    ArrayDataPool *_empties;
  public:
    ArrayFactory(int poolsize)
    {
      _scalars = new ArrayDataPool(poolsize,ScalarBuilder);
      _empties = new ArrayDataPool(poolsize,EmptyBuilder);
    }
    ~ArrayFactory()
    {
      delete _scalars;
      delete _empties;
    }
    inline Array scalar(double val)
    {
      ArrayData *p = _scalars->pop();
      //      ((BasicArray<double>*) p->_real)->setNoBoundsCheck(1,val);
      ((double*)(p->_real))[0] = val;
      p->_class = Double;
      return Array(p);
    }
    inline Array empty()
    {
      return Array(_empties->pop());
    }
  };

  inline double RealScalarValue(const Array &a)
  {
    return ((double*)(a->_real))[0];
    // return ((const BasicArray<double>*)(a->_real))->getNoBoundsCheck(1);
  }



}

#endif
