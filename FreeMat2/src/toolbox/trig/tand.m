%!
%@Module TAND Tangent Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the tangent of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|cos|). The syntax for its use is
%@[
%   y = tand(x)
%@]
%@@Examples
%The tangent of 45 degrees should be @|1|
%@<
%tand(45)
%@>
%@@Tests
%@$y1=tand(x1)|near
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = tand(x)
  y = tan(deg2rad(x));
