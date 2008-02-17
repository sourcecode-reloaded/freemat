#ifndef __NPerm_hpp__
#define __NPerm_hpp__

#include "Global.hpp"

class NPerm {
  int m_data[NDims];
public:
  inline NPerm(int t1, int t2) {
    m_data[0] = t1;
    m_data[1] = t2;
    for (int i=2;i<NDims;i++) m_data[i] = i+1;
  }
  inline NPerm(int t1, int t2, int t3) {
    m_data[0] = t1;
    m_data[1] = t2;
    m_data[2] = t3;
    for (int i=3;i<NDims;i++) m_data[i] = i+1;
  }
  inline NPerm() {
    for (int i=0;i<NDims;i++) m_data[i] = i+1;
  }
  inline void set(int dim, int len) {
    m_data[dim] = len;
  }
  inline int& get(int dim) {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  inline const int get(int dim) const {
    if ((dim<0) || (dim >= NDims))
      throw Exception("Illegal range in get");
    return m_data[dim];
  }
  inline const int operator[](int dim) const {
    return get(dim);
  }
  inline int& operator[](int dim) {
    return get(dim);
  }
};


#endif
