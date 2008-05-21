#ifndef __Index_hpp__
#define __Index_hpp__

class IndexArray {
  BasicArray<index_t> m_data;
  bool m_colon;
public:
  template <typename T>
  IndexArray(const BasicArray<T> &arg);
};

#endif
