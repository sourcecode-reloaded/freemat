%!
%@Module COSD Cosine Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the cosine of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|cos|). The syntax for its use is
%@[
%   y = cosd(x)
%@]
%@@Examples
%The cosine of 45 degrees should be @|sqrt(2)/2|
%@<
%cosd(45)
%@>
%and the cosine of @|60| degrees should be 0.5:
%@<
%cosd(60)
%@>
%@@Tests
%@$near|y1=cosd(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cosd(x)
  y = cos(deg2rad(x));
