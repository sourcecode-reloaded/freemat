#include <boost/timer/timer.hpp>
#include <stdio.h>

struct Info
{
  int refcnt;
  float data[1000];
  int cnt;
  int _refcount;
};

static double copycount = 0;
static double assignmentcount = 0;



Info **pool;
int poolsize = 0;

void initPool()
{
  pool = (Info**) malloc(sizeof(Info*)*10);
  for (int i=0;i<10;i++)
    pool[i] = new Info;
  poolsize = 0;
}


Info * allocateInfo()
{
  Info *p = pool[poolsize++];
  p->_refcount = 0;
  return p;
}

void returnInfo(Info * p)
{
  pool[--poolsize] = p;
}

// Based on this...
//
//  Array should just be a 
//
//  typedef FMISharedDataPointer<ArrayData> Array;
//
//

template <class T>
class FMISharedDataPointer {
  T* _baseptr;
public:
  inline FMISharedDataPointer() : _baseptr(0) 
  {
  }
  inline FMISharedDataPointer(T* pData) : _baseptr(pData) 
  {
    ++_baseptr->_refcount;
  }
  inline FMISharedDataPointer(const FMISharedDataPointer<T>& copy) : _baseptr(copy._baseptr) 
  {
    if (_baseptr) ++_baseptr->_refcount;
  }
  inline ~FMISharedDataPointer()
  {
    if (_baseptr && ((--_baseptr->_refcount) == 0))
      {
	returnInfo(_baseptr);
      }
  }
  inline FMISharedDataPointer<T>& operator=(const FMISharedDataPointer<T>& sp)
  {
    if (this != &sp)
      {
	if (_baseptr && ((--_baseptr->_refcount) == 0))
	  returnInfo(_baseptr);
	_baseptr = sp._baseptr;
	if (_baseptr) ++_baseptr->_refcount;
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
  inline T* base()
  {
    return _baseptr;
  }
};


template <class T>
class FMI2SharedDataPointer {
  T* _baseptr;
public:
  inline FMI2SharedDataPointer() : _baseptr(0) 
  {
  }
  inline FMI2SharedDataPointer(T* pData) : _baseptr(pData) 
  {
    ++_baseptr->_refcount;
  }
  inline FMI2SharedDataPointer(const FMI2SharedDataPointer<T>& copy) : _baseptr(copy._baseptr) 
  {
    if (_baseptr) ++_baseptr->_refcount;
  }
  inline ~FMI2SharedDataPointer()
  {
    if (_baseptr && ((--_baseptr->_refcount) == 0))
      {
	delete (_baseptr);
      }
  }
  inline FMI2SharedDataPointer<T>& operator=(const FMI2SharedDataPointer<T>& sp)
  {
    if (this != &sp)
      {
	if (_baseptr && ((--_baseptr->_refcount) == 0))
	  delete (_baseptr);
	_baseptr = sp._baseptr;
	if (_baseptr) ++_baseptr->_refcount;
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
  inline T* base()
  {
    return _baseptr;
  }
};


class CNArray
{
public:
  float real;
  bool scalar;
  Info *areal;

  inline CNArray() : real(0), scalar(true), areal(0) {}
  inline ~CNArray() {
    if (areal) 
      {
	areal->refcnt--;
	if (areal->refcnt == 0) returnInfo(areal);
      }
  }
  inline CNArray(const CNArray &copy )
  {
    copycount++;
    real = copy.real;
    scalar = copy.scalar;
    areal = copy.areal;
    if (areal) 
      areal->refcnt++;
  }
  inline CNArray& operator=(const CNArray& sp)
  {
    assignmentcount++;
    if (this != &sp)
      {
	if (areal)
	  {
	    areal->refcnt--;
	    if (areal->refcnt == 0) returnInfo(areal);
	  }
	real = sp.real;
	scalar = sp.scalar;
	areal = sp.areal;
	if (areal)
	  areal->refcnt++;
      }
    return *this;
  }  
};

int case8()
{
  int cnt = 1;
  for (int i=0;i<1000000;i++)
    {
      CNArray A;
      A.scalar = i;
      for (int j=0;j<100;j++)
	{
	  CNArray B = A;
	  B.scalar += j;
	  CNArray C = A;
	  cnt += B.scalar;
	}
    }  
  return cnt;
}


int case9()
{
  int cnt = 1;
  for (int i=0;i<1000000;i++)
    {
      CNArray A;
      A.areal = allocateInfo();
      A.areal->refcnt = 1;
      for (int j=0;j<100;j++)
	{
	  CNArray B = A;
	  cnt += B.areal->refcnt;
	  CNArray C = B;
	  C = A;
	  cnt += C.areal->refcnt;
	}
    }
  return cnt;  
}

int case10()
{
  int total = 0;
  double busy = 0;
  for (int i=0;i<1000000;i++)
    {
      Info *A = allocateInfo();
      A->cnt = 1;
      for (int j=0;j<100;j++)
	{
	  Info *B = A;
	  A->cnt++;
	  busy += A->cnt;
	  Info *C = A;
	  A->cnt++;
	  A->cnt--;
	  if (A->cnt == 0) returnInfo(A);
	  A->cnt--;
	  busy += A->cnt;
	  if (A->cnt == 0) returnInfo(A);
	  total++;
	}
      A->cnt--;
      if (A->cnt == 0) returnInfo(A);
    }
  return total - busy;
}

int case11()
{
  typedef FMISharedDataPointer<Info> Array;

  int total = 0;
  double busy = 0;
  for (int i=0;i<1000000;i++)
    {
      Array A = Array(allocateInfo());
      A.base()->cnt = i;
      for (int j=0;j<100;j++)
	{
	  Array B = A;
	  busy += B.base()->cnt;
	  Array C = A;
	  busy += C.base()->cnt;
	  total++;
	}
    }
  return total - busy;
}


int case12()
{
  typedef FMI2SharedDataPointer<Info> Array;

  int total = 0;
  double busy = 0;
  for (int i=0;i<1000000;i++)
    {
      Array A = Array(new Info);
      A.base()->cnt = i;
      for (int j=0;j<100;j++)
	{
	  Array B = A;
	  busy += B.base()->cnt;
	  Array C = A;
	  busy += C.base()->cnt;
	  total++;
	}
    }
  return total - busy;
}

int case13()
{
  class WArray
  {
  public:
    FMISharedDataPointer<Info> p;
    int typecode;
    
    WArray(Info* q) : p(q) {}
  };

  int total = 0;
  double busy = 0;
  for (int i=0;i<1000000;i++)
    {
      WArray A = WArray(allocateInfo());
      A.p.base()->cnt = i;
      for (int j=0;j<100;j++)
	{
	  WArray B = A;
	  busy += B.p.base()->cnt;
	  WArray C = A;
	  busy += C.p.base()->cnt;
	  total++;
	}
    }
  return total - busy;
}



int main()
{
  boost::timer::cpu_timer timer;

  initPool();
  
  timer.start();
  int p = case8();
  timer.stop();

  std::cout << "Array copy test: (Class - scalar) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  p = case9();
  timer.stop();

  std::cout << "Array copy test: (Class - heap) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  p = case10();
  timer.stop();

  std::cout << "Array copy test: (C stype - internal pool) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  p = case11();
  timer.stop();

  std::cout << "Array copy test: (ishared pointer type - internal pool) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  p = case12();
  timer.stop();

  std::cout << "Array copy test: (ishared pointer type - heap) " << timer.elapsed().wall/1.0e9 << "\n";

  timer.start();
  p = case13();
  timer.stop();

  std::cout << "Array copy test: (ishared pointer type - internal pool - class wrapper) " << timer.elapsed().wall/1.0e9 << "\n";


  std::cout << "p = " << p << "\n";

  std::cout << "Assignment count " << assignmentcount << "\n";
  std::cout << "copy count " << copycount << "\n";
  return 0;
}
