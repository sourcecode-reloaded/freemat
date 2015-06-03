classdef handle < base_handle
    properties
        listeners
    end
    methods
        function addlistener(obj,event,funcptr)
        % TODO - verify that event is valid
            if (is_valid_event(obj,event))
                obj.listeners.(event) = listener(obj,event,funcptr)
            else
                throw ['Cannot use ',event,' as an event for this class']
            end
        end
        function notify(obj,event)
        %            for i=1:numel(obj.listeners)
                %                if (listeners{i}.EventName == 
                obj.listeners.(event).Callback(obj,event)
                %   end
        end
    end
end
        