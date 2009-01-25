function gen_wbtest_etime_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    a1=1e3*[2.006,.006,.009,.011,.044,.01907414];a2=1e3*[2.006,.006,.009,.011,.044,.024688253];y1=etime(a2,a1);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_etime_1_ref.mat error_refs  y1_refs 
end
