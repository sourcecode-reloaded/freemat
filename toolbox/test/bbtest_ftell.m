% Regression test function (black blox) for FreeMat v svn
% This function is autogenerated by helpgen.
function bbtest_success = bbtest_ftell
  bbtest_success = 1;
NumErrors = 0;
try
  fp = fopen('test.dat','wb');

catch
  NumErrors = NumErrors + 1;
end
try
  fwrite(fp,randn(512,1));

catch
  NumErrors = NumErrors + 1;
end
try
  fclose(fp);

catch
  NumErrors = NumErrors + 1;
end
try
  fp = fopen('test.dat','rb');

catch
  NumErrors = NumErrors + 1;
end
try
  x = fread(fp,[512,1],'float');

catch
  NumErrors = NumErrors + 1;
end
try
  ftell(fp)

catch
  NumErrors = NumErrors + 1;
end
if (NumErrors ~= 0) bbtest_success = 0; return; end
