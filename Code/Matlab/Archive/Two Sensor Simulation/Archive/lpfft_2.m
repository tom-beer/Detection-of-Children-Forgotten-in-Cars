function [ fener ] = lpfft_2( sig, cutoff, Fs )

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
S1=SIG(index(2:end)); % ignore DC component
fener=S1'*S1;

end

