#ifndef __FMValue_hpp__
#define __FMValue_hpp__

#include <complex>

template <class T>
struct Complex
{
  T r;
  T i;
};

class RefCounted
{
public:
  unsigned _ref;
};

const int type_null = 0;
const int type_double = 1;

class FMValue {
  struct Private
  {
    union Data
    {
      Complex<double> z;
      Complex<float> c;
      double d;
      float f;
      RefCounted *shared;
    } data;
    uint type : 30;
    uint is_shared : 1;
    uint is_null : 1;
  };
  Private d;
public:
  inline FMValue() {}
  FMValue(double t) 
  {
    d.is_null = false;
    d.type = type_double;
    d.data.d = t;
  }
  FMValue(const FMValue & p) : d(p.d)
  {
    if (d.is_shared)
      ++d.data.shared->_ref;
  }
  ~FMValue()
  {
    if (d.is_shared && !(--d.data.shared->_ref))
      delete d.data.shared;
  }
  FMValue& operator=(const FMValue &value)
  {
    if (this == &value) return *this;
    if (d.is_shared && !(--d.data.shared->_ref))
      delete d.data.shared;
    d = value.d;
    if (d.is_shared)
      ++d.data.shared;
    return *this;
  }
  double doubleValue() const {
    return d.data.d;
  }
};

#endif
