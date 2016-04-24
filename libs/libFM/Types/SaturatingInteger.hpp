#ifndef __SaturatingInteger_hpp__
#define __SaturatingInteger_hpp__

#include "Complex.hpp"

namespace FM
{
  
  // This is a templated type that implements saturating arithmetic
  template <class T>
  struct SaturatingInt {
    T val;
    SaturatingInt() : val(T{0}) {}
    explicit SaturatingInt(T x) : val(x) {}
    template <class S>
    explicit operator SaturatingInt<S>() const {return SaturatingInt<S>::fromDouble(val);}
    static SaturatingInt fromDouble(double y) {
      if (y < static_cast<double>(std::numeric_limits<T>::min()))
	return SaturatingInt(std::numeric_limits<T>::min());
      else if (y > static_cast<double>(std::numeric_limits<T>::max()))
	return SaturatingInt(std::numeric_limits<T>::max());
      else
	return SaturatingInt(static_cast<T>(y));
    }
    explicit operator double() const { return double(val);}
    explicit operator float() const { return float(val);}
    explicit operator ndx_t() const { return ndx_t(val);}
    explicit operator FMChar() const { return FMChar(val);}
    bool operator==(const SaturatingInt<T> &a) const {
      return val == a.val;
    }
    bool operator!=(const SaturatingInt<T> &a) const {
      return val != a.val;
    }
    bool operator>(const SaturatingInt<T> &a) const {
      return val > a.val;
    }
    bool operator>=(const SaturatingInt<T> &a) const {
      return val >= a.val;
    }
    bool operator<(const SaturatingInt<T> &a) const {
      return val < a.val;
    }
    bool operator<=(const SaturatingInt<T> &a) const {
      return val <= a.val;
    }
    SaturatingInt operator-() const {
      return fromDouble(-double(*this));
    }
  };

  template <class T>
  inline std::ostream& operator<<(std::ostream& o, const SaturatingInt<T> &a) {
    o << double(a.val);
    return o;
  }

  using sint8_t = SaturatingInt<std::int8_t>;
  using sint16_t = SaturatingInt<std::int16_t>;
  using sint32_t = SaturatingInt<std::int32_t>;
  using sint64_t = SaturatingInt<std::int64_t>;
  using usint8_t = SaturatingInt<std::uint8_t>;
  using usint16_t = SaturatingInt<std::uint16_t>;
  using usint32_t = SaturatingInt<std::uint32_t>;
  using usint64_t = SaturatingInt<std::uint64_t>;
  
};

#endif
