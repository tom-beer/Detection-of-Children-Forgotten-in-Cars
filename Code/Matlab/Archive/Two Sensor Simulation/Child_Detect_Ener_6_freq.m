function  [flag_out, rf]= Child_Detect_Ener_6_freq( s1, s2)

Threshold=0.7;

Fs=1000;
fftlen=1024;
segmentlen=fftlen;
WindowLen=4*segmentlen;
h=hamming(WindowLen);
   
s1 = Peaks_Removal_3(s1);
s2 = Peaks_Removal_3(s2);
    
s1 = s1.*h;
s2 = s2.*h;
    
[fener1, v1] = lpfft_3( s1, 12, Fs );
[fener2, ~] = lpfft_3( s2, 12, Fs );
    
% fprintf('v1= %1.2f %1.2f %1.2f \n', v1(1), v1(2), v1(3));

rf=log10(fener1/fener2);

flag_out=0;
if (rf > Threshold) && (v1(1)>v1(3))
     flag_out=1;
end

end


