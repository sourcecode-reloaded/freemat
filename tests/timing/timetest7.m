function ft = timetest7

printf('Starting time trial...\n');
ft(1) = timeit('b = toep();');


function t = timeit(expr)
  printf('Testing %s\n',expr);
  tic;
    evalin('caller',['for i=1:3; ' expr ' end;']);
  t = toc;
  printf('Time trial %s is %f second\n',expr,t/3.);
  
function printf(varargin)
  disp(sprintf(varargin{:}));
