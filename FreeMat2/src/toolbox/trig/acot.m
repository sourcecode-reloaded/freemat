%!
%@Module ACOT Inverse Cotangent Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cotangent of its argument.  The general
%syntax for its use is
%@[
%  y = acot(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acot| function is computed from the formula
%\[
%   \cot^{-1}(x) = \tan^{-1}\left(\frac{1}{x}\right)
%\]
%@@Examples
%Here is a simple plot of the inverse cotangent function
%@<
%x1 = -2*pi:pi/30:-0.1;
%x2 = 0.1:pi/30:2*pi;
%plot(x1,acot(x1),x2,acot(x2)); grid('on');
%mprint('acotplot');
%@>
%@figure acotplot
%@$y1=acot(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acot(x)
  if (nargin == 0 || ~isnumeric(x))
    error('acot expects a single, numeric input');
  end
  y = atan(1.0 ./ x);
  
