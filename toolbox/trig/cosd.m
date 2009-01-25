% COSD COSD Cosine Degrees Function
% 
% Usage
% 
% Computes the cosine of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cos). The syntax for its use is
% 
%    y = cosd(x)
% 
% COSD COSD Cosine Degrees Function
% 
% Usage
% 
% Computes the cosine of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for cos). The syntax for its use is
% 
%    y = cosd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cosd(x)
  y = cos(deg2rad(x));
