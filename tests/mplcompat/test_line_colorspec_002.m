function result = test_line_colorspec_002
a = figure;
p = plot(randn(100,1));
colornames = {'none'}
result = false;
for i = 1:numel(colornames)
  try
    set(p,'color',colornames{i});
    draw
  catch
    lasterr
    return 
  end
end
close(a)
result = true;
