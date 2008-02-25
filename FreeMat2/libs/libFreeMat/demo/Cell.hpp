#ifndef __Cell_hpp__
#define __Cell_hpp__

#include "NDimArray.hpp"
#include "Array.hpp"

class CellArray {
  NDimArray<Array> m_data;
public:
  inline CellArray(const NTuple &pos) : m_data(pos) {}
  inline const NTuple dimensions() const {
    return m_data.dimensions();
  }
  inline const Array operator[](const NTuple& pos) const {
    return m_data[pos];
  }
  inline Array& operator[](const NTuple& pos) {
    return m_data[pos];
  }
  inline const Array operator[](index_t pos) const {
    return m_data[pos];
  }
  inline Array& operator[](index_t pos) {
    return m_data[pos];
  }
  
};

#endif
