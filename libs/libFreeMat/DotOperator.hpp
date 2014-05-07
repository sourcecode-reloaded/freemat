template <class Op>
FMObject3 DotOperator(const FMObject3 &a, const FMObject3 &b) {
  if (a.type() == b.type()) {
    FMObject3 c = a.type()->zeroArrayOfSize(FMTuple::computeDotOperatorSize(a.dims(),b.dims()));
    switch (a.type()->code())
    {
        case Double:
          Op::template func_st<double>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());
          break;
        case ComplexDouble:
          Op::template func_st<complexdouble>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());
          break;
        case Single:
          Op::template func_st<single>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());
          break;
        case ComplexSingle:
          Op::template func_st<complexsingle>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());
          break;
        case UInt64:
          Op::template func_st<uint64>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());
          break;
        case ComplexUInt64:
          Op::template func_st<complexuint64>(c.readWriteData(),a.readOnlyData(),b.readOnlyData(),a.isScalar(),b.isScalar(),c.elementCount());
          break;
    }
  }
}
