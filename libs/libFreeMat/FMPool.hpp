#ifndef __FMPool_hpp__
#define __FMPool_hpp__

template <class T>
class FMPool
{
  T** _pool;
  unsigned _fill;
  unsigned _size;
  T* (*_maker)(FMPool<T>*);
public:
  FMPool(unsigned cnt, T* (maker)(FMPool<T>*))
  {
    _fill = 0;
    _size = cnt;
    _pool = new T*[cnt];
    _maker = maker;
  }
  T* pop() {
    if (!_fill) fill();
    return _pool[--_fill];
  }
  void push(T* ret) {
    if (_fill == _size) 
      {
	delete ret;
	return;
      }
    _pool[_fill++] = ret;
  }
  void fill() {
    while (_fill < _size)
      push(_maker(this));
  }
};


#endif
