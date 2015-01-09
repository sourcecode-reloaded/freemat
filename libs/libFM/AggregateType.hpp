#ifndef __AggregateType__
#define __AggregateType__

#include "Object.hpp"
#include "Type.hpp"

namespace FM
{
  const bool ValueSemantics = false;
  const bool HandleSemantics = true;

  // An AggregateType is a type where the data for the type consists
  // of automatically-managed memory objects.  For example, Objects
  // or STL containers.  In these cases, the copy and delete functions
  // can be simplified by using default C++ copy constructors and 
  // destructors.
  template <typename DataType, bool HandleStyle>
  class AggregateType : public Type {
  public:
    virtual void destroyObject(ObjectBase* p)
    {
      if (HandleStyle) std::cout << "Destroy object called on object of type " + p->type->name() + "\n";
      if (--p->data->refcnt == 0)
	{
	  DataType *k = static_cast<DataType *>(p->data->ptr);
	  delete k;
	}
      delete p->data;
    }
    bool equals(const Object &a, const Object &b) {
      return this->ro(a) == this->ro(b);
    }
    virtual Data* duplicateData(const ObjectBase *p, dim_t &reserve) const
    {
      Data *q = new Data;
      q->refcnt = 1;
      dim_t elem_count = p->dims.count();
      const DataType *pdata = static_cast<const DataType *>(p->data->ptr);
      DataType *copy = new DataType(*pdata);
      q->ptr = copy;
      reserve = elem_count; // FIXME
      return q;
    }
    const DataType *ro(const Object &p) const {
      assert(p.type()->code() == this->code());
      return static_cast<const DataType *>(p.d->data->ptr);
    }
    DataType * rw(Object &p) const {
      assert(p.type()->code() == this->code());
      p.detach();
      return static_cast<DataType *>(p.d->data->ptr);
    }
    Object empty() {
      Data *q = new Data;
      q->refcnt = 0;
      q->ptr = new DataType(_ctxt);
      return Object(new ObjectBase(q,this,0,Tuple(0,0),0,0,HandleStyle));
    }
    Object makeScalar() {
      Data *q = new Data;
      q->refcnt = 0;
      q->ptr = new DataType(_ctxt);
      return Object(new ObjectBase(q,this,0,Tuple(1,1),0,0,HandleStyle));
    }
  };
}

#endif
