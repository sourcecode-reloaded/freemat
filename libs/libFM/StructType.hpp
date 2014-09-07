#ifndef __StructType_hpp__
#define __StructType_hpp__

#include "PODType.hpp"
#include "Object.hpp"
#include "AggregateType.hpp"

namespace FM
{
  struct StructData {
    FMMap<FMString,int> m_fields;
    Object m_data; // Data is stored in a cell array.  Row dimension corresponds to field index.
  };

  struct ThreadContext;

  //  class StructType : public Type {
  class StructType : public AggregateType<StructData> {
  public:
    StructType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeStruct;}
    virtual const FMString& name() const {static FMString _name = "struct"; return _name;}
    // virtual void destroyObject(ObjectBase* p) 
    // {
    //   if (--p->data->refcnt == 0)
    // 	{
    // 	  StructData *k = static_cast<StructData *>(p->data->ptr);
    // 	  delete k;
    // 	}
    //   delete p->data;
    // }
    // virtual Data* duplicateData(const ObjectBase* p, dim_t &reserve) const
    // {
    //   Data *q = new Data;
    //   q->refcnt = 1;
    //   dim_t elem_count = p->dims.elementCount();
    //   const StructData *pdata = static_cast<const StructData *>(p->data->ptr);
    //   StructData *copy = new StructData(*pdata);
    //   q->ptr = copy;
    //   reserve = elem_count; // FIXME
    //   return q;
    // }
    Object makeStruct(const Tuple & dims, const FMStringList &fields);
      Object makeScalarStruct(const FMStringList &fields) {
          return makeStruct(Tuple(1,1),fields);
      }
    inline int fieldIndex(const Object &p, const FMString &t) const {
      return static_cast<const StructData *>(p.d->data->ptr)->m_fields.value(t);
    }
    inline bool hasField(const Object &p, const FMString &t) const {
        const StructData * q = static_cast<const StructData *>(p.d->data->ptr);
      return q->m_fields.contains(t);
    }
    void insertField(Object &p, const FMString &t);
    const Object* readOnlyData(const Object &p) const;
    Object* readWriteData(Object &p) const;
    void setScalar(Object &q, const FMString &field, const Object &p);
    const Object & getScalar(const Object &q, const FMString &field);
    FMStringList orderedFieldList(const Object &a) {
      const StructData *fp = static_cast<const StructData *>(a.d->data->ptr);
      FMStringList ret;
      ret.resize(fp->m_fields.count());
      for (FMMap<FMString,int>::const_iterator i=fp->m_fields.constBegin();
	   i!=fp->m_fields.constEnd();++i)
	ret[i.value()] = i.key();
      return ret;
    }
    virtual FMString describe(const Object &a);
    virtual bool equals(const Object &a, const Object &b)
    {
      // FIXME - allow struct equality tests
      return false;
    }
    virtual Object asIndex(const Object &, dim_t) {
      throw Exception("struct arrays cannot be used as index arrays");
    }
  };
}

#endif
