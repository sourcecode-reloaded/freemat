%!
%@Module CSCD Cosecant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the cosecant of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|csc|). The syntax for its use is
%@[
%   y = cscd(x)
%@]
%@@Tests
%@$y1=cscd(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cscd(x)
  y = csc(deg2rad(x));
