function result = test_line_marker_001
a = figure;
p = plot(randn(100,1));
markers = {'+','o','*','.','x','square','s','diamond','d','^','v','>','<'};
result = false;
for i = 1:numel(markers)
  try
    set(p,'marker',markers{i});
    if (~strcmp(get(p,'marker')(1),markers{i}(1)))
       printf('Unable to use marker type %s\n',markers{i})
       return
    end
    draw
  catch
    lasterr
    return 
  end
end
close(a)
result = true;
