function result = test_line_colorspec_001
a = figure;
p = plot(randn(100,1));
colornames = {'y','yellow','m','magenta','c','cyan','r','red','g','green','b','blue','w','white','k','black'};
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
