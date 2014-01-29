function run_tests(subset)
if (nargin == 1)
  a = dir(['test_' subset '.m']);
else
  a = dir('test*.m');
end
results = {};
for i=1:numel(a)
  [thedir,thefilename,thext] = fileparts(a(i).name);
  success = 0;
  try
    success = feval(thefilename);
  catch
    close all
    success = 0;
  end;
  results{i,1} = thefilename;
  results{i,2} = success;
end

printf('\n\n');
printf('***Passes***\n\n')
for i=1:size(results,1)
   if (results{i,2})
     printf('Test %s result %d\n',results{i,1},results{i,2})
   end
end
printf('***Failed***\n\n')
for i=1:size(results,1)
   if (~results{i,2})
     printf('Test %s result %d\n',results{i,1},results{i,2})
   end
end

