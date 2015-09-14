#include "StructType.hpp"
#include "ThreadContext.hpp"
#include "CellType.hpp"
#include "ListType.hpp"
#include "DoubleType.hpp"
#include "StringType.hpp"

using namespace FM;

StructData::StructData(ThreadContext *ctxt) : m_data(ctxt->_cell->empty()) {}

static inline Object orderedFieldList(ThreadContext *ctxt, const StructData *sd) {
  Object fields = ctxt->_list->makeMatrix(sd->m_fields.size(),1);
  Object *fp = ctxt->_list->rw(fields);
  for (auto i : sd->m_fields)
    fp[i.second] = i.first;
  return fields;
}

FMString StructType::brief(const Object &a) {
  FMString ret = a.dims().toString() + " struct array\n";
  return ret;
}

FMString StructType::describe(const Object &a) {
  FMString ret = a.dims().toString() + " struct array with fields:\n";
  const StructData *sd = this->ro(a);
  Object fields = orderedFieldList(_ctxt,sd);
  const Object *fp = _ctxt->_list->ro(fields);
  if (a.isScalar()) {
    const Object *cd = _ctxt->_list->ro(_ctxt->_cell->scalarValue(sd->m_data));
    for (int i=0;i<fields.count();i++)
      {
	ret += "   " + _ctxt->_string->str(fp[i]) + ": ";
	int ndx = sd->m_fields.at(fp[i]);
	ret += cd[ndx].brief() + "\n";
      }
    return ret;
  } else {
    for (int i=0;i<fields.count();i++)
      ret += "   " + _ctxt->_string->str(fp[i]) + "\n";
    return ret;
  }
}

Object StructType::getField(const Object &a, const Object &b) {
  const StructData *sd = this->ro(a);
  auto i = sd->m_fields.find(b);
  if (i == sd->m_fields.end()) throw Exception("Field " + b.description() + " is not defined for this struct");
  int ndx = i->second;
  Object output = _ctxt->_list->makeMatrix(a.count(),1);
  Object *op = _ctxt->_list->rw(output);
  const Object *cp = _ctxt->_cell->ro(sd->m_data);
  for (ndx_t i=0;i<a.count();i++) {
    const Object *rp = _ctxt->_list->ro(cp[i]);
    op[i] = rp[ndx];
  }
  return output;
}

Object StructType::getParens(const Object &a, const Object &args) {
  Object ret = this->empty();
  StructData *sd = this->rw(ret);
  const StructData *ad = this->ro(a);
  sd->m_fields = ad->m_fields;
  sd->m_data = _ctxt->_cell->getParens(ad->m_data,args);
  ret.d->dims = sd->m_data.dims();
  return ret;
}

void StructType::addNewFields(Object &a, const Object &fields) {
  StructData *ad = this->rw(a);
  const Object *fp = _ctxt->_list->ro(fields);
  for (ndx_t i=0;i<fields.count();i++)
    ad->m_fields[fp[i]] = ad->m_fields.size();
  if (ad->m_data.isEmpty()) {
    Object x = _ctxt->_list->makeMatrix(ad->m_fields.size(),1);
    ad->m_data = _ctxt->_cell->makeScalar(x);
    return;
  }
  Object *cp = _ctxt->_cell->rw(ad->m_data);
  for (ndx_t i=0;i<ad->m_data.count();i++)
    _ctxt->_list->resize(cp[i],Tuple(ad->m_fields.size(),1));
}


// Merge fields from object b into object a
void StructType::mergeFields(Object &a, const Object &b) {
  StructData *ad = this->rw(a);
  Object newfields = _ctxt->_list->empty();
  const StructData *bd = this->ro(b);
  Object bfields = orderedFieldList(_ctxt,bd);
  const Object *bf_ptr = _ctxt->_list->ro(bfields);
  for (int j=0;j<bfields.count();j++)
    if (ad->m_fields.find(bf_ptr[j]) == ad->m_fields.end())
      _ctxt->_list->push(newfields,bf_ptr[j]);
  // add newfields
  std::cout << "new fields: " << newfields.description() << "\n";
  addNewFields(a,newfields);
}

// Reorder the fields in a to match those in b
void StructType::reorderFields(Object &a, const Object &b) {
  StructData *ad = this->rw(a);
  Object aFieldList = orderedFieldList(_ctxt,ad);
  std::vector<int> permutation;
  auto numFields = aFieldList.count();
  permutation.resize(unsigned(numFields));
  const StructData *bd = this->ro(b);
  const Object *afp = _ctxt->_list->ro(aFieldList);
  for (auto i=0;i<numFields;i++)
      permutation[i] = bd->m_fields.at(afp[i]);
  Object temp = _ctxt->_list->makeMatrix(numFields,1);
  Object *tp = _ctxt->_list->rw(temp);
  Object *cp = _ctxt->_cell->rw(ad->m_data);
  for (ndx_t i=0;i<a.count();i++)
    {
      Object *rp = _ctxt->_list->rw(cp[i]);
      for (int j=0;j<numFields;j++)
	tp[permutation[j]] = rp[j];
      for (int j=0;j<numFields;j++)
	rp[j] = tp[j];
    }
  ad->m_fields = bd->m_fields;
}

void StructType::fillEmpties(Object &a) {
  StructData *ad = this->rw(a);
  Object *rp = _ctxt->_cell->rw(ad->m_data);
  int fieldCount = ad->m_fields.size();
  for (ndx_t i=0;i<a.count();i++)
    if (!rp[i].isList())
      rp[i] = _ctxt->_list->makeMatrix(fieldCount,1);
}

void StructType::setParens(Object &a, const Object &args, const Object &b) {
  StructData *ad = this->rw(a);
  if (b.isEmpty())
    {
      _ctxt->_cell->setParens(ad->m_data,args,b);
      a.d->dims = ad->m_data.dims();
      return;
    }
  if (b.type() != this)
    throw Exception("Cannot assign A(..) = b where A is a structure array, and b is not");
  const StructData *bd = this->ro(b);
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
  StructData *sd = this->rw(a);
  const HashMap<int>::const_iterator i = sd->m_fields.find(args);
  int ndx;
  if (i == sd->m_fields.end()) {
    addNewFields(a,_ctxt->_list->makeScalar(args));
    ndx = sd->m_fields.size()-1; // New field will take last position
  } else {
    ndx = i->second;
  }
  Object *cd = _ctxt->_cell->rw(sd->m_data);
  if (sd->m_data.isEmpty()) {
    Object x = _ctxt->_list->makeMatrix(sd->m_fields.size(),1);
    sd->m_data = _ctxt->_cell->makeScalar(x);
  }
  cd = _ctxt->_cell->rw(sd->m_data);
  // Case one - scalar to scalar assignment
  ndx_t num_assignments = std::max<ndx_t>(1,a.count());
  const Object *bp = &b;
  if (b.isList()) 
    bp = _ctxt->_list->ro(b);
  // TODO - Allow for mismatch, and use empty to fill out?
  if (b.isList() && (num_assignments > b.count()))
      throw Exception("Mismatch in number of left and right hand sides in expression a.field = b");
  for (ndx_t n=0;n<num_assignments;n++)
    {
      Object *rd = _ctxt->_list->rw(cd[n]);
      rd[ndx] = *bp;
      if (b.isList()) bp++;
    }
  a.d->dims = sd->m_data.dims();
  std::cout << "result: " << a.description() << "\n";
}

Object StructType::NCat(const Object &p, int dim) {
  // Create a master list of fields
  const Object *ip = _ctxt->_list->ro(p);
  HashMap<int> group_fields;
  int fieldcounter = 0;
  for (auto i=0;i<p.count();i++) {
    const StructData *sd = this->ro(ip[i]);
    for (auto field : sd->m_fields)
      if (group_fields.find(field.first) == group_fields.end())
	group_fields.insert(std::make_pair(field.first,fieldcounter++));
  }
  Object uniform = _ctxt->_list->makeMatrix(p.count(),1);
  Object *rip = _ctxt->_list->rw(uniform);
  // For each struct, permute the data and fill it so that it fits
  for (auto i=0;i<p.count();i++) 
    rip[i] = expandStruct(ip[i],group_fields);
  // Now that the uniforms are all of the same structure, we can kick the
  // problem to NCat for the cell array
  Object merged = _ctxt->_cell->NCat(uniform,dim);
  // Create a new structure to return
  Object ret = this->empty();
  StructData *rd = this->rw(ret);
  rd->m_fields = group_fields;
  rd->m_data = merged;
  ret.d->dims = rd->m_data.dims();
  return ret;
}

Object StructType::expandStruct(const Object &s, const HashMap<int> &expanded_fields) {
  // Set up the permutation
  std::vector<int> permutation;
  const StructData *sd = this->ro(s);
  auto oldfields_count = sd->m_fields.size();
  permutation.resize(oldfields_count);
  for (auto field : sd->m_fields) 
    permutation[field.second] = expanded_fields.at(field.first);
  Object outdata = _ctxt->_cell->zeroArrayOfSize(sd->m_data.dims());
  Object *op = _ctxt->_cell->rw(outdata);
  const Object *ip = _ctxt->_cell->ro(sd->m_data);
  auto newfields_count = expanded_fields.size();
  for (auto m=0;m<sd->m_data.count();m++)
    {
      const Object *sf = _ctxt->_list->ro(ip[m]);
      op[m] = _ctxt->_list->makeMatrix(newfields_count,1);
      Object *rp = _ctxt->_list->rw(op[m]);
      for (ndx_t k=0;k<oldfields_count;k++)
	rp[permutation[unsigned(k)]] = sf[k];
    }
  return outdata;
  /*
  Object ret = this->empty();
  StructData *rd = this->rw(ret);
  rd->m_fields = expanded_fields;
  rd->m_data = outdata;
  ret.d->dims = rd->m_data.dims();
  return ret;
  */
}
