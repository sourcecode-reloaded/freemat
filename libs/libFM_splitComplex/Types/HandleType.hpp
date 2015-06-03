#ifndef __HandleType__
#define __HandleType__

#include "AggregateType.hpp"

namespace FM
{
  template <typename DataType>
  class SHandleType : public AggregateType<DataType,HandleSemantics> {
  public:
    virtual Data* duplicateData(const ObjectBase *p, ndx_t&) const
    {
      p->data->refcnt++;
      return p->data;
    }
  };
};

#endif
