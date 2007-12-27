function ft = timetest8

printf('Starting time trial...\n');
ft(1) = timeit('a=0;for j=1:10000; a=a+j;end; ');


function t = timeit(expr)
  printf('Testing %s\n',expr);
  tic;
    evalin('caller',['for i=1:3; ' expr ' end;']);
  t = toc;
  printf('Time trial %s is %f second\n',expr,t/3.);
  
function printf(varargin)
  disp(sprintf(varargin{:}));