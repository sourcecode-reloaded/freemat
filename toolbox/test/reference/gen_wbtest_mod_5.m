function gen_wbtest_mod_5(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=mod([9 3 2 0],[1 0 2 2]);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_mod_5_ref.mat error_refs  y1_refs 
end
