function result = test_cla_001
result = false;
a = figure;
plot(rand(1,15));
cla
close(a);
result = true;
