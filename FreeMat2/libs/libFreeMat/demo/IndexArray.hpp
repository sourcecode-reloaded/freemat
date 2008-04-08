#ifndef __IndexArray_hpp__
#define __IndexArray_hpp__

#include "BasicArray.hpp"

BasicArray<bool> GetDeletionMap(const IndexArray& vec, index_t length);

bool IsColonOp(const IndexArray& ndx);

IndexArray ExpandColons(const IndexArray& ndx, index_t len);

bool isSliceIndexCase(const IndexArrayList& index);

bool DimensionCovered(const IndexArray& ndx, index_t length);

index_t getSliceIndex(const NTuple &dimensions, const IndexArrayList& index);

#endif
