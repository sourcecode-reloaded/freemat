#include "ObjectArrayType.hpp"

using namespace FM;

void ObjectArrayType::visitContainedObjects(const ObjectBase *p, ObjectVisitor &visitor) const {
  const Object * base = static_cast<const Object *>(p->data->ptr) + p->offset;
  for (ndx_t n=0;n<p->dims.count();n++)
    visitor(base[n]);
}

void* ObjectArrayType::allocateArray(ndx_t size) const {
  void *p = malloc(size_t(sizeof(Object)*size));
  Object *t = static_cast<Object *>(p);
  Object mt = Object(_ctxt);
  for (ndx_t i=0;i<size;i++)
    new (&t[i]) Object(mt);  // Hmm?
  return p;
}

void ObjectArrayType::releaseData(Object* dst, ndx_t size) const {
  Object mt = Object(_ctxt);
  for (ndx_t i=0;i<size;i++)
    dst[i] = mt;
}

void ObjectArrayType::freeData(Object* ptr, ndx_t size) const {
  for (ndx_t i=0;i<size;i++)
    (ptr+i)->~Object();
  free(ptr);
}

Object ObjectArrayType::zeroElement() const {
  return Object(_ctxt);
}
