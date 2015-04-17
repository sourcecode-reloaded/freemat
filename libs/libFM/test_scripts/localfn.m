function z = localfn(x,y)
  z = triple(x) + triple(y)
  triple = 3;
  z = z + triple;
end

% This function should be hidden to the world, but accessible to localfn.
function a = triple(b)
    a = b + b + b;
    a = b - 2*b;
    a = double(b) + b;
end

function g = double(x)
    g = x + x;
end