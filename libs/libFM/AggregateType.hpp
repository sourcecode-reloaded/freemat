#ifndef __AggregateType__
#define __AggregateType__

namespace FM
{
  // An AggregateType is a type where the data for the type consists
  // of automatically-managed memory objects.  For example, Objects
  // or STL containers.  In these cases, the copy and delete functions
  // can be simplified by using default C++ copy constructors and 
  // destructors.
  template <typename DataType>
  class AggregateType : public Type {
  public:
    virtual void destroyObject(ObjectBase* p)
    {
      if (--p->data->refcnt == 0)
	{
	  DataType *k = static_cast<DataType *>(p->data->ptr);
	  delete k;
	}
      delete p->data;
    }
    virtual Data* duplicateData(const ObjectBase *p, dim_t &reserve) const
    {
      Data *q = new Data;
      q->refcnt = 1;
      dim_t elem_count = p->dims.elementCount();
      const DataType *pdata = static_cast<const DataType *>(p->data->ptr);
      DataType *copy = new DataType(*pdata);
      q->ptr = copy;
      reserve = elem_count; // FIXME
      return q;
    }
    const DataType *readOnlyData(const Object &p) const {
      return static_cast<const DataType *>(p.d->data->ptr);
    }
    DataType * readWriteData(Object &p) const {
      p.detach();
      return static_cast<DataType *>(p.d->data->ptr);
    }
  };
}

#endif
