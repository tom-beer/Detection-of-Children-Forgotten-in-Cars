function  [flag_out, prob]= Child_Detect_Ener_2_freq( ExpNum )

Threshold=0.7;

[y1, y2]= Prepare_Data_freq_2( ExpNum );

Fs=1000;
fftlen=1024;
segmentlen=fftlen;
WindowLen=4*segmentlen;
h=hamming(WindowLen);

ptr=1;
for segment=1:segmentlen:length(y1)-WindowLen

    cord=segment:segment+WindowLen-1;

    s1=y1(cord);
    s2=y2(cord);
%     s1=s1.*h;
%     s2=s2.*h;
    fener1 = lpfft_2( s1, 12, Fs );
    fener2 = lpfft_2( s2, 12, Fs );
    

    rf(ptr)=log10(fener1/fener2);

    if rf<Threshold
        flag(ptr)=0;
    else
        flag(ptr)=1;
    end

    ptr=ptr+1;

end

prob=int16(100*sum(flag)/length(flag));

if prob > 50
    flag_out=1;
else
    flag_out=0;
    prob=100-prob;
end

end

