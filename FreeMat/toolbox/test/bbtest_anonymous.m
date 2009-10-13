% Regression test function (black blox) for FreeMat v4.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_anonymous
  bbtest_success = 1;
NumErrors = 0;
try
  a = 2; b = 4;    % define a and b (slope and intercept)

catch
  NumErrors = NumErrors + 1;
end
try
  y = @(x) a*x+b   % create the anonymous function

catch
  NumErrors = NumErrors + 1;
end
try
  y(2)             % evaluate it for x = 2

catch
  NumErrors = NumErrors + 1;
end
try
  a = 5; b = 7;    % change a and b

catch
  NumErrors = NumErrors + 1;
end
try
  y(2)             % the value did not change!  because a=2,b=4 are captured in y

catch
  NumErrors = NumErrors + 1;
end
try
  y = @(x) a*x+b   % recreate the function

catch
  NumErrors = NumErrors + 1;
end
try
  y(2)             % now the new values are used

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
