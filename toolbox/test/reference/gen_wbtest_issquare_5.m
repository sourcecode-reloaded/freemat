function gen_wbtest_issquare_5(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=issquare(rand(1,1,3));
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_issquare_5_ref.mat error_refs  y1_refs 
end
