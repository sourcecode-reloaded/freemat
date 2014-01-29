function result = test_line_markersize_001
a = figure;
p = plot(randn(100,1),'s-');
markersizes = {4,6,10,20,24};
result = false;
for i = 1:numel(markersizes)
  try
    set(p,'markersize',markersizes{i});
    draw
  catch
    lasterr
    return 
  end
end
close(a)
result = true;
