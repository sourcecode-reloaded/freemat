% Regression test function (black box) for round
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_elementary_round
  bbtest_success = 1;
NumErrors = 0;
try
  round(3)
catch
  NumErrors = NumErrors + 1;
end
try
  round(-3)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  round(3.023f)
catch
  NumErrors = NumErrors + 1;
end
try
  round(-2.341f)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  round(4.312)
catch
  NumErrors = NumErrors + 1;
end
try
  round(-5.32)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
