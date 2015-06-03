#ifndef __FixedPool_hpp__
#define __FixedPool_hpp__

namespace FM
{

  template <class T>
  class FixedPool 
  {
    T* pool;
    unsigned fill;
    unsigned size;
  public:
    FixedPool(unsigned _size) 
    {
      pool = new T[_size];
      size = _size;
      fill = 0;
    }
    ~FixedPool()
    {
      delete[] pool;
    }
    bool push(T x)
    {
      if (fill >= size) return false;
      pool[fill++] = x;
      return true;
    }
    bool isEmpty() const
    {
      return (fill == 0);
    }
    bool isFull() const
    {
      return (fill == size);
    }
    T pop()
    {
      return pool[--fill];
    }
  };
}

#endif
