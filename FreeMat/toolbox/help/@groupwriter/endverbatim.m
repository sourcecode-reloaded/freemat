
% Copyright (c) 2002-2007 Samit Basu
% Licensed under the GPL

function endverbatim(&p)
  for i=1:numel(p.clients)
    endverbatim(p.clients{i});
  end
