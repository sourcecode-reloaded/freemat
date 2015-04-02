#ifndef __Operators_hpp__
#define __Operators_hpp__

#include "Convert.hpp"
#include "Powfunc.hpp"

struct OpAdd
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    FM::OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(*a) + static_cast<vtype>(*b));
  }
};

struct OpSubtract
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    FM::OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(*a) - static_cast<vtype>(*b));
  }
};

struct OpDotPower
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    FM::OpRangeConvert<ctype,vtype>::rangeConvert(*c,FM::powfunc(static_cast<vtype>(*a),static_cast<vtype>(*b)));
  }
};

struct OpDotMultiply
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    FM::OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(*a) * static_cast<vtype>(*b));
  }
};

struct OpDotRightDivide
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    FM::OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(*a) / static_cast<vtype>(*b));
  }
};

struct OpDotLeftDivide
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    FM::OpRangeConvert<ctype,vtype>::rangeConvert(*c,static_cast<vtype>(*b) / static_cast<vtype>(*a));
  }
};

struct OpLE
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) <= static_cast<vtype>(*b);
  }
};

struct OpLT
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) < static_cast<vtype>(*b);
  }
};

struct OpGE
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) >= static_cast<vtype>(*b);
  }
};

struct OpGT
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) > static_cast<vtype>(*b);
  }
};

struct OpEQ
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) == static_cast<vtype>(*b);
  }
};

struct OpNE
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) != static_cast<vtype>(*b);
  }
};

struct OpOr
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) || static_cast<vtype>(*b);
  }
};

struct OpAnd
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) && static_cast<vtype>(*b);
  }
};

struct OpNeg
{
  template <class T>
  inline static void func(T* c, const T* a)
  {
    *c = -*a;
  }
};

#endif
