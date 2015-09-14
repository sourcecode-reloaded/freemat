function tf = strcmp(a,b)
  if (isa(a,'string') && isa(b,'string'))
      tf = (numel(a) == numel(b)) && all(a(:) == b(:));
  else
      tf = 0;
  end
end
       