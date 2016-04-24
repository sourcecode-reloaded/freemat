#ifndef __TypeCompute_hpp__
#define __TypeCompute_hpp__


#include "SaturatingInteger.hpp"
#include "Complex.hpp"

namespace FM
{

  template <class atype, class btype>
  struct compare_result_type
  {
    typedef double via_type;
    enum { is_valid = false };
  };

  // All types can be compared with themselves
  template <class T>
  struct compare_result_type<T,T>
  {
    typedef T via_type;
    enum { is_valid = true };
  };

  // We start with a default case in which the type computation is not allowed.
  template <class atype, class btype>
  struct compute_result_type
  {
    typedef atype result_type;
    typedef double via_type;
    enum { is_valid = false };
  };

  // Be explicit with double precision
  template <>
  struct compute_result_type<double,double>
  {
    typedef double result_type;
    typedef double via_type;
    enum { is_valid = true };
  };
  
  // An exception is computation with single precion, which is done via single precision
  template <>
  struct compute_result_type<float,float>
  {
    typedef float result_type;
    typedef float via_type;
    enum { is_valid = true };
  };
 
  // For all types, they can be mixed with double - end result is the type, with
  // computation done via double precision
  template <class T>
  struct compute_result_type<T,double>
  {
    typedef T result_type;
    typedef double via_type;
    enum { is_valid = true };
  };

  template <class T>
  struct compute_result_type<double,T>
  {
    typedef T result_type;
    typedef double via_type;
    enum { is_valid = true };
  };

  // Now for complex types.  Following MATLAB's lead, we don't allow
  // computation with complex types unless they are double or single precision

  // First, we override the default T,T match so that we use cdouble with cdoubles
  template <>
  struct compute_result_type<cdouble,cdouble>
  {
    typedef cdouble result_type;
    typedef cdouble via_type;
    enum { is_valid = true };
  };

  // Repeat the excercise with cfloat
  template <>
  struct compute_result_type<cfloat,cfloat>
  {
    typedef cfloat result_type;
    typedef cfloat via_type;
    enum { is_valid = true };
  };

  // Finally, enable double/float combinations, using cdouble as the intermediate type
  template <>
  struct compute_result_type<cdouble,cfloat>
  {
    typedef cdouble result_type;
    typedef cdouble via_type;
    enum { is_valid = true };
  };
  
  template <>
  struct compute_result_type<cfloat,cdouble>
  {
    typedef cdouble result_type;
    typedef cdouble via_type;
    enum { is_valid = true };
  };

  // Need to make sure that complex/double interactions preserve the complexity
  template <>
  struct compute_result_type<cdouble, double>
  {
    typedef cdouble result_type;
    typedef cdouble via_type;
    enum { is_valid = true };
  };

  // Need to make sure that complex/double interactions preserve the complexity
  template <>
  struct compute_result_type<double, cdouble>
  {
    typedef cdouble result_type;
    typedef cdouble via_type;
    enum { is_valid = true};
  };

  // Need to make sure that complex/double interactions preserve the complexity
  template <>
  struct compute_result_type<cfloat, float>
  {
    typedef cfloat result_type;
    typedef cfloat via_type;
    enum { is_valid = true };
  };

  // Need to make sure that complex/float interactions preserve the complexity
  template <>
  struct compute_result_type<float, cfloat>
  {
    typedef cfloat result_type;
    typedef cfloat via_type;
    enum { is_valid = true};
  };

  
};

#endif /* __TypeCompute_hpp__ */
