#ifndef __VariantList_hpp__
#define __VariantList_hpp__

#include "Variant.hpp"
#include "FastList.hpp"

class VariantList : public FastList<Variant> {
public:
  VariantList() {}
};


#endif
