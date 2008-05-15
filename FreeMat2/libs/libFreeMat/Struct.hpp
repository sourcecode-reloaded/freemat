#ifndef __Struct_hpp__
#define __Struct_hpp__

#include "BasicArray.hpp"
#include "Types.hpp"

class Array;

class StructArray : public QMap<QString, BasicArray<Array> > {
  StringVector m_classPath;
public:
  bool isUserClass() const {return !m_classPath.empty();}
  QString className() const {return m_classPath.back();}
  void sliceClass(QString name) {m_classPath.push_back(name);}
  void unsliceClass() {m_classPath.pop_back();}
  const StringVector & classPath() const {return m_classPath;}
  void setClassPath(const StringVector & classPath) {m_classPath = classPath;}
};

#endif
