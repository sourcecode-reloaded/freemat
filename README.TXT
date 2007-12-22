This is the development branch for FreeMat4.  Its design is based on everything
that was learned from implementing FreeMat versions 1, 2 and 3.  The main
goals for FreeMat 4 are to maximize compatibility with the MATLAB programming
language, minimize the amount of code, and maximize the speed of execution.

The basic ideas are as follows:

1.  Reference counting is a serious pain - it slows down performance of
    the code.  FreeMat should handle this problem in an intelligent way.
    I haven't figured out what that is yet - perhaps some type of garbage
    collection.

2.  Type-based resolution should be done by the interpreter, not the
    individual functions involved.  For example, consider the various
    incarnations of the simple add operation.  In MATLAB-speak, the
    following statement:
           
                 y = a + b

    could have any of a dozen meanings.  For example, it could mean:
                 y = @double/add(a,b)
    or
                 y = @single/add(a,b)
    or
                 y = @foo/add(a,b)
    In previous incarnations of FreeMat, this was resolved at runtime
    by examining the types of a and b, promoting their types if necessary,
    and then adding (all within a single add function).   If we apply a
    standard set of type rules for all function calls (using operator
    precedence, for example), we can eliminate the distinction between
    builtin types and user-defined types.

3.  Differentiating between scalars and arrays - scalars are certainly
    special cases of arrays - no doubt about it.  But a lot of performance
    requirements are driven by differences between scalars and arrays.
    The new architecture will distinguish between the two, with a fall-back
    mechanism (where scalars are promoted to arrays automatically) when
    necessary.

4.  Intermediate language.  It is quite difficult to execute MATLAB-code
    directly.  And in FreeMat 1,2 and 3, debugging was actually quite 
    difficult.  Using an intermediate language was tried in the past, but
    it was based on the same internal array class as the interpreter, so
    it did little to help.  Instead, suppose, we have something like:
            
                 y = [1,2] * a + b

    and we break it down into a simplified set of expressions:
    
                 %t1 = [1,2];
                 %t2 = @double/mul_vec_sc(%t1,a);
                 %t3 = @double/add_vec_vec(%t2,b);

    where this code generation is valid for a=scalar, b=vector.
