#ifndef __Struct_hpp__
#define __Struct_hpp__

#include "BasicArray.hpp"
#include "Types.hpp"

class Array;

class StructArray {
  QStringList m_fields;
  QVector<BasicArray<Array> > m_data;
  StringVector m_classPath;
  NTuple m_dims;
public:
  bool isUserClass() const {return !m_classPath.empty();}
  QString className() const {return m_classPath.back();}
  void sliceClass(QString name) {m_classPath.push_back(name);}
  void unsliceClass() {m_classPath.pop_back();}
  const StringVector & classPath() const {return m_classPath;}
  void setClassPath(const StringVector & classPath) {m_classPath = classPath;}
  StringVector fieldNames() const {return m_fields;}
  int fieldCount() const {return m_fields.size();}
  QString fieldName(int i) const {return m_fields[i];}
  int fieldIndex(QString name) const {
    if (m_fields.contains(name)) 
      return m_fields.indexOf(name);
    throw Exception("Fieldname " + name + " not defined");
  }
  bool contains(QString name) const { return m_fields.contains(name); }
  void insert(QString name, BasicArray<Array> t) {
    if (!contains(name)) m_fields += name;
    m_data.push_back(t);
    updateDims();
  }
  NTuple dimensions() const {return m_dims;}
  void setDimensions(const NTuple &x) {m_dims = x;}
  void updateDims() {
    if (m_data.size() == 0)
      m_dims = NTuple(0,0);
    else
      m_dims = m_data[0].dimensions();
  }
  BasicArray<Array>& operator[](int i) {return m_data[i];}
  const BasicArray<Array>& operator[](int i) const {return m_data[i];}
  BasicArray<Array>& operator[](QString name) {
    if (!m_fields.contains(name)) {
      m_fields += name;
      m_data.push_back(BasicArray<Array>());
    }
    return m_data[fieldIndex(name)];
  }
  const BasicArray<Array>& operator[](QString name) const {return m_data[fieldIndex(name)];}
};

#endif
