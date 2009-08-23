% Regression test function (black blox) for FreeMat v4.0.0
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_lu
  bbtest_success = 1;
NumErrors = 0;
try
  a = float([1,2,3;4,5,8;10,12,3])

catch
  NumErrors = NumErrors + 1;
end
try
  [l,u,p] = lu(a)

catch
  NumErrors = NumErrors + 1;
end
try
  l*u

catch
  NumErrors = NumErrors + 1;
end
try
  p*a

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  a = sparse([1,0,0,4;3,2,0,0;0,0,0,1;4,3,2,4])

catch
  NumErrors = NumErrors + 1;
end
try
  [l,u,p,q,r] = lu(a)

catch
  NumErrors = NumErrors + 1;
end
try
  full(l*a)

catch
  NumErrors = NumErrors + 1;
end
try
  b = r*a

catch
  NumErrors = NumErrors + 1;
end
try
  full(b(p,q))

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
