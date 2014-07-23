#ifndef __Operators_hpp__
#define __Operators_hpp__

struct OpAdd
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) + static_cast<vtype>(*b);
  }
};

struct OpDotMultiply
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) * static_cast<vtype>(*b);
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

#endif
