function result = test_clf_002
result = false;
a = figure;
plot3(rand(15,1),rand(15,1),rand(15,1));
clf;
close(a);
result = true;
