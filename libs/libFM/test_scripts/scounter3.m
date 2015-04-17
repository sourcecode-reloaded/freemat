function [y,s] = scounter3(x)
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
    s.incr = @incr;
    s.decr = @decr;
end