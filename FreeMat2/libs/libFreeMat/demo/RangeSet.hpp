#ifndef __RangeSet_hpp__
#define __RangeSet_hpp__

#include "NTuple.hpp"

// consider 1:2:10 .. 1 3 5 7 9 -- 9/2 = 4.5 + 1 = 5
// consider 1:1:1 .. 1 -- 0/1 = 0 + 1
class RangeSet {
  NTuple m_first;
  NTuple m_step;
  NTuple m_last;
  NTuple m_dims;
public:
  RangeSet(NTuple first, NTuple step, NTuple last) :
    m_first(first), m_step(step), m_last(last) {
    calculateDims();
  }
  RangeSet() {}
  inline const NTuple& first() const {return m_first;}
  inline const NTuple& step() const {return m_step;}
  inline const NTuple& last() const {return m_last;}
  inline const NTuple& dims() const {return m_dims;}
  // Take a vector index, and convert to a sliced index
  inline void remap(index_t pos, NTuple& out) const {
    pos = pos-1;
    index_t pagesize = 1;
    for (int i=0;i<NDims-1;i++) {
      pagesize *= m_dims[i];
      index_t sliced_index = pos % pagesize;
      out[i] = m_first[i]+sliced_index*m_step[i];
      pos = pos / pagesize;
    }
  }
  inline void remap(const NTuple& pos, NTuple& out) const {
    for (int i=0;i<NDims;i++)
      out[i] = m_first[i] + (pos[i]-1)*m_step[i];
  }
  inline void calculateDims() {
    for (int i=0;i<NDims;i++)
      m_dims[i] = (((index_t) (m_last[i] - m_first[i])/(m_step[i])) + 1);
  }
  inline index_t count() {
    return m_dims.count();
  }
};

#endif
