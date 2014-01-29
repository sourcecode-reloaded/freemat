% Function to lookup values in a table.
function idx = lookup(table, y, opt)
  Ycnt = numel(y);
  idx = zeros(size(y));
  M = numel(table);
  reversed = false;
  if (table(end) < table(1))
     reversed = true;
  end
  % Yes... this is actually efficient because it JIT compiles
  for i=1:Ycnt
    yval = y(i);
    if (~reversed)
      result = tlookup_normal(table, yval);
    else
      result = tlookup_reversed(table, yval);
    end
    idx(i) = result;
  end
  match_mode = false;
  bool_mode = false;
  left_inf_mode = false;
  right_inf_mode = false;
  if (nargin > 2)
     match_mode = any(opt == 'm');
     bool_mode = any(opt == 'b');
     if (~reversed)
       left_inf_mode = any(opt == 'l');
       right_inf_mode = any(opt == 'r');
     else
       left_inf_mode = any(opt == 'r');
       right_inf_mode = any(opt == 'l');
     end
  end
  if (left_inf_mode)
     idx = max(1,idx);
  end
  if (right_inf_mode)
     idx = min(M-1,idx);
  end
  if (match_mode || bool_mode)
    matches = table(max(1,idx)) == y;
     tmp = idx;
     idx = zeros(size(y));
     idx(matches) = tmp(matches);
  end
  if (bool_mode)
     idx = (idx > 0);
  end
end

function result = tlookup_reversed(table, yval)
  result = 0;
  M = numel(table);
  if (yval > table(1))
    result = 0;
  elseif (yval <= table(M))
    result = M;
  else
    printf('Reversed case\n');
    lobound = M;
    hibound = 1;
    keyfound = 0;
    while (((lobound-hibound) > 1) && (keyfound == 0))
      mid = floor((lobound + hibound)/2);
      printf('lo = %d hi = %d mid = %d table(mid) = %f yval = %f\n',...
	     lobound,hibound,mid,table(mid),yval);
      if (table(mid) > yval)
	hibound = mid;
      elseif (table(mid) < yval)
	lobound = mid;
      else
	hibound = mid;
	keyfound = 1;
      end
    end
      printf('END lo = %d hi = %d mid = %d table(mid) = %f yval = %f\n',...
	     lobound,hibound,mid,table(mid),yval);
    result = hibound;
    printf('result = %d\n',result);
  end
end


function result = tlookup_normal(table, yval)
  result = 0;
  M = numel(table);
  if (yval < table(1))
    result = 0;
  elseif (yval >= table(M))
    result = M;
  else
    lobound = 1;
    hibound = M;
    keyfound = 0;
    while (((hibound-lobound) > 1) && (keyfound == 0))
      mid = floor((lobound + hibound)/2);
      if (table(mid) < yval)
        lobound = mid;
      elseif (table(mid) > yval)
	hibound = mid;
      else
	lobound = mid;
	keyfound = 1;
      end
    end
    result = lobound;
  end
end

function result = tlookup_normal_string(table, yval)
  result = 0;
  M = numel(table);
  if (yval < table(1))
    result = 0;
  elseif (yval >= table(M))
    result = M;
  else
    lobound = 1;
    hibound = M;
    keyfound = 0;
    while (((hibound-lobound) > 1) && (keyfound == 0))
      mid = floor((lobound + hibound)/2);
      if (table(mid) < yval)
        lobound = mid;
      elseif (table(mid) > yval)
	hibound = mid;
      else
	lobound = mid;
	keyfound = 1;
      end
    end
    result = lobound;
  end
end
