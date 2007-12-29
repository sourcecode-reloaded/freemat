function a=toep()
a=zeros(500,500);
for k1=1:500
	for k2=1:500
		a(k1,k2)=(k1-k2);
	end
end
