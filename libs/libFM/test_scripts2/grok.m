classdef grok < bar
    properties
        grok1 = 9
    end
    methods
        function obj = grok(n)
            obj = obj@bar(n)
        end
    end
end