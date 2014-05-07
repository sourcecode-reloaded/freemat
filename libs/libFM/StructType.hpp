#ifndef __StructType_hpp__
#define __StructType_hpp__

#include "PODType.hpp"
#include "Object.hpp"

namespace FM
{
  struct StructData {
    FMMap<FMString,int> m_fields;
    FMVector<Object> *m_data;
  };

  class StructType : public Type {
  public:
    StructType(BaseTypes *base) {_base = base;}
    virtual DataCode code() const {return TypeStruct;}
    virtual const FMString& name() const {static FMString _name = "struct"; return _name;}
    virtual void destroyObject(ObjectBase* p) 
    {
      if (--p->data->refcnt == 0)
	{
	  StructData *k = static_cast<StructData *>(p->data->ptr);
	  delete [] k->m_data;
	  delete k;
	}
      delete p->data;
    }
    virtual Data* duplicateData(const ObjectBase* p) const
    {
      Data *q = new Data;
      q->refcnt = 1;
      dim_t elem_count = p->dims.elementCount();
      const StructData *pdata = static_cast<const StructData *>(p->data->ptr);
      StructData *copy = new StructData;
      copy->m_fields = pdata->m_fields;
      copy->m_data = new FMVector<Object>[elem_count];
      for (dim_t i=0;i<elem_count;i++)
	copy->m_data[i] = pdata->m_data[i+p->offset];
      q->ptr = copy;
      return q;
    }
    Object makeScalarStruct(const FMStringList &fields) {
      Object p = makeStruct(Tuple(1,1),fields);
      p.d->flags |= OBJECT_SCALAR_FLAG;
      return p;
    }
    Object makeStruct(const Tuple & dims, const FMStringList &fields) {
      dim_t elementCount = dims.elementCount();
      Data *q = new Data;
      q->refcnt = 0;
      StructData *sd = new StructData;
      int fieldnum = fields.size();
      for (int i=0;i<fieldnum;i++)
	sd->m_fields[fields[i]] = i;
      sd->m_data = new FMVector<Object>[elementCount];
      for (dim_t i=0;i<elementCount;i++)
	sd->m_data[i].resize(fieldnum);
      q->ptr = sd;
      ObjectBase *p = new ObjectBase(q);
      p->type = this;
      p->refcnt = 0;
      p->dims = dims;
      p->offset = 0;
      return Object(p);      
    }
    inline int fieldIndex(const Object &p, const FMString &t) const {
      return static_cast<const StructData *>(p.d->data->ptr)->m_fields.value(t);
    }
    inline const FMVector<Object>* readOnlyData(const Object &p) const {
      const StructData *fp = static_cast<const StructData *>(p.d->data->ptr);
      return fp->m_data + p.d->offset;
    }
    inline FMVector<Object>* readWriteData(Object &p) const {
      p.detach();
      const StructData *fp = static_cast<const StructData *>(p.d->data->ptr);
      return fp->m_data;
    }
    void setScalar(Object &q, const FMString &field, const Object &p) {
      assert(q.isScalar());
      int n = this->fieldIndex(q,field);
      FMVector<Object> *data = this->readWriteData(q);
      data[0][n] = p;
    }
    const Object & getScalar(const Object &q, const FMString &field) {
      assert(q.isScalar());
      int n = this->fieldIndex(q,field);
      const FMVector<Object> *data = this->readOnlyData(q);
      return data[0][n];
    }
    FMStringList orderedFieldList(const Object &a) {
      const StructData *fp = static_cast<const StructData *>(a.d->data->ptr);
      FMStringList ret;
      ret.resize(fp->m_fields.count());
      for (FMMap<FMString,int>::const_iterator i=fp->m_fields.constBegin();
	   i!=fp->m_fields.constEnd();++i)
	ret[i.value()] = i.key();
      return ret;
    }
    virtual FMString describe(const Object &a)
    {
      if (a.isScalar()) {
	FMString ret = a.dims().toString() + " struct array with fields:\n";
	FMStringList fields(this->orderedFieldList(a));
	const FMVector<Object> *adata = this->readOnlyData(a);
	for (int i=0;i<fields.size();i++)
	  ret += "   " + fields[i] + ": " + adata[0][fieldIndex(a,fields[i])].description() + "\n";
	return ret;
      } else {
	FMString ret = a.dims().toString() + " struct array with fields:\n";
	FMStringList fields(this->orderedFieldList(a));
	for (int i=0;i<fields.size();i++)
	  ret += "   " + fields[i] + "\n";
	return ret;
      }
    }
    virtual bool equals(const Object &a, const Object &b)
    {
    }
    virtual Object asIndex(const Object &, dim_t) {
      throw Exception("Cell arrays cannot be used as index arrays");
    }
  };
}

#endif
