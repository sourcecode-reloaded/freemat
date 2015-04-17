function b = exc(x)
a = 2
b = 7
try
    d = 4 + 5
    try
        try
            h = excb(a);
        catch
            'In Exception Handler 0'
        end
        e = d - g
        if ( a < 3)
            throw 'Hello!'
        end
        b = 45
    catch
        'In Exception Handler 1'
        b = 72
    end
catch
    'In Exception Handler 2'
    b = 32
end
b = b - 1
throw 'yuck!'
