#include "ObjectArrayType.hpp"

using namespace FM;

const Object* containedObjects(const ObjectBase *p, dim_t &count) const {
  count = p->dims.count();
  return (static_cast<const Object *>(p->data->ptr) + p->offset); 
}

void* ObjectArrayType::allocateArray(dim_t size) const {
  void *p = malloc(sizeof(Object)*size);
  Object *t = static_cast<Object *>(p);
  Object mt = Object(_ctxt);
  for (dim_t i=0;i<size;i++)
    new (&t[i]) Object(mt);  // Hmm?
  return p;
}

void ObjectArrayType::releaseData(Object* dst, dim_t size) const {
  Object mt = Object(_ctxt);
  for (dim_t i=0;i<size;i++)
    dst[i] = mt;
}

void ObjectArrayType::freeData(Object* ptr, dim_t size) const {
  for (dim_t i=0;i<size;i++)
    (ptr+i)->~Object();
  free(ptr);
}

Object ObjectArrayType::zeroElement() const {
  return Object(_ctxt);
}
