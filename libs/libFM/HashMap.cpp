#include "HashMap.hpp"
#include "StringType.hpp"
#include "Object.hpp"

using namespace FM;

std::size_t FMHashMap_hash::operator()(const Object &x) const 
{
  StringType *st = x.asType<StringType>();
  dim_t len = x.elementCount();
  size_t hash = 5381;
  const FMChar *t = st->readOnlyData(x);
  for (dim_t i=0;i<len;i++)
    hash = hash * 33 + t[i];
  return hash;
}
