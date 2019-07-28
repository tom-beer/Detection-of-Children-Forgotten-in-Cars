function [ fener, v ] = lpfft_3( sig, cutoff, Fs )

delta=Fs/length(sig);
f=0:delta:Fs/2;

% ------  Plotting ------------
% fplot=linspace(-Fs/2,Fs/2-delta,length(sig));
% F=fftshift(abs(fft(sig)));
% figure
% plot(fplot,F);

% ------ Filtering in the freq. domain ---------

SIG=fft(sig);
% index=find(f>cutoff);
% SIG(index)=0;
% index=length(SIG)-index;
% SIG(index)=0;

% ------ Plotting -----

% F=fftshift(abs(SIG));
% hold
% plot(fplot,F,'g');

% ------ Back to the time domain ----

% lpsig=ifft(SIG);

% ------ Plotting -----

% F=fftshift(abs(fft(lpsig)));
% plot(fplot,F,'r');
% title('lp Signal Spectrum (red)');

% --- Parseval ---------

index=find(f<cutoff);
S1=SIG(index(3:end)); % ignore DC component
fener=S1'*S1;

index=find((f >= 0.25) &  (f < 2.5));
e=SIG(index); 
v(1)=e'*e;
v(1)= v(1)/length(index);

index=find((f >= 2.5) &  (f < 6));
e=SIG(index); 
v(2)=e'*e;
v(2)= v(2)/length(index);

index=find((f >= 6) &  (f < cutoff));
e=SIG(index); 
v(3)=e'*e;
v(3)= v(3)/length(index);


% index=find((f >= 0.25) &  (f < cutoff));
% fener/length(index)
% sum(v)

v=v/max(v);


end

