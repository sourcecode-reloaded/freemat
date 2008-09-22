%!
%@Module SECD Secant Degrees Function
%@@Section MATHFUNCTIONS
%@@Usage
%Computes the secant of the argument, but takes
%the argument in degrees instead of radians (as is the case
%for @|sec|). The syntax for its use is
%@[
%   y = secd(x)
%@]
%@@Tests
%@$y1=secd(x1)
%!

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = secd(x)
  y = sec(deg2rad(x));
