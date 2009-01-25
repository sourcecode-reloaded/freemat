function gen_wbtest_csvread_3(verbose)
  load wbinputs.mat
  error_refs = 0;
  y1 = [];
  try
    y1=csvread('sample_data.csv',1,2,[1,2,3,4]);
  catch
    error_refs = 1;
  end
  if (~error_refs)
  y1_refs = {y1};
  end
  save wbtest_csvread_3_ref.mat error_refs  y1_refs 
end
