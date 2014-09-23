#ifndef __HashMap__
#define __HashMap__

#include <boost/unordered_map.hpp>
#include "Object.hpp"

namespace FM
{
  struct FMHashMap_hash : std::unary_function<FM::Object,std::size_t>
  {
    std::size_t operator()(const Object &x) const;
  };

  const int cutover = 5;

  template <class T>
  class HashMap
  {
    boost::unordered_map<FM::Object,T,FMHashMap_hash> _hash;
    std::vector<std::pair<FM::Object,T> > _linear;
    int _count;
  public:
    HashMap() : _count(0) {_linear.reserve(cutover);}
    void insert(const FM::Object &key, const T& value) {
      if (_count < cutover)
	_linear.push_back(std::make_pair(key,value));
      else
	{
	  if (_count == cutover)
	    {
	      for (int i=0;i<_linear.size();i++)
		_hash.insert(_linear[i]);
	      _linear.clear();
	    }
	  _hash.insert(std::make_pair(key,value));
	}
      _count++;
    }
    T& at(const FM::Object & key) {
      if (_count <= cutover)
	{
	  for (int i=0;i<_linear.size();i++)
	    if (_linear[i].first == key) return _linear[i].second;
	  throw std::out_of_range("No value found");
	}
      return _hash.at(key);
    }
    const T& at(const FM::Object &key) const {
      if (_count <= cutover)
	{
	  for (int i=0;i<_linear.size();i++)
	    if (_linear[i].first == key) return _linear[i].second;
	  throw std::out_of_range("No value found");
	}
      return _hash.at(key);
    }
  };
};

#endif
