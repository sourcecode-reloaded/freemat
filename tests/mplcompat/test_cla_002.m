function result = test_cla_002
result = false;
a = figure;
plot3(rand(15,1),rand(15,1),rand(15,1));
cla;
close(a);
result = true;
