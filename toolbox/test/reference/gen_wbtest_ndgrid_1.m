function gen_wbtest_ndgrid_1(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  y2 = [];
  try
    [y1,y2] = ndgrid(1:2,3:5);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  y2_refs = {y2};
  end
  save wbtest_ndgrid_1_ref.mat error_refs  y1_refs   y2_refs 
end
