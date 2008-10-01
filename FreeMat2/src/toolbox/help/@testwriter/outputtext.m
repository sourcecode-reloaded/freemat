
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function outputtext(&p,text)
  if (p.ignore) return; end
  fname = sprintf('%s/toolbox/test/wb_testmatrix.mat',p.sourcepath);
  if (~exist(fname))
    recs = [];
    exprs = {};
    inputs = wbtestinputs;
    save(fname,'recs','inputs','exprs');
  end
  v = regexp(text,'@\$([|].*)','tokens');
  if (isempty(v))
    error(sprintf('bad line: %s',text));
  end
% Search for inputs
expr = v{1}{1};
vars = symvar(expr);
pnt = regexp(vars,'x.');
in_count = 0; 
for i=1:numel(pnt); 
  if (~isempty(pnt{i}))
    in_count = in_count + 1; 
  end; 
end
pnt = regexp(vars,'y.');
out_count = 0;
for i=1:numel(pnt);
  if (~isempty(pnt{i}))
    out_count = out_count + 1;
  end;
end
% Generate the test record
% The columns of the test record are:
% in_count_1 in_count_2 output_counts expr_index success_flag output_index
load(fname);
exprs = [exprs;{expr}];
exprnum = numel(exprs);
if (in_count == 0)
  rec = zeros(1,6);
  rec(3) = out_count;
  rec(4) = exprnum;
elseif (in_count == 1)
  rec = zeros(numel(inputs),6);
  for i=1:numel(inputs)
    rec(i,1) = i;
    rec(i,3) = out_count;
    rec(i,4) = exprnum;
  end
elseif (in_count == 2)
  rec = zeros(numel(inputs)^2,6);
  q = 1;
  for j=1:numel(inputs)
    for i=1:numel(inputs)
      rec(q,1) = i;
      rec(q,2) = j;
      rec(q,3) = out_count;
      rec(q,4) = exprnum;
      q = q + 1;
    end
  end
end
recs = [recs;rec];
save(fname,'recs','inputs','exprs');
