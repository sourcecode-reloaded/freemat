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

struct OpSubtract
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) - static_cast<vtype>(*b);
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

struct OpDotDivide
{
  template <class ctype, class atype, class btype, class vtype>
  inline static void func(ctype *c, const atype *a, const btype *b)
  {
    *c = static_cast<vtype>(*a) / static_cast<vtype>(*b);
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


#endif
