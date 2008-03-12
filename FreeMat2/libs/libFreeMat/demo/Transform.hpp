#ifndef __Transform_hpp__
#define __Transform_hpp__

Variant Hermitian(const Variant& arg);
Variant Permute(const Variant& arg, const NTuple& perm);
Variant Transpose(const Variant& arg);
template <typename T>
Variant Range(T minval, T stepsize, T maxval, bool vertical);
Variant Matrix(const VariantMatrix& m);
Variant Cell(const VariantMatrix& m);
Variant Cell(const VariantList& m);
Variant Cell(const Variant& m);
Variant Struct(const StringList& s, const VariantList& v);
Variant Diagonal(const Variant& m, index_t order);
bool IsPositive(const Variant& m);
bool IsSymmetric(const Variant& m);
bool AllZeros(const Variant& m);
bool AnyNotFinite(const Variant& m);

#endif
