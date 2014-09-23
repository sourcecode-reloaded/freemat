#include "StructType.hpp"
#include "ThreadContext.hpp"
#include "CellType.hpp"
#include "ListType.hpp"
#include "DoubleType.hpp"

using namespace FM;


Object StructType::empty() {
  Data *q = new Data;
  q->refcnt = 0;
  StructData *sd = new StructData;
  sd->m_fields = _ctxt->_list->empty();
  sd->m_data = _ctxt->_cell->empty();
  q->ptr = sd;
  ObjectBase *p = new ObjectBase(q,this,0,Tuple(0,0),0,sd->m_data.capacity());
  return Object(p);
};
  
/*
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
*/

/*
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

*/

int StructType::getFieldIndex(const Object &a, const Object &b) {
  const StructData *sd = this->readOnlyData(a);
  const Object *fn = _ctxt->_list->readOnlyData(sd->m_fields);
  for (int i=0;i<sd->m_fields.elementCount();i++)
    if (fn[i] == b) return i;
  return -1;
}
/*
Object* StructTyoe::getRWDP(StructData *sd) {
  
}
*/
Object StructType::getField(const Object &a, const Object &b) {
  int ndx = this->getFieldIndex(a,b);
  if (ndx == -1) throw Exception("Field " + b.description() + " is not defined for this struct");
  const StructData *sd = this->readOnlyData(a);
  const Object *cd = _ctxt->_cell->readOnlyData(sd->m_data);
  dim_t toret = sd->m_data.elementCount();
  Object ret = _ctxt->_list->empty();
  for (dim_t i=0;i<toret;i++)
    {
      const Object *rd = _ctxt->_list->readOnlyData(cd[i]);
      _ctxt->_list->push(ret,rd[ndx]);
    }
  return ret;
}

Object StructType::getParens(const Object &a, const Object &args) {
  Object ret = this->empty();
  StructData *sd = this->readWriteData(ret);
  const StructData *ad = this->readOnlyData(a);
  sd->m_fields = ad->m_fields;
  sd->m_data = _ctxt->_cell->getParens(ad->m_data,args);
  ret.d->dims = sd->m_data.dims();
  return ret;
}

void StructType::addNewFields(Object &a, const Object &fields) {
  StructData *ad = this->readWriteData(a);
  _ctxt->_list->merge(ad->m_fields,fields);
  if (ad->m_data.isEmpty()) {
    Object x = _ctxt->_list->makeMatrix(fields.elementCount(),1);
    ad->m_data = _ctxt->_cell->makeScalar(x);
    return;
  }
  Object *cp = _ctxt->_cell->readWriteData(ad->m_data);
  for (dim_t i=0;i<ad->m_data.elementCount();i++)
    _ctxt->_list->resize(cp[i],Tuple(ad->m_fields.elementCount(),1));
}

void StructType::setParens(Object &a, const Object &args, const Object &b) {
  // First the easy case
  // TODO - Type check b?
  StructData *ad = this->readWriteData(a);
  const StructData *bd = this->readOnlyData(b);
  if (ad->m_fields == bd->m_fields)
    {
      _ctxt->_cell->setParens(ad->m_data,args,bd->m_data);
      a.d->dims = ad->m_data.dims();
    }
  else
    {
      Object newfields = _ctxt->_list->empty();
      const Object *bd_ptr = _ctxt->_list->readOnlyData(bd->m_fields);
      std::vector<int> ndx_save;
      for (int i=0;i<bd->m_fields.elementCount();i++)
	{
	  int ndx = this->getFieldIndex(a,bd_ptr[i]);
	  ndx_save.push_back(ndx);
	  if (ndx == -1) {
	    _ctxt->_list->push(newfields,bd_ptr[i]);
	  }
	}
      // add newfields
      std::cout << "new fields: " << newfields.description() << "\n";
      addNewFields(a,newfields);
      throw Exception("Cannot handle twisted structure assignment.");
    }
}

void StructType::setField(Object &a, const Object &args, const Object &b) {
  std::cout << "set: " << a.description() << " field: " << args.description() << " to " << b.description() << "\n";
  int ndx = this->getFieldIndex(a,args);
  StructData *sd = this->readWriteData(a);
  Object *cd = _ctxt->_cell->readWriteData(sd->m_data);
  if (ndx == -1) {
    _ctxt->_list->push(sd->m_fields,args);
    for (dim_t i=0;i<sd->m_data.elementCount();i++)
      _ctxt->_list->push(cd[i],_ctxt->_double->empty());
    ndx = sd->m_fields.elementCount()-1;
  }
  if (sd->m_data.isEmpty()) {
    Object x = _ctxt->_list->makeMatrix(sd->m_fields.elementCount(),1);
    sd->m_data = _ctxt->_cell->makeScalar(x);
  }
  if (!sd->m_data.isScalar()) throw Exception("Multiple assignments not supported");
  cd = _ctxt->_cell->readWriteData(sd->m_data);
  Object *rd = _ctxt->_list->readWriteData(cd[0]);
  rd[ndx] = b;
  a.d->dims = sd->m_data.dims();
  std::cout << "result: " << a.description() << "\n";
}

FMString StructType::describe(const Object &a) {
  if (a.isScalar()) {
    FMString ret = a.dims().toString() + " struct array with fields:\n";
    const StructData *sd = this->readOnlyData(a);
    const Object *fn = _ctxt->_list->readOnlyData(sd->m_fields);
    const Object *cd = _ctxt->_cell->readOnlyData(sd->m_data);
    const Object *rd = _ctxt->_list->readOnlyData(cd[0]);
    for (int i=0;i<sd->m_fields.elementCount();i++)
      ret += "   " + fn[i].description() + ": " + rd[i].description() + "\n";
    return ret;
  } else {
    FMString ret = a.dims().toString() + " struct array with fields:\n";
    const StructData *sd = this->readOnlyData(a);
    const Object *fn = _ctxt->_list->readOnlyData(sd->m_fields);
    for (int i=0;i<sd->m_fields.elementCount();i++)
      ret += "   " + fn[i].description() + "\n";
    return ret;
  }
}

