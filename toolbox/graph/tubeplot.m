% TUBEPLOT TUBEPLOT Creates a Tubeplot
% 
% Usage
% 
% This tubeplot function is from the tubeplot package
% written by Anders Sandberg. The simplest syntax for the
% tubeplot routine is
% 
%     tubeplot(x,y,z)
% 
% plots the basic tube with radius 1, where x,y,z are
% vectors that describe the tube.  If the radius of the
% tube is to be varied, use the second form
% 
%     tubeplot(x,y,z,r) 
% 
% which plots the basic tube with variable radius r (either 
% a vector or a scalar value).  The third form allows you
% to specify the coloring using a vector of values:
% 
%     tubeplot(x,y,z,r,v)
% 
% where the coloring is now dependent on the values in the 
% vector v.  If you want to create a tube plot with 
% a greater degree of tangential subdivisions (i.e.,
% the tube is more circular, use the form
% 
%     tubeplot(x,y,z,r,v,s)
% 
% where s is the number of tangential subdivisions (default is 6)
% You can also use tubeplot to calculate matrices to feed to mesh
% and surf.
% 
%     [X,Y,Z]=tubeplot(x,y,z)
% 
% returns N x 3 matrices suitable for mesh or surf.
% 
% Note that the tube may pinch at points where the normal and binormal 
% misbehaves. It is suitable for general space curves, not ones that 
% contain straight sections. Normally the tube is calculated using the
% Frenet frame, making the tube minimally twisted except at inflexion points.
% 
% To deal with this problem there is an alternative frame:
% 
%     tubeplot(x,y,z,r,v,s,vec)
% 
% calculates the tube by setting the normal to
% the cross product of the tangent and the vector vec. If it is chosen so 
% that it is always far from the tangent vector the frame will not twist unduly.
% TUBEPLOT TUBEPLOT Creates a Tubeplot
% 
% Usage
% 
% This tubeplot function is from the tubeplot package
% written by Anders Sandberg. The simplest syntax for the
% tubeplot routine is
% 
%     tubeplot(x,y,z)
% 
% plots the basic tube with radius 1, where x,y,z are
% vectors that describe the tube.  If the radius of the
% tube is to be varied, use the second form
% 
%     tubeplot(x,y,z,r) 
% 
% which plots the basic tube with variable radius r (either 
% a vector or a scalar value).  The third form allows you
% to specify the coloring using a vector of values:
% 
%     tubeplot(x,y,z,r,v)
% 
% where the coloring is now dependent on the values in the 
% vector v.  If you want to create a tube plot with 
% a greater degree of tangential subdivisions (i.e.,
% the tube is more circular, use the form
% 
%     tubeplot(x,y,z,r,v,s)
% 
% where s is the number of tangential subdivisions (default is 6)
% You can also use tubeplot to calculate matrices to feed to mesh
% and surf.
% 
%     [X,Y,Z]=tubeplot(x,y,z)
% 
% returns N x 3 matrices suitable for mesh or surf.
% 
% Note that the tube may pinch at points where the normal and binormal 
% misbehaves. It is suitable for general space curves, not ones that 
% contain straight sections. Normally the tube is calculated using the
% Frenet frame, making the tube minimally twisted except at inflexion points.
% 
% To deal with this problem there is an alternative frame:
% 
%     tubeplot(x,y,z,r,v,s,vec)
% 
% calculates the tube by setting the normal to
% the cross product of the tangent and the vector vec. If it is chosen so 
% that it is always far from the tangent vector the frame will not twist unduly.
function [varargout]=tubeplot(x,y,z,varargin)  
  subdivs = 6;

  N=size(x,1);
  if (N==1)
    x=x';
    y=y';
    z=z';
    N=size(x,1);
  end

  if (nargin == 3)
    r=x*0+1;
  else
    r=varargin{1};
    if (size(r,1)==1 & size(r,2)==1)
      r=r*ones(N,1);
    end
  end
  if (nargin > 5)
    subdivs=varargin{3}+1;
  end
  if (nargin > 6)
    vec=varargin{4};
    [t,n,b]=frame(x,y,z,vec);
  else
    [t,n,b]=frenet(x,y,z);
  end

  X=zeros(N,subdivs);
  Y=zeros(N,subdivs);
  Z=zeros(N,subdivs);

  theta=0:(2*pi/(subdivs-1)):(2*pi);

  for i=1:N
    X(i,:)=x(i) + r(i)*(n(i,1)*cos(theta) + b(i,1)*sin(theta));
    Y(i,:)=y(i) + r(i)*(n(i,2)*cos(theta) + b(i,2)*sin(theta));
    Z(i,:)=z(i) + r(i)*(n(i,3)*cos(theta) + b(i,3)*sin(theta));
  end

  if (nargout==0)
    if (nargin > 4)
      V=varargin{2};
      if (size(V,1)==1)
	V=V';
      end
      V=V*ones(1,subdivs);
      surf(X,Y,Z,V);
    else
      surf(X,Y,Z);
    end
  else
    varargout(1) = {X}; 
    varargout(2) = {Y}; 
    varargout(3) = {Z}; 
  end

function [t,n,b]=frame(x,y,z,vec)

% FRAME Calculate a Frenet-like frame for a polygonal space curve
% [t,n,b]=frame(x,y,z,v) returns the tangent unit vector, a normal
% and a binormal of the space curve x,y,z. The curve may be a row or
% column vector, the frame vectors are each row vectors. 
%
% This function calculates the normal by taking the cross product
% of the tangent with the vector v; if v is chosen so that it is
% always far from t the frame will not twist unduly.
% 
% If two points coincide, the previous tangent and normal will be used.
%
% Written by Anders Sandberg, asa@nada.kth.se, 2005


N=size(x,1);
if (N==1)
  x=x';
  y=y';
  z=z';
  N=size(x,1);
end

t=zeros(N,3);
b=zeros(N,3);
n=zeros(N,3);

p=[x y z];

for i=2:(N-1)
  t(i,:)=(p(i+1,:)-p(i-1,:));
  tl=norm(t(i,:));
  if (tl>0)
    t(i,:)=t(i,:)/tl;
  else
    t(i,:)=t(i-1,:);
  end
end

t(1,:)=p(2,:)-p(1,:);
t(1,:)=t(1,:)/norm(t(1,:));

t(N,:)=p(N,:)-p(N-1,:);
t(N,:)=t(N,:)/norm(t(N,:));

for i=2:(N-1)
  n(i,:)=cross(t(i,:),vec);
  nl=norm(n(i,:));
  if (nl>0)
    n(i,:)=n(i,:)/nl;
  else
    n(i,:)=n(i-1,:);
  end
end

n(1,:)=cross(t(1,:),vec);
n(1,:)=n(1,:)/norm(n(1,:));

n(N,:)=cross(t(N,:),vec);
n(N,:)=n(N,:)/norm(n(N,:));

for i=1:N
  b(i,:)=cross(t(i,:),n(i,:));
  b(i,:)=b(i,:)/norm(b(i,:));
end

function [t,n,b]=frenet(x,y,z)

% FRENET Calculate the Frenet frame for a polygonal space curve
% [t,n,b]=frenet(x,y,z) returns the tangent unit vector, the normal
% and binormal of the space curve x,y,z. The curve may be a row or
% column vector, the frame vectors are each row vectors. 
%
% If two points coincide, the previous tangent and normal will be used.
%
% Written by Anders Sandberg, asa@nada.kth.se, 2005

N=size(x,1);
if (N==1)
  x=x';
  y=y';
  z=z';
  N=size(x,1);
end

t=zeros(N,3);
b=zeros(N,3);
n=zeros(N,3);

p=[x y z];

for i=2:(N-1)
  t(i,:)=(p(i+1,:)-p(i-1,:));
  tl=norm(t(i,:));
  if (tl>0)
    t(i,:)=t(i,:)/tl;
  else
    t(i,:)=t(i-1,:);
  end
end

t(1,:)=p(2,:)-p(1,:);
t(1,:)=t(1,:)/norm(t(1,:));

t(N,:)=p(N,:)-p(N-1,:);
t(N,:)=t(N,:)/norm(t(N,:));

for i=2:(N-1)
  n(i,:)=(t(i+1,:)-t(i-1,:));
  nl=norm(n(i,:));
  if (nl>0)
    n(i,:)=n(i,:)/nl;
  else
    n(i,:)=n(i-1,:);
  end
end

n(1,:)=t(2,:)-t(1,:);
n(1,:)=n(1,:)/norm(n(1,:));

n(N,:)=t(N,:)-t(N-1,:);
n(N,:)=n(N,:)/norm(n(N,:));

for i=1:N
  b(i,:)=cross(t(i,:),n(i,:));
  b(i,:)=b(i,:)/norm(b(i,:));
end


