function gen_wbtest_ismatrix_3(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=ismatrix(rand(4,4,2));
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_ismatrix_3_ref.mat error_refs  y1_refs 
end
