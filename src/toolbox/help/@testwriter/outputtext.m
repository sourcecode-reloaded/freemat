
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function outputtext(&p,text)
  if (p.ignore) return; end
  text(end) = [];
  v = regexp(text,'@\$([^\|].*)\|(.*)','tokens');
  if (isempty(v))
    printf(sprintf('bad line: %s',text));
    return;
  end
  inputs = wbtestinputs;
  % Search for inputs
  ttype = v{1}{1};
if (~any(strcmp(ttype,{'near','exact','near_permute'})))
    printf(sprintf('bad test type in line: %s',text));
    return;
  end
  expr = v{1}{2};
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
  if (in_count == 0)
    rec = [];
    rec.inputs = [];
    rec.out_count = out_count;
    rec.expr = expr;
    rec.test = ttype;
    recp = {rec};
  elseif (in_count == 1)
    recp = cell(numel(inputs),1);
    for i=1:numel(inputs)
      rec = [];
      rec.inputs = [i];
      rec.out_count = out_count;
      rec.expr = expr;
      rec.test = ttype;
      recp{i} = rec;
    end
  elseif (in_count == 2)
    recp = cell(numel(inputs)^2,1);
    q = 1;
    for j=1:numel(inputs)
      for i=1:numel(inputs)
        rec = [];
	rec.inputs = [i,j];
        rec.out_count = out_count;
        rec.expr = expr;
        rec.test = ttype;
        recp{q} = rec; 
        q = q + 1;
      end
    end
  end
  p.recs = [p.recs;recp];
