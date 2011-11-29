% Regression test function (black box) for char
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_typecast_char
  bbtest_success = 1;
NumErrors = 0;
try
  char([32:64;65:97])
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  char({'hello','to','the','world'})
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  char('hello','to','the','world')
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
