#ifndef __Object_hpp__
#define __Object_hpp__

#include "Type.hpp"
#include "Tuple.hpp"

namespace FM
{

  // Priority list:
  // double
  // complex
  // single
  // scomplex
  // integer
  // icomplex
  // sparse ?


  // The ArrayBase is really a bare, reference counted data chunk.
  // But unlike most reference counted objects, it doesn't know
  // anything about the type of the data block it has, so it cannot
  // be a true object.
  struct Data {
    ref_t refcnt;
    void *ptr;
  };

  class ObjectBase;
  
  template <class T>
  class ArrayType;

  template <class T>
  class PODType;

  class ObjectArrayType;

  class StringType;


  // Flags like complexity must be carried at this level, since the 
  // type does not indicate complexity.
  const int OBJECT_COMPLEX_FLAG = 4;

  class ObjectBase {
  private:
    ObjectBase() = delete;
    ObjectBase& operator=(const ObjectBase& t) = delete;
    //! The pointer to the actual data.  Must not be NULL.
    Data *data;
    //! Reference count.  The Object is reference counted,
    //but also contains a reference counting pointer to the
    //underlying data.  This dual level of reference counting
    //seems inelegant, but is required by the need for slicing.
    //Slicing means that one object has an internal (read-only)
    //view into another objects data.
    ref_t refcnt;
    //! Garbage collection reference count - used by the reference
    //cycle garbage collector to fiddle with the reference count.
    ref_t gc_refcnt;
    //! Indicates if the object is handle-style - i.e., shared.
    bool handle;
    //! Pointer to the base metatype object that contains the 
    //method pointers for the object of interest.
    Type *type;
    //! The offset into the data vector.  This is 
    //used for slicing.
    size_t offset;
    //! The dimensions of the array
    Tuple dims;
    //! The flags for the object
    int flags;
    //! The ultimate capacity of the object.  Allows for expansion
    // without reallocating/copying
    dim_t capacity;
  public:
    ObjectBase(Data* p, Type* _type, size_t _offset, Tuple _dims, int _flags, size_t _capacity, bool _handle = false) : data(p) {
      data->refcnt++;
      type = _type;
      offset = _offset;
      dims = _dims;
      flags = _flags;
      capacity = _capacity;
      handle = _handle;
      refcnt = 0;
    }
    ObjectBase(const ObjectBase& t) {
      type = t.type;
      refcnt = 0;
      dims = t.dims;
      offset = t.offset;
      data = t.data;
      data->refcnt++;
      flags = t.flags;
      capacity = t.capacity;
      handle = t.handle;
    }
    Type* get_type() const {
      return type;
    }
    ref_t& count() {
      return refcnt;
    }
    ref_t& gc_count() {
      return gc_refcnt;
    }
    void *ptr() const {return data->ptr;}
    friend class Object;
    friend class Type;
    template <class T> friend class ArrayType;
    template <class T> friend class PODType;
    friend class ObjectArrayType;
    template <class T, bool H> friend class AggregateType;
    template <class T> friend class HandleType;
    friend class DoubleType;
    friend class SingleType;
    friend class StringType;
    friend class StructType;
    friend class ListType;
    friend class ClassType;
    friend class FunctionType;
    friend void debugTrap(FMString,ObjectBase*);
    //  friend class PODArrayType<double>;
  };


  struct ThreadContext;

  inline void debugTrap(FMString method, ObjectBase *p) {
    std::cout << method << " count " << p->count() << " ptr " << p << " class " << p->type->name() << "\n";
  }
    

  // Need to add a default constructor for Object that
  // points to a global "Empty" object (maybe with its
  // own type?).
  class Object
  {
    ObjectBase *d;
  public:
    inline Object(ThreadContext *ctxt);
    inline Object(ObjectBase *p) : d(p) {
      assert(d);
      d->refcnt++;
      //      if (isClass()) debugTrap("Create",d);
    }
    inline Object(const Object &copy) : d(copy.d) {
      d->refcnt++;
      //      if (isClass()) debugTrap("Copy",d);
    }
    inline ~Object() {
      bool isc = isClass();
      if ((--d->refcnt) == 0) {
	d->type->destroyObject(d);
	// if (isc) debugTrap("Delete",d);
	d = 0;
      }
    }
    // Don't use this function!! It is only to be used by
    // the destroyObject function in the ClassType.  It is
    // used to substitute an ObjectBase into a sacrificial
    // object.  That's the only good use for it!
    inline ObjectBase* swap(ObjectBase *t) {
      ObjectBase *q = d;
      d = t;
      d->refcnt++;
      --q->refcnt;
      return q;
    }
    // FIXME - this shouldn't be available
    inline ObjectBase* raw() const {
      return d;
    }
    inline const Tuple& dims() const {
      return d->dims;
    }
    inline Tuple& dims() {
      detach();
      return d->dims;
    }
    inline dim_t rows() const {
      return d->dims.rows();
    }
    inline dim_t cols() const {
      return d->dims.cols();
    }
    inline dim_t columns() const {
      return d->dims.cols();
    }
    inline bool isEmpty() const {
      return (count() == 0);
    }
    inline bool isComplex() const {
      return ((d->flags & OBJECT_COMPLEX_FLAG) != 0);
    }
    inline bool isScalar() const {
      return (count() == 1);
    }
    inline bool isList() const {
      return (typeCode() == TypeListArray);
    }
    inline bool isString() const {
      return (typeCode() == TypeString);
    }
    inline bool isClass() const {
      return (typeCode() == TypeClass);
    }
    inline bool is2D() const {
      return (d->dims.is2D());
    }
    inline bool isSquare() const {
      return is2D() && (rows() == columns());
    }
    inline bool isVector() const {
      return (d->dims.isVector());
    }
    inline int flags() const {
      return d->flags;
    }
    inline size_t capacity() const {
      return d->capacity;
    }
    inline FMString brief() const {
      return d->type->brief(*this);
    }
    inline FMString description() const {
      return d->type->describe(*this);
    }
    inline dim_t count() const {
      return d->dims.count();
    }
    inline Object asIndex(dim_t max) const {
      return d->type->asIndex(*this,max);
    }
    inline Object asIndexNoBoundsCheck() const {
      return d->type->asIndexNoBoundsCheck(*this);
    }
    inline double asDouble() const {
      return d->type->doubleValue(*this);
    }
    inline Object exportTo(ThreadContext *dst) const;
    inline ThreadContext* context() const {
      return this->type()->context();
    }
    Object& operator=(const Object& copy) {
      if (this == &copy) return *this;
      if ((--d->refcnt) == 0)
	d->type->destroyObject(d);
      //      if (isClass()) debugTrap("Assign_old",d);
      d = copy.d;
      assert(d);
      d->refcnt++;
      //      if (isClass()) debugTrap("Assign_new",d);
      return *this;
    }
    inline void detach() 
    {
      //      if (isClass()) debugTrap("Detach",d);
      if (!d->handle) {
	if (d->refcnt > 1) {
	  d = new ObjectBase(*d);
	  d->refcnt++;
	}
	if (d->data->refcnt > 1) {
	  d->data = d->type->duplicateData(d,d->capacity);
	  d->offset = 0;
	}
      }
    }
    inline Type* type() const
    {
      return d->type;
    }
    inline DataCode typeCode() const
    {
      return type()->code();
    }
    inline bool is(DataCode a) const
    {
      return typeCode() == a;
    }
    template <class S>
    inline S* asType() const
    {
      return reinterpret_cast<S*>(d->type);
    }
    inline int refcnt() const {
      return d->refcnt;
    }
    template <class T> friend class ArrayType;
    friend class PODType<double>;
    template <class T> friend class PODType;
    friend class ObjectArrayType;
    template <class T, bool H> friend class AggregateType;
    template <class T> friend class HandleType;
    friend class DoubleType;
    friend class SingleType;
    friend class IndexType;
    friend class Int32Type;
    friend class BoolType;
    friend class StringType;
    friend class StructType;
    friend class ListType;
    friend class VisitAncestors;

    bool operator==(const Object& b) const {
      if (type()->code() != b.type()->code()) return false;
      return type()->equals(*this,b);
    }
  };

  inline std::ostream& operator<<(std::ostream &o, const Object &a) {
    o << a.description();
    return o;
  }

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

  class ObjectVector : public FMVector<Object> {};


  /* To be added later...
     template <class Op, int IntCode, class IntType>
     Object IntegerType::binop(const Object &a, const Object &b)
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
     Object SingleType::binop(const Object &a, const Object &b)
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
}


#include "ThreadContext.hpp"

inline FM::Object::Object(FM::ThreadContext *ctxt) : d(ctxt->_empty) 
{
  d->refcnt++;
}

inline FM::Object FM::Object::exportTo(FM::ThreadContext *dst) const
{
  return GetTypeForCode(dst,this->typeCode())->import(*this);
}

#endif
