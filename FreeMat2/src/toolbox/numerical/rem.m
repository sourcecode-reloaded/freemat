%!
%@Module REM Remainder After Division
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the remainder after division of an array.  The syntax for its use is
%@[
%   y = rem(x,n)
%@]
%where @|x| is matrix, and @|n| is the base of the modulus.  The
%effect of the @|rem| operator is to add or subtract multiples of @|n|
%to the vector @|x| so that each element @|x_i| is between @|0| and @|n|
%(strictly).  Note that @|n| does not have to be an integer.  Also,
%@|n| can either be a scalar (same base for all elements of @|x|), or a
%vector (different base for each element of @|x|).
%
%Note that the following are defined behaviors:
%\begin{enumerate}
%\item @|rem(x,0) = nan|@
%\item @|rem(x,x) = 0|@ for nonzero @|x|
%\item @|rem(x,n)|@ has the same sign as @|x| for all other cases.
%\end{enumerate}
%Note that @|rem| and @|mod| return the same value if @|x| and @|n|
%are of the same sign.  But differ by @|n| if @|x| and @|y| have 
%different signs.
%@@Example
%The following examples show some uses of @|rem|
%arrays.
%@<
%rem(18,12)
%rem(6,5)
%rem(2*pi,pi)
%@>
%Here is an example of using @|rem| to determine if integers are even
% or odd:
%@<
%rem([1,3,5,2],2)
%@>
%Here we use the second form of @|rem|, with each element using a 
%separate base.
%@<
%rem([9 3 2 0],[1 0 2 2])
%@>
%!
function y = rem(x,n)
  if (isscalar(x))
    x = repmat(x,size(n));
  end
  if (isscalar(n))
    n = repmat(n,size(x));
  end
  m = fix(x./n);
  y = x - m.*n;