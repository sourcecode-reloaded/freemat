#ifndef __QSharedData_hpp__
#define __QSharedData_hpp__


// Provide a surrogate for the shared data concept.
class QSharedData {
  int _refcount;
public:
  QSharedData() : _refcount(0) {}
  virtual ~QSharedData() {}
};

template class <T>
class QSharedDataPointer {
  T* _baseptr;
public:
  QShareDataPointer
};
