function gen_wbtest_rem_3(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=rem(2*pi,pi);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_rem_3_ref.mat error_refs  y1_refs 
end
