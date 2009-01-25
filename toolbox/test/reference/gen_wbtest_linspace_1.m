function gen_wbtest_linspace_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    z=linspace(0,60000,60001);y1=z(end);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_linspace_1_ref.mat error_refs  y1_refs 
end
