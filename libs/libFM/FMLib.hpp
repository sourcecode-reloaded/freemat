#ifndef __FMLib_hpp__
#define __FMLib_hpp__

#include <string>
#include <set>
#include <list>
#include <vector>
#include <stack>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>
#include <sys/types.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/system_error.hpp>
#include <boost/tokenizer.hpp>


//#include <dlfcn.h>

//typedef std::wstring FMString;
//class FMString : public std::wstring
//{
//public:
//  FMString(const char *p)
//  {
//    for (int i=0;i<strlen(p);i++)
//      this->push_back(p[i]);
//  }
//  char * toAscii()
//  {
//    
//  }
//};
//
//typedef std::vector<std::wstring> FMStringList;
//

template <typename T>
inline const T& qMin(const T& a, const T &b) {
 return (a < b) ? a : b;
}

template <typename T>
inline const T& qMax(const T& a, const T &b) {
 return (a < b) ? b : a;
}


template <class T>
class FMVector : public std::vector<T>
{
public:
  FMVector(int sze) : std::vector<T>(sze) {}
  FMVector() : std::vector<T>() {}
  void pop_front() {
    this->erase(this->begin());
  }
  void push_front(const T& x) {
    this->insert(this->begin(),x);
  }
  FMVector& operator<<(const T& x)
  {
    this->push_back(x);
    return *this;
  }
  FMVector& operator<<(const FMVector<T>& x)
  {
    for (int i=0;i<x.size();i++)
      this->push_back(x[i]);
    return *this;
  }
  bool isEmpty() const
  {
    return this->size() == 0;
  }
  void remove(size_t ndx) 
  {
    this->erase(this->begin()+ndx);
  }
  const T* constData() const 
  {
    return &(this->at(0));
  }
};

class FMByteArray : public FMVector<char>
{
public:
  FMByteArray(const char *p, int len) : FMVector<char>(len)
  {
    char *dp = &(FMVector<char>::operator[](0));
    for (int i=0;i<len;i++)
      dp[i] = p[i];
  }
  FMByteArray(const char *p)
  {
    resize(strlen(p));
    char *dp = &(FMVector<char>::operator[](0));
    for (size_t i=0;i<strlen(p);i++)
      dp[i] = p[i];
  }
  FMByteArray() {}
  size_t length() const {
    return this->size();
  }
  size_t count() const {
    return this->size();
  }
  void remove(int pos, int len) {
    erase(begin()+pos,begin()+pos+len);
  }
};

template <class S>
class FMList : public std::vector<S>
{
public:
  FMList& operator<<(const S& x)
  {
    this->push_back(x);
    return *this;
  }
};

class FMStringList;

typedef char FMChar;

class FMString : public std::string
{
public:
  FMString(const char *p) : std::string(p) {}
  FMString(const std::string &q) : std::string(q) {}
  FMString(size_t len, FMChar p = 0) : std::string(len,p) {}
  FMString(const char *p, size_t len) : std::string(p,len) {}
  FMString(FMChar t) : std::string(1,t) {}
  FMString() : std::string() {}
  const char * toAscii() const
  {
    return this->c_str();
  }
  bool isEmpty() const 
  {
    return this->empty();
  }
  char back() const
  {
    return this->at(this->size()-1);
  }
  FMString leftJustified(int width, char fill=' ') const {
    return (*this) + FMString(qMax<int>(0,width-this->size()),fill);
  }
  FMString rightJustified(int width, char fill=' ') const {
    return FMString(qMax<int>(0,width-this->size()),fill)+(*this);
  }
  FMString toLower() const {
    FMString ret(this->size());
    for (size_t i=0;i<this->size();i++)
      ret[i] = tolower(this->at(i));
    return ret;
  }
  FMString toUpper() const {
    FMString ret(this->size());
    for (size_t i=0;i<this->size();i++)
      ret[i] = toupper(this->at(i));
    return ret;
  }
  int toInt() const {
    return atoi(this->c_str());
  }
  void chop(int cnt) {
    erase(end()-cnt,end());
  }
  void remove(int position, int len) {
    erase(position,len);
  }
  bool endsWith(const FMString &ending, bool caseInsensitive = false) const {
    if (!caseInsensitive)
      return (right(ending.size()) == ending);
    return (right(ending.size()).toUpper() == ending.toUpper());
  }
  bool startsWith(const FMString &starting) const {
    return (left(starting.size()) == starting);
  }
  double toDouble() const {
    return atof(this->c_str());
  }
  float toFloat() const {
    return float(atof(this->c_str()));
  }
  FMString left(size_t len) const
  {
    return FMString(substr(0,len));
  }
  FMString right(size_t len) const
  {
    return FMString(substr(this->size()-len,len));
  }
  FMString mid(size_t from, int len=-1) const
  {
    if (len == -1)
      len = size() - from;
    return FMString(substr(from,len));
  }
  int indexOf(const FMString& t, size_t start_pos = 0) const
  {
    return std::string::find(t, start_pos);
  }
  bool contains(const FMString& t) const
  {
    return (std::string::find(t,0) != std::string::npos);
  }
  int lastIndexOf(const FMString& t) const
  {
    return std::string::find_last_of(t);
  }
  FMString& replace(const std::string& from, const std::string& to) {
    if(from.empty())
      return *this;
    size_t start_pos = 0;
    while((start_pos = find(from, start_pos)) != std::string::npos) {
      std::string::replace(start_pos, from.length(), to);
      start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    return *this;
  }
  FMStringList split(const FMString& t, bool skipEmptyParts = false);
  FMString& append(char t)
  {
    this->push_back(t);
    return *this;
  }
  FMString& append(const FMString &t)
  {
    this->operator+=(t);
    return *this;
  }
  FMString trimmed() const
  {
    FMString copy(*this);
    while (copy.size() > 0 && (isspace(copy.back()))) {
      copy.chop(1);
    }
    while (copy.size() > 0 && (isspace(copy[0]))) {
      copy.remove(0,1);
    }
    return copy;
  }
  FMString rightTrimmed() const
  {
    FMString copy(*this);
    while (copy.size() > 0 && (isspace(copy.back()))) {
      copy.chop(1);
    }
    return copy;
  }
  FMString simplified() const
  {
    FMString copy = this->trimmed();
    FMString ret;
    bool inWhitespace = false;
    size_t ptr = 0;
    while (ptr < copy.size())
      {
	if (!inWhitespace)
	  {
	    if (isspace(copy[ptr]))
	      {
		inWhitespace = true;
		ret += ' ';
	      }
	    else
	      {
		ret += copy[ptr];
	      }
	  }
	else
	  {
	    if (!isspace(copy[ptr]))
	      {
		inWhitespace = false;
		ret += copy[ptr];
	      }
	  }
	ptr++;
      }
    return ret;
  }
};

inline const char *qPrintable(const FMString & c)
{
  return c.toAscii();
}

#define FMt_int64_C(c) _t_int64_C(c)
#define FM_UINT64_C(c) __UINT64_C(c)

template <class T>
inline FMString Stringify(const T& x)
{
  std::stringstream o;
  o << x;
  return FMString(o.str());
}

template <class T>
class FMStack : public std::stack<T>
{
};

template <class T>
class FMSet : public std::set<T>
{
public:
  bool contains(const T& p) const {
    return (std::set<T>::count(p) > 0);
  }
  size_t count() const {
    return this->size();
  }
  FMList<T> values() const {
    FMList<T> values;
    for (typename std::set<T>::const_iterator it=this->begin();
	 it != this->end(); ++it)
      values.push_back(*it);
    return values;
  }
};

class FMStringList : public FMVector<FMString>
{
public:
  FMStringList() : FMVector<FMString>() {}
  FMStringList(const FMString &t)
  {
    this->push_back(t);
  }
  int indexOf(const FMString& t) const
  {
    for (int i=0;i<this->size();i++)
      if (this->at(i) == t) return i;
    return -1;
  }
  int size() const
  {
    return FMVector<FMString>::size();
  }
  bool contains(const FMString& t) const
  {
    for (int i=0;i<this->size();i++)
      if (this->at(i) == t) return true;
    return false;
  }
  FMStringList& operator+=(const FMString& t) 
  {
    this->push_back(t);
    return *this;
  }
  FMStringList& operator<<(const FMString& x)
  {
    this->push_back(x);
    return *this;
  }
  FMStringList& operator<<(const FMStringList& x)
  {
    for (int i=0;i<x.size();i++)
      this->push_back(x[i]);
    return *this;
  }
};

inline FMStringList FMString::split(const FMString& t, bool skipEmptyParts)
{
  boost::char_separator<char> sep;
  if (skipEmptyParts)
    sep = boost::char_separator<char>(t.c_str());
  else
    sep = boost::char_separator<char>(t.c_str(),"",boost::keep_empty_tokens);
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  tokenizer tokens(*this,sep);
  FMStringList ret;
  for (tokenizer::iterator iter = tokens.begin();
       iter != tokens.end(); ++iter)
    ret.push_back(*iter);
  return ret;
  /*
  FMStringList ret;
  size_t start_pos = 0;
  size_t next_pos = 0;
  while ((next_pos = find(t,start_pos)) != std::string::npos) {
    ret << substr(start_pos,next_pos-start_pos+1);
    start_pos = next_pos + t.length();
  }
  return ret;
  */
}

template <class T>
inline std::ostream& operator<<(std::ostream& o, const FMList<T>& p)
{
  o << "[";
  for (typename FMList<T>::const_iterator i=p.begin();
       i != p.end(); ++i)
    o << *i << " ";
  o << "]";
  return o;
}

inline std::ostream& operator<<(std::ostream& o, const FMStringList& p)
{
  o << "(";
  for (int i=0;i<p.size();i++)
    o << p[i] << " ";
  o << ")";
  return o;
}


// This will need to be ported to Win32
#if 0
class FMLibrary
{
  FMString filename;
  void *lib;
public:
  FMLibrary(const FMString& libname) : filename(libname), lib(0) {}
  FMLibrary() : filename(""), lib(0) {}
  void setFileName(const FMString& libname) {filename = libname; lib = 0;}
  bool isLoaded() const {return lib != 0;}
  void unload() {dlclose(lib);}
  void load() {if (lib) return; lib = dlopen(filename.c_str(),RTLD_LAZY);}
  void* resolve(FMString symbolname) {
    load();
    return dlsym(lib,symbolname.c_str());
  }
};
#endif

class FMDir
{
  boost::filesystem::path p;
public:
  FMDir(boost::filesystem::path q) : p(q) {}
  FMDir(FMString path) {
    p = boost::filesystem::path(path.c_str());
  }
  boost::filesystem::path boostPath() {
    return p;
  }
  FMString absolutePath() {
    return boost::filesystem::absolute(p).string();
  }
  static FMString currentPath() {
    return boost::filesystem::current_path().string();
  }
  static FMDir current() {
    return FMDir(boost::filesystem::current_path());
  }
  static FMString homePath();
  static FMString separator() {
    return FMString("/");
  }
  bool operator==(const FMDir& other) {
    return p == other.p;
  }
  static bool setCurrent(const FMString& path) {
    boost::system::error_code ec;
    boost::filesystem::current_path(path.c_str(),ec);
    return (ec == 0);
  }
};

/*
class FMDateTime 
{
  std::time_t _time;
public:
};*/

template <class S, class T>
class FMMultiMap : public std::multimap<S,T>
{
public:
  void insert(const S& s, const T& t)
  {
    std::multimap<S,T>::insert(std::pair<S,T>(s,t));
  }
  FMList<T> values(const S& s) const
  {
    std::pair<typename std::multimap<S,T>::const_iterator,
	      typename std::multimap<S,T>::const_iterator> ret;
    ret = this->equal_range(s);
    FMList<T> values;
    for (typename std::multimap<S,T>::const_iterator it=ret.first; it != ret.second; ++it)
      values.push_back(it->second);
    return values;
  }
  FMList<S> uniqueKeys() const
  {
    FMSet<S> keys;
    for (typename std::multimap<S,T>::const_iterator it=this->begin(); 
	 it != this->end(); ++it)
      keys.insert(it->first);
    return keys.values();
  }
};

template <class S, class T>
class FMMap : public std::map<S,T>
{
public:
  class iterator
  {
    typename std::map<S,T>::iterator d;
  public:
    iterator(const typename std::map<S,T>::iterator &x) : d(x) {}
    iterator() : d() {}
    T& value() const {
      return d->second;
    }
    const S& key() const {
      return d->first;
    }
    bool operator!=(const iterator& other) const
    {
      return d != other.d;
    }
    iterator& operator++()
    {
      d++;
      return *this;
    }
    bool operator==(const iterator& other) const
    {
      return d == other.d;
    }
  };
  class const_iterator
  {
    typename std::map<S,T>::const_iterator d;
  public:
    const_iterator(const typename std::map<S,T>::const_iterator &x) : d(x) {}
    const_iterator() : d() {}
    const T& value() const {
      return d->second;
    }
    const S& key() const {
      return d->first;
    }
    bool operator!=(const const_iterator& other) const
    {
      return d != other.d;
    }
    const_iterator& operator++()
    {
      d++;
      return *this;
    }
    const_iterator operator++(int)
    {
      const_iterator copy(*this);
      d++;
      return copy;
    }
    bool operator==(const const_iterator& other) const
    {
      return d == other.d;
    }
  };
  void insert(const S& s, const T& t)
  {
    std::map<S,T>::insert(std::pair<S,T>(s,t));
  }
  bool contains(const S &key) const
  {
    return (std::map<S,T>::count(key) > 0);
  }
  const T & value(const S &key) const
  {
    return this->at(key);
  }
  void remove(const S &key)
  {
    this->erase(key);
  }
  size_t count() const {
    return this->size();
  }
  const_iterator constBegin() const {
    return const_iterator(std::map<S,T>::begin());
  }
  const_iterator constEnd() const {
    return const_iterator(std::map<S,T>::end());
  }
  iterator begin() {
    return iterator(std::map<S,T>::begin());
  }
  iterator end() {
    return iterator(std::map<S,T>::end());
  }
  const T& operator[](const S& key) const {
    return this->at(key);
  }
  T& operator[](const S& key) {
    return std::map<S,T>::operator[](key);
  }
  // T& operator[](const S& key) {
  //   return this->at(key);
  // }
};


class FMMutex : public boost::mutex
{
  
};

class FMWaitCondition : public boost::condition_variable
{
};


class FMThread
{
};

class FMMutexLocker
{
  FMMutex *p;
public:
  FMMutexLocker(FMMutex *ptr) : p(ptr) {p->lock();}
  ~FMMutexLocker() {p->unlock();}
};

typedef std::ostream FMTextStream;

class FMFile
{
  FMString _name;
  FILE *_fp;
public:
  FMFile(FMString filename) : _name(filename) {}
  FMFile(FILE *fp) : _fp(fp) {}
  bool valid()
  {
    return (_fp != NULL);
  }
  bool open(FMString mode);
  int read(void *ptr, int bytes)
  {
    return fread(ptr,1,bytes,_fp);
  }
  int write(const void *ptr, int bytes)
  {
    return fwrite(ptr,1,bytes,_fp);
  }
  int write(const FMString &t)
  {
    return fwrite(t.c_str(),1,t.length(),_fp);
  }
  bool getChar(FMChar *t)
  {
    int p = fgetc(_fp);
    if (p == EOF) return false;
    *t = p;
    return true;
  }
  void flush()
  {
    fflush(_fp);
  }
  FMString errorString() const;
  void ungetChar(FMChar t)
  {
    ungetc(t,_fp);
  }
  long pos()
  {
    return ftell(_fp);
  }
  bool seek(long pos)
  {
    return (fseek(_fp,pos,SEEK_SET) == 0);
  }
  int64_t size()
  {
    long p = ftell(_fp);
    fseek(_fp,0,SEEK_END);
    long size = ftell(_fp);
    fseek(_fp,p,SEEK_SET);
    return size;
  }
  bool atEnd()
  {
    if (!_fp) return true;
    if (feof(_fp)) return true;
    FMChar c;
    if (!getChar(&c)) return true;
    ungetChar(c);
    return false;
  }
  ~FMFile() 
  {
    _fp && fclose(_fp);
  }
  FMString readLine()
  {
    FMString ret;
    FMChar c;
    while (getChar(&c))
      {
	ret += c;
	if (c == '\n')
	  break;
      }
    return ret;
  };
};

inline FMString ReadFileIntoString(FMString filename, bool& failed) {
  std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
  failed = !in;
  if (failed) return FMString();
  FMString contents;
  in.seekg(0, std::ios::end);
  contents.resize(unsigned(in.tellg()));
  in.seekg(0, std::ios::beg);
  in.read(&contents[0], contents.size());
  in.close();
  if (!contents.endsWith("\n")) contents += "\n";
  return contents;
}

// Provide a surrogate for the shared data concept.
class FMSharedData {
  int _refcount;
public:
  FMSharedData() : _refcount(0) {}
  void addRef() {++_refcount;}
  int release() {return --_refcount;}
  void setRef(int cnt) {_refcount = cnt;}
  bool unique() {return _refcount == 1;}
  virtual ~FMSharedData() {}
};

template <class T>
class FMSharedDataPointer {
  T* _baseptr;
public:
  FMSharedDataPointer() : _baseptr(0) 
  {
  }
  FMSharedDataPointer(T* pData) : _baseptr(pData) 
  {
    _baseptr->addRef();
  }
  FMSharedDataPointer(const FMSharedDataPointer<T>& copy) : _baseptr(copy._baseptr) 
  {
    if (_baseptr) _baseptr->addRef();
  }
  ~FMSharedDataPointer()
  {
    if (_baseptr && (_baseptr->release() == 0))
      {
	delete _baseptr;
      }
  }
  void ensureUnique()
  {
    if (_baseptr->unique())
      return;
    _baseptr->release();
    _baseptr = new T(*_baseptr);
    _baseptr->setRef(1);
  }
  T& operator*()
  {
    ensureUnique();
    return *_baseptr;
  }
  T* operator->()
  {
    ensureUnique();
    return _baseptr;
  }
  const T* operator->() const
  {
    return _baseptr;
  }
  const T& operator*() const
  {
    return *_baseptr;
  }
  FMSharedDataPointer<T>& operator=(const FMSharedDataPointer<T>& sp)
  {
    if (this != &sp)
      {
	if (_baseptr && (_baseptr->release() == 0))
	  delete _baseptr;
	_baseptr = sp._baseptr;
	if (_baseptr) _baseptr->addRef();
      }
    return *this;
  }
  bool operator!() const
  {
    return _baseptr == 0;
  }
  operator bool() const
  {
    return _baseptr != 0;
  }
};

#endif
