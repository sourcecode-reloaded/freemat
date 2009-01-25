function gen_wbtest_polyval_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=polyval([1,0,0,0],linspace(-1,1));
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_polyval_1_ref.mat error_refs  y1_refs 
end
