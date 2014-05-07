#ifndef __FMRefPtr_hpp__
#define __FMRefPtr_hpp__

// This is a non-threadsafe, copy-on-write, shared reference counting
// pointer.  The object being pointed to must have a counter called
// _refcnt available as a public member.  The template class argument
// must also support a static member function called release for 
// freeing an object, and duplicate for copying it.

template <class T>
class FMRefPtrCOW
{
  T* _baseptr;
 public:
  inline FMRefPtrCOW() : _baseptr(0) {}
  inline FMRefPtrCOW(T *pData) : _baseptr(pData) {
    if (_baseptr) ++_baseptr->_refcnt;
  }
  inline FMRefPtrCOW(const FMRefPtrCOW<T> &copy) : _baseptr(copy._baseptr) {
    if (_baseptr) ++_baseptr->_refcnt;
  }
  inline ~FMRefPtrCOW() {
    // Violating DRY for performance
    if (_baseptr && ((--_baseptr->_refcnt) == 0))
      T::release(_baseptr);
  }
  inline FMRefPtrCOW& operator=(const FMRefPtrCOW& sp) {
    if (this != &sp)
      {
	if (_baseptr && ((--_baseptr->_refcnt) == 0))
	  T::release(_baseptr);
	_baseptr = sp._baseptr;
	if (_baseptr) ++_baseptr->_refcnt;
      }
    return *this;
  }
  inline bool operator!() const
  {
    return _baseptr == 0;
  }
  inline operator bool() const
  {
    return _baseptr != 0;
  }
  inline T& operator*()
  {
    if (_baseptr && _baseptr->_refcnt > 1) {
      --_baseptr->_refcnt;
      _baseptr = T::duplicate(_baseptr);
      ++_baseptr->_refcnt;
    }
    return *_baseptr;
  }
  inline T* operator->()
  {
    if (_baseptr && _baseptr->_refcnt > 1) {
      --_baseptr->_refcnt;
      _baseptr = T::duplicate(_baseptr);
      ++_baseptr->_refcnt;
    }
    return _baseptr;
  }
  inline const T& operator*() const
  {
    return *_baseptr;
  }
  inline const T* operator->() const
  {
    return _baseptr;
  }
};

#endif
