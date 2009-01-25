% SECD SECD Secant Degrees Function
% 
% Usage
% 
% Computes the secant of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for sec). The syntax for its use is
% 
%    y = secd(x)
% 
% SECD SECD Secant Degrees Function
% 
% Usage
% 
% Computes the secant of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for sec). The syntax for its use is
% 
%    y = secd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = secd(x)
  y = sec(deg2rad(x));
