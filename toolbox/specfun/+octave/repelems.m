function y = repelems(X,R)
% refactor if required for performance
  y = [];
  for i=1:size(R,2)
      y = [y, X(R(1,i)*ones(1,R(2,i)))(:)'];
  end
