#ifndef __FMArray_hpp__
#define __FMArray_hpp__

#include "FMRefPtr.hpp"
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




  // enum DataClass {
  //   Invalid = 0,
  //   CellArray = 1,
  //   Struct = 2,
  //   StringArray = 3,
  //   NumericArray = 4,
  //   SliceArray = 5
  // };

  enum DataClass {
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

    static void release(ArrayData *p);
    ArrayData* duplicate(ArrayData *p);

    ArrayData(){ }

    ArrayData(const ArrayData &orig) : _class(orig._class),
				       _dims(orig._dims),
				       _length(orig._length),
				       _offset(orig._offset),_refcnt(0)
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

  inline void ArrayData::release(ArrayData *p) {
    if (p->_pool)
      p->_pool->push(p);
  }


  inline ArrayData* ArrayData::duplicate(ArrayData *p) {
    return new ArrayData(*p);
  }


  typedef FMRefPtrCOW<ArrayData> Array;

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
