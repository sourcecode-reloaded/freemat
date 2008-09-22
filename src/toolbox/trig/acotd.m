%!
%@Module ACOTD Inverse Cotangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the inverse cotangent of its argument in degrees.  The general
%syntax for its use is
%@[
%  y = acotd(x)
%@]
%where @|x| is an @|n|-dimensional array of numerical type.
%@@Tests
%@$y1=acotd(x1)
%!


% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = acotd(x)
  y = rad2deg(acot(x));
  
  
