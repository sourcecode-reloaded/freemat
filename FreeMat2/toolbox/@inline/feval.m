function varargout = feval(varargin)
  if (~isa(varargin{1},'inline'))
    [varargout{1:max(1,nargout)}] = builtin('feval',varargin{:});
  else
    t = varargin{1};
    varargout = {inline_evaluate(t.expr,t.assign_args,varargin(2:end))};
    keyboard
  end
  
