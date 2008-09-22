%!
%@Module ACOSH Inverse Hyperbolic Cosine Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse hyperbolic cosine of its argument.  The general
%syntax for its use is
%@[
%  y = acosh(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Function Internals
%The @|acosh| function is computed from the formula
%\[
%   \cosh^{-1}(x) = \log\left(x + (x^2 - 1)^0.5\right)
%\]
%where the @|log| (and square root) is taken in its most general sense.
%@@Examples
%Here is a simple plot of the inverse hyperbolic cosine function
%@<
%x = linspace(1,pi);
%plot(x,acosh(x)); grid('on');
%mprint('acoshplot');
%@>
%@figure acoshplot
%@@Tests
%@$y1=acosh(x1)
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acosh(x)
  y = log(x+(x.*x-1).^(0.5));
  
