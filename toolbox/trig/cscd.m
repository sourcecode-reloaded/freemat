% CSCD CSCD Cosecant Degrees Function
% 
% Usage
% 
% Computes the cosecant of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for csc). The syntax for its use is
% 
%    y = cscd(x)
% 
% CSCD CSCD Cosecant Degrees Function
% 
% Usage
% 
% Computes the cosecant of the argument, but takes
% the argument in degrees instead of radians (as is the case
% for csc). The syntax for its use is
% 
%    y = cscd(x)
% 

% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function y = cscd(x)
  y = csc(deg2rad(x));
