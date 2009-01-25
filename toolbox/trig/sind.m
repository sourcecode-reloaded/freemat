% SIND SIND Sine Degrees Function
% 
% Usage
% 
% Computes the sine of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cos). The syntax for its use is
% 
%    y = sind(x)
% 
% SIND SIND Sine Degrees Function
% 
% Usage
% 
% Computes the sine of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cos). The syntax for its use is
% 
%    y = sind(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = sind(x)
  y = sin(deg2rad(x));
