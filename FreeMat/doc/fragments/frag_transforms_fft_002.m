0
delta(1:10) = 1;
plot((0:255)/256*pi*2,real(fft(delta,256)),'r-');
hold on
plot((0:9)/10*pi*2,real(fft(delta)),'go');
mprint('fft2');
