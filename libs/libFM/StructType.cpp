#include "StructType.hpp"
#include "ThreadContext.hpp"
#include "CellType.hpp"
#include "ListType.hpp"
#include "DoubleType.hpp"
#include "StringType.hpp"

using namespace FM;


Object StructType::empty() {
  Data *q = new Data;
  q->refcnt = 0;
  StructData *sd = new StructData;
  sd->m_data = _ctxt->_cell->empty();
  q->ptr = sd;
  ObjectBase *p = new ObjectBase(q,this,0,Tuple(0,0),0,0);
  return Object(p);
};

static inline Object orderedFieldList(ThreadContext *ctxt, const StructData *sd) {
  Object fields = ctxt->_list->makeMatrix(sd->m_fields.size(),1);
  Object *fp = ctxt->_list->readWriteData(fields);
  for (HashMap<int>::const_iterator i=sd->m_fields.begin();i!=sd->m_fields.end();++i)
    fp[i->second] = i->first;
  return fields;
}

FMString StructType::describe(const Object &a) {
  FMString ret = a.dims().toString() + " struct array with fields:\n";
  const StructData *sd = this->readOnlyData(a);
  Object fields = orderedFieldList(_ctxt,sd);
  const Object *fp = _ctxt->_list->readOnlyData(fields);
  if (a.isScalar()) {
    const Object *cd = _ctxt->_cell->readOnlyData(_ctxt->_cell->scalarValue(sd->m_data));
    for (int i=0;i<fields.elementCount();i++)
      {
	ret += "   " + _ctxt->_string->getString(fp[i]) + ": ";
	int ndx = sd->m_fields.at(fp[i]);
	ret += cd[ndx].description() + "\n";
      }
    return ret;
  } else {
    for (int i=0;i<fields.elementCount();i++)
      ret += "   " + _ctxt->_string->getString(fp[i]) + "\n";
    return ret;
  }
}

Object StructType::getField(const Object &a, const Object &b) {
  const StructData *sd = this->readOnlyData(a);
  const HashMap<int>::const_iterator i = sd->m_fields.find(b);
  if (i == sd->m_fields.end()) throw Exception("Field " + b.description() + " is not defined for this struct");
  int ndx = i->second;
  Object output = _ctxt->_list->makeMatrix(a.elementCount(),1);
  Object *op = _ctxt->_list->readWriteData(output);
  const Object *cp = _ctxt->_cell->readOnlyData(sd->m_data);
  for (dim_t i=0;i<a.elementCount();i++) {
    const Object *rp = _ctxt->_list->readOnlyData(cp[i]);
    op[i] = rp[ndx];
  }
  return output;
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
  const Object *fp = _ctxt->_list->readOnlyData(fields);
  for (dim_t i=0;i<fields.elementCount();i++)
    ad->m_fields[fp[i]] = ad->m_fields.size();
  if (ad->m_data.isEmpty()) {
    Object x = _ctxt->_list->makeMatrix(ad->m_fields.size(),1);
    ad->m_data = _ctxt->_cell->makeScalar(x);
    return;
  }
  Object *cp = _ctxt->_cell->readWriteData(ad->m_data);
  for (dim_t i=0;i<ad->m_data.elementCount();i++)
    _ctxt->_list->resize(cp[i],Tuple(ad->m_fields.size(),1));
}


// Merge fields from object b into object a
void StructType::mergeFields(Object &a, const Object &b) {
  StructData *ad = this->readWriteData(a);
  Object newfields = _ctxt->_list->empty();
  const StructData *bd = this->readOnlyData(b);
  Object bfields = orderedFieldList(_ctxt,bd);
  const Object *bf_ptr = _ctxt->_list->readOnlyData(bfields);
  for (int j=0;j<bfields.elementCount();j++)
    {
      const HashMap<int>::const_iterator i = ad->m_fields.find(bf_ptr[j]);
      if (i == ad->m_fields.end())
	_ctxt->_list->push(newfields,bf_ptr[j]);
    }
  // add newfields
  std::cout << "new fields: " << newfields.description() << "\n";
  addNewFields(a,newfields);
}

// Reorder the fields in a to match those in b
void StructType::reorderFields(Object &a, const Object &b) {
  StructData *ad = this->readWriteData(a);
  Object aFieldList = orderedFieldList(_ctxt,ad);
  std::vector<int> permutation;
  int numFields = aFieldList.elementCount();
  permutation.resize(numFields);
  const StructData *bd = this->readOnlyData(b);
  const Object *afp = _ctxt->_list->readOnlyData(aFieldList);
  for (int i=0;i<numFields;i++)
      permutation[i] = bd->m_fields.at(afp[i]);
  Object temp = _ctxt->_list->makeMatrix(numFields,1);
  Object *tp = _ctxt->_list->readWriteData(temp);
  Object *cp = _ctxt->_cell->readWriteData(ad->m_data);
  for (dim_t i=0;i<a.elementCount();i++)
    {
      Object *rp = _ctxt->_list->readWriteData(cp[i]);
      for (int j=0;j<numFields;j++)
	tp[permutation[j]] = rp[j];
      for (int j=0;j<numFields;j++)
	rp[j] = tp[j];
    }
  ad->m_fields = bd->m_fields;
}

void StructType::fillEmpties(Object &a) {
  StructData *ad = this->readWriteData(a);
  Object *rp = _ctxt->_cell->readWriteData(ad->m_data);
  int fieldCount = ad->m_fields.size();
  for (dim_t i=0;i<a.elementCount();i++)
    if (!rp[i].isValid())
      rp[i] = _ctxt->_list->makeMatrix(fieldCount,1);
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
      // Copy fields from b --> a
      mergeFields(a,b);
      // Copy fields from a --> b
      Object bcopy = b;
      mergeFields(bcopy,a);
      // Permute the fields of b to match the field order of a
      reorderFields(bcopy,a);
      this->setParens(a,args,bcopy);
    }
  fillEmpties(a);
}

void StructType::setField(Object &a, const Object &args, const Object &b) {
  StructData *sd = this->readWriteData(a);
  const HashMap<int>::const_iterator i = sd->m_fields.find(args);
  int ndx;
  if (i == sd->m_fields.end()) {
    addNewFields(a,_ctxt->_list->makeScalar(args));
    ndx = sd->m_fields.size()-1; // New field will take last position
  } else {
    ndx = i->second;
  }
  Object *cd = _ctxt->_cell->readWriteData(sd->m_data);
  if (sd->m_data.isEmpty()) {
    Object x = _ctxt->_list->makeMatrix(sd->m_fields.size(),1);
    sd->m_data = _ctxt->_cell->makeScalar(x);
  }
  cd = _ctxt->_cell->readWriteData(sd->m_data);
  // Case one - scalar to scalar assignment
  dim_t num_assignments = a.elementCount();
  const Object *bp = &b;
  if (b.isList()) 
    bp = _ctxt->_list->readOnlyData(b);
  // TODO - Allow for mismatch, and use empty to fill out?
  if (b.isList() && (num_assignments > b.elementCount()))
      throw Exception("Mismatch in number of left and right hand sides in expression a.field = b");
  for (dim_t n=0;n<num_assignments;n++)
    {
      Object *rd = _ctxt->_list->readWriteData(cd[n]);
      rd[ndx] = *bp;
      if (b.isList()) bp++;
    }
  a.d->dims = sd->m_data.dims();
  std::cout << "result: " << a.description() << "\n";
}


