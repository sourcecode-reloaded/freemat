classdef listener < base_handle
    properties
        Source
        EventName
        Callback
        Enabled = 1
        Recursive = 0
    end
    methods
        function obj = listener(source,event,funcptr)
            obj.Source = source;
            obj.EventName = event;
            obj.Callback = funcptr;
        end
    end
end
