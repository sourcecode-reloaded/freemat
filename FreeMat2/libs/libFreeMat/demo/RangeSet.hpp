#ifndef __RangeSet_hpp__
#define __RangeSet_hpp__

#include "NTuple.hpp"
#include <QVector>

class BaseRange {
public:
  virtual index_t at(index_t t) const = 0;
  virtual index_t count() const = 0;
  virtual ~BaseRange() {}
};

class NullRange : public BaseRange {
public:
  NullRange() {}
  virtual index_t at(index_t) const {return 1;}
  virtual index_t count() const {return 1;}
  virtual ~NullRange() {}
};

class VecRange : public BaseRange {
  QVector<index_t> m_vals;
public:
  VecRange(QVector<index_t> vals) :
    m_vals(vals) {
  }
  virtual index_t at(index_t t) const {return m_vals.at(t);}
  virtual index_t count() const {return m_vals.size();}
  virtual ~VecRange() {}
};

class StepRange : public BaseRange {
  index_t m_first;
  index_t m_step;
  index_t m_last;
  index_t m_count;
public:
  StepRange(index_t first, index_t step, index_t last) :
    m_first(first), m_step(step), m_last(last) {
    m_count = ((index_t) (m_last - m_first)/(m_step)) + 1;
  }
  virtual index_t at(index_t t) const {return m_first + (t-1)*m_step;}
  virtual index_t count() const {return m_count;}
  virtual ~StepRange() {}
};

// consider 1:2:10 .. 1 3 5 7 9 -- 9/2 = 4.5 + 1 = 5
// consider 1:1:1 .. 1 -- 0/1 = 0 + 1

typedef BaseRange* BaseRangePtr;

class RangeSet {
  BaseRangePtr m_range[NDims];
public:
  RangeSet() {
    for (int i=0;i<NDims;i++) m_range[i] = new NullRange;
  }
  inline void set(int i, BaseRangePtr t) {
    delete m_range[i];
    m_range[i] = t;
  }
  ~RangeSet() {
    for (int i=0;i<NDims;i++) delete m_range[i];
  }
  // Take a vector index, and convert to a sliced index
  inline void remap(index_t pos, NTuple& out) const {
    pos = pos-1;
    index_t pagesize = 1;
    for (int i=0;i<NDims-1;i++) {
      pagesize *= m_range[i]->count();
      index_t sliced_index = pos % pagesize;
      out[i] = m_range[i]->at(sliced_index+1);
      pos = pos / pagesize;
    }
  }
  inline void remap(const NTuple& pos, NTuple& out) const {
    for (int i=0;i<NDims;i++)
      out[i] = m_range[i]->at(pos[i]-1);
  }
  inline NTuple dims() const {
    NTuple out;
    for (int i=0;i<NDims;i++)
      out[i] = m_range[i]->count();
    return out;
  }
};

#endif
