#ifndef __BasicArrayPrivate_hpp__
#define __BasicArrayPrivate_hpp__


template class BasicArray<logical>;
template class BasicArray<uint8>;
template class BasicArray<int8>;
template class BasicArray<uint16>;
template class BasicArray<int16>;
template class BasicArray<uint32>;
template class BasicArray<int32>;
template class BasicArray<uint64>;
template class BasicArray<int64>;
template class BasicArray<float>;
template class BasicArray<double>;


template <>
void BasicArray<logical>::set(index_t pos, double val) {
  m_data[pos+m_offset-1] = (val != 0);
}

template <>
inline void BasicArray<uint8>::set(index_t pos, double val) {
  if (val < 0) val = 0;
  if (val > UCHAR_MAX) val = UCHAR_MAX;
  m_data[pos+m_offset-1] = (uint8) val;
}

template <>
inline void BasicArray<int8>::set(index_t pos, double val) {
  if (val < SCHAR_MIN) val = SCHAR_MIN;
  if (val > SCHAR_MAX) val = SCHAR_MAX;
  m_data[pos+m_offset-1] = (int8) val;
}

template <>
inline void BasicArray<uint16>::set(index_t pos, double val) {
  if (val < 0) val = 0;
  if (val > USHRT_MAX) val = USHRT_MAX;
  m_data[pos+m_offset-1] = (uint16) val;
}

template <>
inline void BasicArray<int16>::set(index_t pos, double val) {
  if (val < SHRT_MIN) val = SHRT_MIN;
  if (val > SHRT_MAX) val = SHRT_MAX;
  m_data[pos+m_offset-1] = (int16) val;
}

template <>
inline void BasicArray<uint32>::set(index_t pos, double val) {
  if (val < 0) val = 0;
  if (val > UINT_MAX) val = UINT_MAX;
  m_data[pos+m_offset-1] = (uint32) val;
}

template <>
inline void BasicArray<int32>::set(index_t pos, double val) {
  if (val < INT_MIN) val = INT_MIN;
  if (val > INT_MAX) val = INT_MAX;
  m_data[pos+m_offset-1] = (int32) val;
}

template <>
inline void BasicArray<uint64>::set(index_t pos, double val) {
  if (val < 0) val = 0;
  if (val > ULLONG_MAX) val = ULLONG_MAX;
  m_data[pos+m_offset-1] = (uint64) val;
}

template <>
inline void BasicArray<int64>::set(index_t pos, double val) {
  if (val < LLONG_MIN) val = LLONG_MIN;
  if (val > LLONG_MAX) val = LLONG_MAX;
  m_data[pos+m_offset-1] = (int64) val;
}

template <>
inline void BasicArray<float>::set(index_t pos, double val) {
  m_data[pos+m_offset-1] = (float) val;
}

template <>
inline void BasicArray<double>::set(index_t pos, double val) {
  m_data[pos+m_offset-1] = val;
}

#endif
