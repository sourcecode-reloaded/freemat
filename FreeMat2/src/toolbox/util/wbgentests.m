% Generate a test matrix based on an input matrix
function wbgentests(fname_in,fname_out)
% Load the test inputs
load(fname_in)
outputs = {};
for ndx=1:18000
%for ndx=1:size(recs,1)
  if (rem(ndx,1000)==0)
    fprintf('Processing test %d of %d...\r',ndx,size(recs,1));
  end
  rec = recs(ndx,:);
  if (rec(1) > 0)
    x1 = inputs{rec(1)};
  end
  if (rec(2) > 0)
    x2 = inputs{rec(2)};
  end
  error_flag = 0;
  texpr = exprs{rec(4)};
  texpr(end) = ';';
  eval(texpr,'error_flag = 1;');
  rec(5) = error_flag;
  if (error_flag == 0)
    if (rec(3) == 1)
      outpod = {y1};
    elseif (rec(3) == 2)
      outpod = {y1,y2};
    elseif (rec(3) == 3)
      outpod = {y1,y2,y3};
    elseif (rec(4) == 4)
      outpod = {y1,y2,y3,y4};
    elseif (rec(5) == 5)
      outpod = {y1,y2,y3,y4,y5};
    else
      outpod = {};
    end
    outputs = [outputs;outpod];
    rec(6) = numel(outputs);
  end
  recs(ndx,:) = rec;
end  
save(fname_out,'outputs','recs');
