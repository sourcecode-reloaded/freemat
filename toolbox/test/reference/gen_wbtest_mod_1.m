function gen_wbtest_mod_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=mod(18,12);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_mod_1_ref.mat error_refs  y1_refs 
end
