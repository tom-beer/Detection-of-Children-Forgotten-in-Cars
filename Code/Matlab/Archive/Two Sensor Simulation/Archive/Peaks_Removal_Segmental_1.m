function  [y]= Peaks_Removal_Segmental_1(x)

y=x;

fftlen=1024;
segmentlen=fftlen;
WindowLen=4*segmentlen;

for segment=1:segmentlen:length(y)-WindowLen

    cord=segment:segment+WindowLen-1;
    cord1=segment:segment+segmentlen-1;

    s1 = y(cord);
    s1 = Peaks_Removal_3(s1);
    y(cord1)=s1(1:segmentlen);

end

end


        
        
        
    
