% Regression test function (black box) for fopen
% This function is autogenerated by helpgen.py
function bbtest_success = bbtest_io_fopen
  bbtest_success = 1;
NumErrors = 0;
try
  fp = fopen('test.dat','w','ieee-le')
catch
  NumErrors = NumErrors + 1;
end
try
  fwrite(fp,float([1.2,4.3,2.1]))
catch
  NumErrors = NumErrors + 1;
end
try
  fclose(fp)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  fp = fopen('test.dat','r','ieee-le')
catch
  NumErrors = NumErrors + 1;
end
try
  fread(fp,[1,3],'float')
catch
  NumErrors = NumErrors + 1;
end
try
  fclose(fp)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  fp = fopen('test.dat','a+','le')
catch
  NumErrors = NumErrors + 1;
end
try
  fwrite(fp,float([pi,e]))
catch
  NumErrors = NumErrors + 1;
end
try
  fclose(fp)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
NumErrors = 0;
try
  fp = fopen('test.dat','r','ieee-le')
catch
  NumErrors = NumErrors + 1;
end
try
  fread(fp,[1,5],'float')
catch
  NumErrors = NumErrors + 1;
end
try
  fclose(fp)
catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
