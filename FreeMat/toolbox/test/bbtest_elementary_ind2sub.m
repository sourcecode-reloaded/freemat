% Regression test function (black box) for ind2sub
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_elementary_ind2sub
  bbtest_success = 1;
NumErrors = 0;
try
  A = randi(ones(3,4),10*ones(3,4))
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  [d1 d2] = ind2sub(size(A),7)
catch
  NumErrors = NumErrors + 1;
end
try
  A(d1,d2)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
