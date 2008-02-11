#ifndef __NTuple_hpp__
#define __NTuple_hpp__

#include <QtGlobal>
#include <QString>
#include <iostream>

typedef qint64 index_t;

const int NDims = 10;

typedef QString Exception;

class NTuple {
  index_t m_data[NDims];
public:
  index_t map(const NTuple& pos) const {
    index_t retval = 1;
    index_t nextCoeff = 1;
    for (int i=0;i<NDims;i++) {
      retval += nextCoeff*(pos[i]-1);
      nextCoeff *= m_data[i];
    }
    return retval;
  }
  inline bool validate(const NTuple& pos) const {
    for (int i=0;i<NDims;i++) 
      if ((pos.m_data[i] <= 0) || 
	  (pos.m_data[i] > m_data[i])) return false;
    return true;
  }
  inline NTuple(index_t rows, index_t cols) {
    m_data[0] = rows;
    m_data[1] = cols;
    for (int i=2;i<NDims;i++) m_data[i] = 1;
  }
  inline NTuple(index_t rows, index_t cols, index_t slices) {
    m_data[0] = rows;
    m_data[1] = cols;
    m_data[2] = slices;
    for (int i=3;i<NDims;i++) m_data[i] = 1;
  }
  inline NTuple() {
    m_data[0] = 0;
    m_data[1] = 0;
    for (int i=2;i<NDims;i++) m_data[i] = 1;
  }
  inline void set(int dim, index_t len) {
    m_data[dim] = len;
  }
  inline index_t& get(int dim) {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  inline index_t get(int dim) const {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  index_t operator[](int dim) const {
    return get(dim);
  }
  index_t& operator[](int dim) {
    return get(dim);
  }
  inline index_t count() const {
    index_t ret = 1;
    for (int i=0;i<NDims;i++) ret *= m_data[i];
    return ret;
  }
};

inline std::ostream& operator<<(std::ostream& o, const NTuple &t) {
  o << "<";
  for (int i=0;i<NDims;i++)
    o << t[i] << " ";
  o << ">";
  return o;
}

#endif
