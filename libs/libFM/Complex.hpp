#ifndef __Complex_hpp__
#define __Complex_hpp__

namespace FM
{

  template <class elem>
  struct Complex
  {
    elem r;
    elem i;
    Complex() : r(0), i(0) {}
    Complex(elem real) : r(real), i(0) {}
    template <class elem2>
    Complex(Complex<elem2> other)
    {
      r = other.r;
      i = other.i;
    }
  };

  template <class elem>
  static Complex<elem> operator+(const Complex<elem> &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    c.r = a.r + b.r;
    c.i = a.i + b.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator+(const elem &a, const Complex<elem> &b)
  {
    Complex<elem> c;
    c.r = a + b.r;
    c.i = b.i;
    return c;
  }

  template <class elem>
  static Complex<elem> operator+(const Complex<elem> &a, const elem &b)
  {
    Complex<elem> c;
    c.r = a.r + b;
    c.i = a.i;
    return c;
  }
}


#endif
