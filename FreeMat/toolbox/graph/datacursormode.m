function varargout=datacursormode(varargin)
    if nargin == 1
        if strcmp(varargin{1}, 'on') 
            varargout{1} = datacursormanager();
        end
    end
