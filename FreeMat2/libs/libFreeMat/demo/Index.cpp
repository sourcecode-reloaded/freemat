#include "Index.hpp"

template <typename T>
IndexArray::IndexArray(const BasicArray<T> &arg) :
  m_data(ConvertType<T,index_t>(arg)) {}

