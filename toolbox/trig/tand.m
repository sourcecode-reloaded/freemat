% TAND TAND Tangent Degrees Function
% 
% Usage
% 
% Computes the tangent of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cos). The syntax for its use is
% 
%    y = tand(x)
% 
% TAND TAND Tangent Degrees Function
% 
% Usage
% 
% Computes the tangent of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cos). The syntax for its use is
% 
%    y = tand(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = tand(x)
  y = tan(deg2rad(x));
