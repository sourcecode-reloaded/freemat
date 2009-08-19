#ifndef __Vector_hpp__
#define __Vector_hpp__

#include <QSharedData>
#include <new>
#include "Exception.hpp"

template <typename T>
class VectorData : public QSharedData
{
  T* m_data;
  size_t m_len;

  void copyvec(const VectorData& copy) {
    if (m_data) delete[] m_data;
    m_len = copy.m_len;
    if (m_len) {
      try {
	m_data = new T[m_len];
      } catch (const std::bad_alloc& x) {
	throw Exception(QString("Cannot allocate enough memory to store an array of size %1").arg(m_len));
      }
      for (size_t i=0;i<m_len;i++)
	m_data[i] = copy.m_data[i];
    }
  }
public:
  inline VectorData() : m_data(0), m_len(0) {}
  inline VectorData(size_t dim) {
    try {
      m_data = new T[dim];
      m_len = dim;
    } catch (const std::bad_alloc& x) {
      throw Exception(QString("Cannot allocate enough memory to store an array of size %1").arg(dim));
    }
  }
  inline VectorData(const VectorData& copy) : QSharedData(copy), m_data(0), m_len(0) {
    copyvec(copy);
  }
  inline T operator[](size_t p) const {
    return m_data[p];
  }
  inline T& operator[](size_t p) {
    return m_data[p];
  }
  inline T at(size_t p) const {
    return m_data[p];
  }
  inline T* data() {
    return m_data;
  }
  inline const T* data() const {
    return m_data;
  }
  inline void clear() {
    m_len = 0;
    if (m_data) delete[] m_data;
  }
};

template <typename T>
class Vector
{
public:
  inline Vector() {d = new VectorData<T>();}
  inline Vector(size_t dim) {d = new VectorData<T>(dim);}
  inline Vector(const Vector& copy) : d(copy.d) {}
  inline T operator[](size_t p) const {
    return (*d)[p];
  }
  inline T& operator[](size_t p) {
    return (*d)[p];
  }
  inline T at(size_t p) const {
    return d->at[p];
  }
  inline T* data() {
    return d->data();
  }
  inline const T* data() const {
    return d->data();
  }
  inline void clear() {
    d->clear();
  }
private:
  VectorData<T> *d;
};

#endif
