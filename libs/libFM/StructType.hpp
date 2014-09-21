#ifndef __StructType_hpp__
#define __StructType_hpp__

#include "PODType.hpp"
#include "Object.hpp"
#include "AggregateType.hpp"


// This has heap allocations on it.  Not a good idea.

namespace FM
{
  struct StructData {
    Object m_fields; // List of strings that represent the field names
    Object m_data; // Array of lists, each of which has the field data in it.
  };

  struct ThreadContext;

  //  class StructType : public Type {
  class StructType : public AggregateType<StructData> {
  public:
    StructType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeStruct;}
    virtual const FMString& name() const {static FMString _name = "struct"; return _name;}
    Object empty();
    // Object makeStruct(const Tuple & dims, const FMStringList &fields);
    // Object makeScalarStruct(const FMStringList &fields) {
    //   return makeStruct(Tuple(1,1),fields);
    // }
    // inline int fieldIndex(const Object &p, const FMString &t) const {
    //   return static_cast<const StructData *>(p.d->data->ptr)->m_fields.value(t);
    // }
    // inline bool hasField(const Object &p, const FMString &t) const {
    //     const StructData * q = static_cast<const StructData *>(p.d->data->ptr);
    //   return q->m_fields.contains(t);
    // }
    // void insertField(Object &p, const FMString &t);
    void setScalar(Object &q, const FMString &field, const Object &p);
    const Object & getScalar(const Object &q, const FMString &field);
    /*    FMStringList orderedFieldList(const Object &a) {
      const StructData *fp = static_cast<const StructData *>(a.d->data->ptr);
      FMStringList ret;
      ret.resize(fp->m_fields.count());
      for (FMMap<FMString,int>::const_iterator i=fp->m_fields.constBegin();
	   i!=fp->m_fields.constEnd();++i)
	ret[i.value()] = i.key();
      return ret;
    }
    */
    int getFieldIndex(const Object &a, const Object &b);
    virtual FMString describe(const Object &a);
    virtual Object getField(const Object &a, const Object &b);
    virtual void setField(Object &a, const Object &args, const Object &b);
    virtual Object getParens(const Object &a, const Object &args);
    virtual void setParens(Object &a, const Object &args, const Object &b);
    virtual bool equals(const Object &a, const Object &b)
    {
      // FIXME - allow struct equality tests
      return false;
    }
  };
}

#endif
