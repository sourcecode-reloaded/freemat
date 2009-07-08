function gen_wbtest_ismatrix_5(verbose)
  load reference/wbinputs.mat
  error_refs = 0;
  y1 = []; y1_refs = {};
  try
    y1=ismatrix(rand(1,1,3));
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save reference/wbtest_ismatrix_5_ref.mat error_refs  y1_refs 
end
