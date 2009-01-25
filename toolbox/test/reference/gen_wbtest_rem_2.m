function gen_wbtest_rem_2(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=rem(6,5);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_rem_2_ref.mat error_refs  y1_refs 
end
