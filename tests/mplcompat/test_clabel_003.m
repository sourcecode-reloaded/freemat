function result = test_clabel_003
result = false;
a = figure;
h = contour(peaks);
clabel(h,'backgroundcolor',[1,1,.6],'edgecolor',[.7,.7,.7]);
close(a);
result = true;
