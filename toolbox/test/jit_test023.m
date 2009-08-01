function A = jit_test023
  A = zeros(1,100000);
  for i=1:numel(A);
    A(i) = i/5.0;
  end
