#include "StructType.hpp"
#include "ThreadContext.hpp"
#include "CellType.hpp"

using namespace FM;

const Object *StructType::readOnlyData(const Object &p) const {
  const StructData *fp = static_cast<const StructData *>(p.d->data->ptr);
  return (_ctxt->_cell->readOnlyData(fp->m_data));
}

Object * StructType::readWriteData(Object &p) const {
  p.detach();
  StructData *fp = static_cast<StructData *>(p.d->data->ptr);
  return (_ctxt->_cell->readWriteData(fp->m_data));
}

Object StructType::makeStruct(const Tuple &dims, const FMStringList &fields) {
  Data *q = new Data;
  q->refcnt = 0;
  StructData *sd = new StructData;
  int fieldnum = fields.size();
  for (int i=0;i<fieldnum;i++)
    sd->m_fields[fields[i]] = i;
  Tuple cdims(dims);
  cdims.prepend(fieldnum);
  sd->m_data = _ctxt->_cell->zeroArrayOfSize(cdims,false);
  q->ptr = sd;
  ObjectBase *p = new ObjectBase(q,this,0,dims,0,1);
  return Object(p);      
}

void StructType::insertField(Object &p, const FMString &t) {
  p.detach();
  StructData *fp = static_cast<StructData *>(p.d->data->ptr);
  int ndx = fp->m_fields.count();
  fp->m_fields[t] = ndx++;
  Tuple newsize = p.dims();
  newsize.prepend(ndx);
  _ctxt->_cell->resize(fp->m_data,newsize);
}

void StructType::setScalar(Object &q, const FMString &field, const Object &p) {
  assert(q.isScalar());
  int n;
  std::cout << "Set Scalar called, field = " << field << " value " << p.description() << "\n";
  if (this->hasField(q,field))
    n = this->fieldIndex(q,field);
  else
    {
      std::cout << "Insert Called!\n";
      this->insertField(q,field);
      n = this->fieldIndex(q,field);
    }
  std::cout << "Writing to index " << n << "\n";
  Object* c = readWriteData(q);
  c[n] = p;
}

const Object & StructType::getScalar(const Object &q, const FMString &field) {
  assert(q.isScalar());
  int n = this->fieldIndex(q,field);
  const Object* c = readOnlyData(q);
  return c[n];
}

FMString StructType::describe(const Object &a) {
  if (a.isScalar()) {
    FMString ret = a.dims().toString() + " struct array with fields:\n";
    FMStringList fields(this->orderedFieldList(a));
    const Object* adata = this->readOnlyData(a);
    for (int i=0;i<fields.size();i++)
      ret += "   " + fields[i] + ": " + adata[fieldIndex(a,fields[i])].description() + "\n";
    return ret;
  } else {
    FMString ret = a.dims().toString() + " struct array with fields:\n";
    FMStringList fields(this->orderedFieldList(a));
    for (int i=0;i<fields.size();i++)
      ret += "   " + fields[i] + "\n";
    return ret;
  }
}
