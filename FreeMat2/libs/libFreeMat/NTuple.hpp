#ifndef __NTuple_hpp__
#define __NTuple_hpp__

#include <QString>
#include <iostream>

#include "Types.hpp"

class NTuple {
  index_t m_data[NDims];
public:
  inline bool is2D() const {
    return (m_data[0]*m_data[1] == count());
  }
  inline index_t rows() const {
    return m_data[0];
  }
  inline index_t cols() const {
    return m_data[1];
  }
  inline bool isVector() const {
    return (is2D() && ((m_data[0] == 1) || (m_data[1] == 1)));
  }
  inline bool isColumnVector() const {
    return (is2D() && (m_data[1] == 1));
  }
  inline bool isRowVector() const {
    return (is2D() && (m_data[0] == 1));
  }
  inline bool isScalar() const {
    return (count() == 1);
  }
  inline index_t stride(int dim) const {
    index_t nextCoeff = 1;
    for (int i=1;i<=dim;i++) {
      nextCoeff *= m_data[i-1];
    }
    return nextCoeff;
  }
  inline index_t map(const NTuple& pos) const {
    index_t retval = 1;
    index_t nextCoeff = 1;
    for (int i=0;i<NDims;i++) {
      retval += nextCoeff*(pos[i]-1);
      nextCoeff *= m_data[i];
    }
    return retval;
  }
  inline void map(index_t vecndx, NTuple& pos) const {
    int64 vecndxi = (int64) (vecndx);
    vecndxi--;
    for (int i=0;i<NDims;i++) {
      pos[i] = (vecndxi % ((int64) m_data[i])) + 1;
      vecndxi /= ((int64) m_data[i]);
    }
  }
  inline bool validate(const NTuple& pos) const {
    for (int i=0;i<NDims;i++) 
      if ((pos.m_data[i] <= 0) || 
	  (pos.m_data[i] > m_data[i])) return false;
    return true;
  }
  inline void ripple(NTuple &pos) const {
    for (int i=0;i<(NDims-1);i++) {
      if (pos[i] > m_data[i]) {
	pos[i] = 1;
	pos[i+1]++;
      }
    }
  }
  inline void ripplePinned(NTuple &pos, int pin_dim) const {
    for (int i=0;i<(NDims-1);i++) {
      int64 dim = (int64) m_data[i];
      if (i == pin_dim) dim = 1;
      if (pos[i] > dim) {
	pos[i] = 1;
	pos[i+1]++;
      }
    }
  }
  inline void increment(NTuple &pos) const {
    pos[0]++;
    ripple(pos);
  }
  inline void increment(NTuple &pos, int pin_dim) const {
    if (pin_dim == 0) 
      pos[1]++;
    else
      pos[0]++;
    ripplePinned(pos,pin_dim);
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
  inline const index_t get(int dim) const {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  const index_t operator[](int dim) const {
    return get(dim);
  }
  index_t& operator[](int dim) {
    return get(dim);
  }
  void cover(const NTuple& alt) {
    for (int i=0;i<NDims;i++)
      if (m_data[i] < alt.m_data[i]) 
	m_data[i] = alt.m_data[i];
  }
  bool operator<=(const NTuple& alt) const {
    for (int i=0;i<NDims;i++)
      if (m_data[i] > alt.m_data[i]) return false;
    return true;
  }
  bool operator!=(const NTuple& alt) const {
    return !(*this == alt);
  }
  bool operator==(const NTuple& alt) const {
    for (int i=0;i<NDims;i++)
      if (m_data[i] != alt.m_data[i]) return false;
    return true;
  }
  bool operator==(index_t alt) const {
    if (m_data[0] != alt) return false;
    for (int i=1;i<NDims;i++)
      if (m_data[i] != 1) return false;
    return true;
  }
  inline const index_t count() const {
    index_t ret = 1;
    for (int i=0;i<NDims;i++) ret *= m_data[i];
    return ret;
  }
  inline int lastNotOne() const {
    int last_not_one = NDims-1;
    while ((m_data[last_not_one] == 1) && (last_not_one > 1)) last_not_one--;
    return last_not_one;
  }
  inline QString toString() const {
    int last_not_one = lastNotOne();
    QString ret = QString("%1").arg(m_data[0]);
    for (int i=1;i<=last_not_one;i++)
      ret += QString("x%1").arg(m_data[i]);
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
