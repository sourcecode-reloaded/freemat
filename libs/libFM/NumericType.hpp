#ifndef __NumericType_hpp__
#define __NumericType_hpp__

#include "PODType.hpp"

namespace FM
{

  class BaseTypes;

  template <class T, FM::DataCode codeNum>
  class NumericType : public PODType<T> {
  public:
    NumericType(BaseTypes *base, const FMString &name) : PODType<T>(base,name) {}
    virtual ~NumericType() {}
    virtual DataCode code() const {return codeNum;}
    virtual Object asIndex(const Object &a, dim_t max);
    virtual Object getBraces(const Object &a, const Object &b) {throw Exception("{} indexing unsupported for numeric arrays");}
    virtual Object getField(const Object &a, const Object &b) {throw Exception(". indexing unsupported for numeric arrays");}
  };
}


#endif
