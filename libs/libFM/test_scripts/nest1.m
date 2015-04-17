function x = nest1
% Free variables: none
% Captured variables: x, g, y, z
  x = 5;
  y = 7;
  
  function nestfun1
  % Free variables: x, g, y, z
  % Captured variables: k
      x = x + 1; % x is a free variable
      h = x - pi; % h and pi are not
      g = g + 1; 
      y = y - 3;
      k = 5;  % captured by nestfunc2 - only
    
      nestfunc2;
    
      function nestfunc2
      % Free variables: g, y, k, z
          g = g - 1; % g is a captured variable
          y = y + 4;
          k = k - y + z;          
      end
    
  end

  g = 32;
  z = 70;
  local = 56;
  nestfun1;

  local = local + z - g;
  
end