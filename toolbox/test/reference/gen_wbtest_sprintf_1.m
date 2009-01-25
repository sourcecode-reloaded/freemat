function gen_wbtest_sprintf_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=sprintf('hello %d',5);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_sprintf_1_ref.mat error_refs  y1_refs 
end
