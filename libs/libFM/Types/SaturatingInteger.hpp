#ifndef __SaturatingInteger_hpp__
#define __SaturatingInteger_hpp__

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
    // Some repetition here...
    template <class S>
    SaturatingInt operator+(const S&) const {
      throw Exception("Unsupported type combination of types");
    }
    SaturatingInt operator+(const SaturatingInt &a) const {
      return fromDouble(static_cast<double>(val) + static_cast<double>(a.val));
    }
    SaturatingInt operator+(const double &a) const {
      return fromDouble(static_cast<double>(val) + a);
    }
    SaturatingInt operator-() const {
      return fromDouble(-static_cast<double>(val));
    }
    SaturatingInt operator-(const SaturatingInt &a) const {
      return fromDouble(static_cast<double>(val) - static_cast<double>(a.val));
    }
    SaturatingInt operator-(const double &a) const {
      return fromDouble(static_cast<double>(val) - a);
    }
    SaturatingInt operator*(const SaturatingInt &a) const {
      return fromDouble(static_cast<double>(val) * static_cast<double>(a.val));
    }
    SaturatingInt operator*(const double &a) const {
      return fromDouble(static_cast<double>(val) * a);
    }
    SaturatingInt operator/(const SaturatingInt &a) const {
      return fromDouble(static_cast<double>(val) / static_cast<double>(a.val));
    }
    SaturatingInt operator/(const double &a) const {
      return fromDouble(static_cast<double>(val) / a);
    }
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
  };

  template <class T>
  inline std::ostream& operator<<(std::ostream& o, const SaturatingInt<T> &a) {
    o << double(a.val);
    return o;
  }

  template <class T>
  inline ndx_t operator+(const ndx_t &a, const SaturatingInt<T> &b) {
    return a + static_cast<ndx_t>(b.val);
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
