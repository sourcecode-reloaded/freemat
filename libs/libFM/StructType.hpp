#ifndef __StructType_hpp__
#define __StructType_hpp__

#include "PODType.hpp"
#include "Object.hpp"
#include "AggregateType.hpp"
#include "HashMap.hpp"


// This has heap allocations on it.  Not a good idea.

namespace FM
{
  // Struct is defined as a cell array of lists.  Each list entry corresponds
  // to the data element for a given field.  So it is effectively:
  //  {<f1,f2,f3>,<f1,f2,f3>,...}
  // where <f1,f2,f3> is a list of values for fields 1, 2 and 3 respectively.
  struct StructData {
    HashMap<int> m_fields; // Maps fields to index in the list for each cell
    Object m_data; // Cell array where each entry is a list of fields
  };

  struct ThreadContext;

  //  class StructType : public Type {
  class StructType : public AggregateType<StructData> {
    void addNewFields(Object &a, const Object &fields);
    void mergeFields(Object &a, const Object &b);
    void reorderFields(Object &a, const Object &b);
    void fillEmpties(Object &a);
  public:
    StructType(ThreadContext *ctxt) {_ctxt = ctxt;}
    virtual DataCode code() const {return TypeStruct;}
    virtual const FMString& name() const {static FMString _name = "struct"; return _name;}
    Object empty();
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
