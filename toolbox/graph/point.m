% POINT POINT Get Axis Position From Mouse Click
% 
% Usage
% 
% Returns information about the currently displayed image based on a use
% supplied mouse-click.  The general syntax for its use is
% 
%    t = point
% 
% The returned vector y has two elements: 
% where x,y are the coordinates in the current axes of the click.  This 
% function has changed since FreeMat 1.10.  If the click is not inside the
% active area of any set of axes, a pair of NaNs are returned.
% POINT POINT Get Axis Position From Mouse Click
% 
% Usage
% 
% Returns information about the currently displayed image based on a use
% supplied mouse-click.  The general syntax for its use is
% 
%    t = point
% 
% The returned vector y has two elements: 
% where x,y are the coordinates in the current axes of the click.  This 
% function has changed since FreeMat 1.10.  If the click is not inside the
% active area of any set of axes, a pair of NaNs are returned.

% Copyright (c) 2002-2006 Samit Basu
% Licensed under the GPL

function t = point
p = hpoint;
% Convert p to a fractional coordinate
s = get(gcf,'figsize'); s = s(:);
p = p./s;
p(2) = 1-p(2);
% Get the list ofchildren
children = get(gcf,'children');
% Check us agains each child
for i=1:numel(children)
  position = get(children(i),'position');
  if (hitTest(position,p))
    xlims = get(children(i),'xlim');
    ylims = get(children(i),'ylim');
    tpos(1) = xlims(1) + (p(1)-position(1))/position(3)*(xlims(2)-xlims(1));
    tpos(2) = ylims(1) + (p(2)-position(2))/position(4)*(ylims(2)-ylims(1));
    t = tpos;
    return;
  end
end
t = [nan,nan];

function b = hitTest(rect,p)
if ((p(1) >= rect(1)) && (p(1) <= (rect(1)+rect(3))) && (p(2) >= rect(2)) && (p(2) <= (rect(2)+rect(4))))
  b = 1;
else
  b = 0;
end
