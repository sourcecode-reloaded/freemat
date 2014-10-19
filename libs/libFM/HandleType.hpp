#ifndef __HandleType__
#define __HandleType__

#include "AggregateType.hpp"

namespace FM
{
  template <typename DataType>
  class HandleType : public AggregateType<DataType> {
  public:
    virtual Data* duplicateData(const ObjectBase *p, dim_t&) const
    {
      return p->data;
    }
  };
};

#endif
