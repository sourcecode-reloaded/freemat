// Test the variant class with a bunch of combinations to see that it behaves properly
#include "Variant.hpp"

// static int count = 0;

// template <typename T>
// void printMatrix(const BasicArray<T> &A) {
//   for (int i=1;i<=A.dimensions()[0];i++) {
//     for (int j=1;j<=A.dimensions()[1];j++) {
//       std::cout << A[NTuple(i,j)] + 0.0 << " ";
//     }
//     std::cout << "\n";
//   }
//   std::cout << "************************************************************ " << 
//     count++ << "\n";
// }

Variant BoolIndex(BoolArray,NTuple(5,1));
Variant Int8Index(Int8Array,NTuple(3,1));
Variant UInt8Index(UInt8Array,NTuple(3,1));
Variant Int16Index(Int16Array,NTuple(3,1));
Variant UInt16Index(UInt16Array,NTuple(3,1));
Variant Int32Index(Int32Array,NTuple(3,1));
Variant UInt32Index(UInt32Array,NTuple(3,1));
Variant Int64Index(Int64Array,NTuple(3,1));
Variant UInt64Index(UInt64Array,NTuple(3,1));
Variant FloatIndex(FloatArray,NTuple(3,1));
Variant DoubleIndex(DoubleArray,NTuple(3,1));

Variant BoolScalarIndex((logical) 1);
Variant Int8ScalarIndex((int8) 3);
Variant UInt8ScalarIndex((uint8) 3);
Variant Int16ScalarIndex((int16) 3);
Variant UInt16ScalarIndex((uint16) 3);
Variant Int32ScalarIndex((int32) 3);
Variant UInt32ScalarIndex((uint32) 3);
Variant Int64ScalarIndex((int64) 3);
Variant UInt64ScalarIndex((uint64) 3);
Variant FloatScalarIndex((float) 3);
Variant DoubleScalarIndex((double) 3);

Variant BoolVec(BoolArray,NTuple(1,5));
Variant Int8Vec(Int8Array,NTuple(1,5));
Variant UInt8Vec(UInt8Array,NTuple(1,5));
Variant Int16Vec(Int16Array,NTuple(1,5));
Variant UInt16Vec(UInt16Array,NTuple(1,5));
Variant Int32Vec(Int32Array,NTuple(1,5));
Variant UInt32Vec(UInt32Array,NTuple(1,5));
Variant Int64Vec(Int64Array,NTuple(1,5));
Variant UInt64Vec(UInt64Array,NTuple(1,5));
Variant FloatVec(FloatArray,NTuple(1,5));
Variant DoubleVec(DoubleArray,NTuple(1,5));

Variant BoolMat(BoolArray,NTuple(5,5));
Variant Int8Mat(Int8Array,NTuple(5,5));
Variant UInt8Mat(UInt8Array,NTuple(5,5));
Variant Int16Mat(Int16Array,NTuple(5,5));
Variant UInt16Mat(UInt16Array,NTuple(5,5));
Variant Int32Mat(Int32Array,NTuple(5,5));
Variant UInt32Mat(UInt32Array,NTuple(5,5));
Variant Int64Mat(Int64Array,NTuple(5,5));
Variant UInt64Mat(UInt64Array,NTuple(5,5));
Variant FloatMat(FloatArray,NTuple(5,5));
Variant DoubleMat(DoubleArray,NTuple(5,5));


template <typename T>
void InitializeIndexArray(Variant &dest) {
  dest.set(1,Variant((T)1));
  dest.set(2,Variant((T)3));
  dest.set(3,Variant((T)5));
}

template <typename T>
void InitializeDataVec(Variant &dest) {
  dest.set(1,Variant((T)1));
  dest.set(2,Variant((T)3));
  dest.set(3,Variant((T)6));
  dest.set(4,Variant((T)8));
  dest.set(5,Variant((T)9));
}

template <typename T>
void InitializeDataMatrix(Variant &dest) {
  for (int i=1;i<=5;i++)
    for (int j=1;j<=5;j++)
      dest.set(NTuple(j,i),Variant((T)i*10+j));
}

void InitializeIndexArrays() {
  BoolIndex.set(1,Variant((logical)1));
  BoolIndex.set(3,Variant((logical)1));
  BoolIndex.set(5,Variant((logical)1));
  InitializeIndexArray<int8>(Int8Index);
  InitializeIndexArray<uint8>(UInt8Index);
  InitializeIndexArray<int16>(Int16Index);
  InitializeIndexArray<uint16>(UInt16Index);
  InitializeIndexArray<int32>(Int32Index);
  InitializeIndexArray<uint32>(UInt32Index);
  InitializeIndexArray<int64>(Int64Index);
  InitializeIndexArray<uint64>(UInt64Index);
  InitializeIndexArray<float>(FloatIndex);
  InitializeIndexArray<double>(DoubleIndex);
}

void InitializeDataMat() {
  BoolMat.set(NTuple(1,1),(logical) 1);
  BoolMat.set(NTuple(2,1),(logical) 1);
  BoolMat.set(NTuple(5,1),(logical) 1);
  BoolMat.set(NTuple(2,2),(logical) 1);
  BoolMat.set(NTuple(3,2),(logical) 1);
  BoolMat.set(NTuple(4,2),(logical) 1);
  BoolMat.set(NTuple(5,2),(logical) 1);
  BoolMat.set(NTuple(2,3),(logical) 1);
  BoolMat.set(NTuple(3,3),(logical) 1);
  BoolMat.set(NTuple(4,3),(logical) 1);
  BoolMat.set(NTuple(1,4),(logical) 1);
  BoolMat.set(NTuple(2,4),(logical) 1);
  BoolMat.set(NTuple(3,4),(logical) 1);
  BoolMat.set(NTuple(1,5),(logical) 1);
  BoolMat.set(NTuple(3,5),(logical) 1);
  BoolMat.set(NTuple(4,5),(logical) 1);
  InitializeDataMatrix<int8>(Int8Mat);
  InitializeDataMatrix<uint8>(UInt8Mat);
  InitializeDataMatrix<int16>(Int16Mat);
  InitializeDataMatrix<uint16>(UInt16Mat);
  InitializeDataMatrix<int32>(Int32Mat);
  InitializeDataMatrix<uint32>(UInt32Mat);
  InitializeDataMatrix<int64>(Int64Mat);
  InitializeDataMatrix<uint64>(UInt64Mat);
  InitializeDataMatrix<float>(FloatMat);
  InitializeDataMatrix<double>(DoubleMat);
}

void InitializeDataVecs() {
  BoolVec.set(1,(logical) 1);
  BoolVec.set(2,(logical) 0);
  BoolVec.set(3,(logical) 0);
  BoolVec.set(4,(logical) 1);
  BoolVec.set(5,(logical) 1);
  InitializeDataVec<int8>(Int8Vec);
  InitializeDataVec<uint8>(UInt8Vec);
  InitializeDataVec<int16>(Int16Vec);
  InitializeDataVec<uint16>(UInt16Vec);
  InitializeDataVec<int32>(Int32Vec);
  InitializeDataVec<uint32>(UInt32Vec);
  InitializeDataVec<int64>(Int64Vec);
  InitializeDataVec<uint64>(UInt64Vec);
  InitializeDataVec<float>(FloatVec);
  InitializeDataVec<double>(DoubleVec);
}

static int count = 0;

template <typename T>
void TestScalarGet(Variant arg) {
  Variant sub(arg.get(Variant((index_t)1)));
  Variant sub2((T) 3);
  if (!(sub == sub2))
    std::cout << "Test " << count++ << " passed\n";
  else
    std::cout << "Test " << count++ << " failed\n";
}

template <typename T>
void TestVecGet(Variant arg, Variant ndx) {
  Variant sub(arg.get(ndx));
  Variant sub2;
  if (ndx.type() == BoolArray)
    ndx = Variant(DoubleArray,Find(ndx.constReal<logical>()));
  sub2 = Variant(arg.type(),ndx.dimensions());
  for (index_t i=1;i<=ndx.length();i++) {
    sub2.set(i,arg.get(ndx.get(i)));
  }
  if (sub == sub2)
    std::cout << "Test " << count++ << " passed\n";
  else
    std::cout << "Test " << count++ << " failed\n";
}

template <typename T>
void TestMatGet(Variant arg, Variant ndx) {
  VariantList args;
  args.push_back(ndx);
  args.push_back(ndx);
  Variant sub(arg.get(args));
  Variant sub2;
  if (ndx.type() == BoolArray)
    ndx = Variant(DoubleArray,Find(ndx.constReal<logical>()));
  sub2 = Variant(arg.type(),NTuple(ndx.length(),ndx.length()));
  for (index_t i=1;i<=ndx.length();i++) {
    for (index_t j=1;j<=ndx.length();j++) {
      VariantList sset;
      sset.push_back(ndx.get(j));
      sset.push_back(ndx.get(i));
      sub2.set(NTuple(j,i),arg.get(sset));
    }
  }
  if (sub == sub2)
    std::cout << "Test " << count++ << " passed\n";
  else
    std::cout << "Test " << count++ << " failed\n";
}

void TestVecCases(Variant ndx) {
  TestVecGet<bool>(BoolVec,ndx);
  TestVecGet<int8>(Int8Vec,ndx);
  TestVecGet<uint8>(UInt8Vec,ndx);
  TestVecGet<int16>(Int16Vec,ndx);
  TestVecGet<uint16>(UInt16Vec,ndx);
  TestVecGet<int32>(Int32Vec,ndx);
  TestVecGet<uint32>(UInt32Vec,ndx);
  TestVecGet<int64>(Int64Vec,ndx);
  TestVecGet<uint64>(UInt64Vec,ndx);
  TestVecGet<float>(FloatVec,ndx);
  TestVecGet<double>(DoubleVec,ndx);
}

void TestMatCases(Variant ndx) {
  TestMatGet<bool>(BoolMat,ndx);
  TestMatGet<int8>(Int8Mat,ndx);
  TestMatGet<uint8>(UInt8Mat,ndx);
  TestMatGet<int16>(Int16Mat,ndx);
  TestMatGet<uint16>(UInt16Mat,ndx);
  TestMatGet<int32>(Int32Mat,ndx);
  TestMatGet<uint32>(UInt32Mat,ndx);
  TestMatGet<int64>(Int64Mat,ndx);
  TestMatGet<uint64>(UInt64Mat,ndx);
  TestMatGet<float>(FloatMat,ndx);
  TestMatGet<double>(DoubleMat,ndx);  
}

void TestScalarVecGet() {
  TestScalarGet<int8>(Int8ScalarIndex);
  TestScalarGet<uint8>(UInt8ScalarIndex);
  TestScalarGet<int16>(Int16ScalarIndex);
  TestScalarGet<uint16>(UInt16ScalarIndex);
  TestScalarGet<int32>(Int32ScalarIndex);
  TestScalarGet<uint32>(UInt32ScalarIndex);
  TestScalarGet<int64>(Int64ScalarIndex);
  TestScalarGet<uint64>(UInt64ScalarIndex);
  TestScalarGet<float>(FloatScalarIndex);
  TestScalarGet<double>(DoubleScalarIndex); 
}

int main() {
  InitializeIndexArrays();
  InitializeDataVecs();
  InitializeDataMat();

  TestVecCases(BoolIndex);
  TestVecCases(Int8Index);
  TestVecCases(UInt8Index);
  TestVecCases(Int16Index);
  TestVecCases(UInt16Index);
  TestVecCases(Int32Index);
  TestVecCases(UInt32Index);
  TestVecCases(Int64Index);
  TestVecCases(UInt64Index);
  TestVecCases(FloatIndex);
  TestVecCases(DoubleIndex);

  TestVecCases(BoolScalarIndex);
  TestVecCases(Int8ScalarIndex);
  TestVecCases(UInt8ScalarIndex);
  TestVecCases(Int16ScalarIndex);
  TestVecCases(UInt16ScalarIndex);
  TestVecCases(Int32ScalarIndex);
  TestVecCases(UInt32ScalarIndex);
  TestVecCases(Int64ScalarIndex);
  TestVecCases(UInt64ScalarIndex);
  TestVecCases(FloatScalarIndex);
  TestVecCases(DoubleScalarIndex);

  TestMatCases(BoolIndex);
  TestMatCases(Int8Index);
  TestMatCases(UInt8Index);
  TestMatCases(Int16Index);
  TestMatCases(UInt16Index);
  TestMatCases(Int32Index);
  TestMatCases(UInt32Index);
  TestMatCases(Int64Index);
  TestMatCases(UInt64Index);
  TestMatCases(FloatIndex);
  TestMatCases(DoubleIndex);

  TestMatCases(BoolScalarIndex);
  TestMatCases(Int8ScalarIndex);
  TestMatCases(UInt8ScalarIndex);
  TestMatCases(Int16ScalarIndex);
  TestMatCases(UInt16ScalarIndex);
  TestMatCases(Int32ScalarIndex);
  TestMatCases(UInt32ScalarIndex);
  TestMatCases(Int64ScalarIndex);
  TestMatCases(UInt64ScalarIndex);
  TestMatCases(FloatScalarIndex);
  TestMatCases(DoubleScalarIndex);

  TestScalarVecGet();
  return 0;
}
