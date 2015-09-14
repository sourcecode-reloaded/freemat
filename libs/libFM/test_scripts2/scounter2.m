function [y,s] = scounter2(x)
    y = x;
    function p = incr
        y = y + 1;
        p = y;
    end
    function p = decr
        y = y - 1;
        p = y;
    end
    s = [];
    s.inc = @incr;
    s.dec = @decr;
end