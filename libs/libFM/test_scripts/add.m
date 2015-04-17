function y = add(a,b)
% 'a:'
%  a
%  'b:'
%  b
% keyboard
    a
    c = a + a
    b
    c = b + a
    c
    d = c - 3 ...
        + 4 - a + b
    for i=1:100
        d = d + i;
        e = d - 3;
        if (e < 0)
            d = d + e
            p = d + i;
        else
            e = e - 3;
            f = i + e;
        end
        q = d + e;
    end
  y = a + b;
  %  'y:'
  %  y
  