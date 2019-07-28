function [flag_out, prob] = Control_3( ExpNum )

segmentlen=2048;
WindowLen=2*segmentlen;

[y1, y2]= Prepare_Data_freq_2( ExpNum );

KidInCnt=0;
KidInCar=0;
HystLevel=min(5, floor(length(y1)/segmentlen)-4); 
% Only for simulation of short recordings
% In implementation set HystLevel value to 10 or more 



ptr=1;
for segment=1:segmentlen:length(y1)-WindowLen

    cord=segment:segment+WindowLen-1;

    s1 = y1(cord);
    s2 = y2(cord);
    
    [flag_in(ptr), rf(ptr)]= Child_Detect_Ener_6_freq( s1, s2 );

    if flag_in(ptr) == 1
        KidInCnt=KidInCnt+1;
    else
        KidInCnt=KidInCnt-1;
    end
    
    if KidInCnt <=0
        KidInCnt=0;
        KidInCar=0;
    else
        if KidInCnt >= HystLevel
            KidInCnt=HystLevel;
            KidInCar=1;
        end
    end
    flag(ptr) = KidInCar;
    ptr = ptr + 1;
end
    
prob=int16((100*sum(flag))/(length(flag)-(HystLevel-1)));

if prob > 50
    flag_out=1;
else
    flag_out=0;
    prob=100-prob;
end
        
% figure
% n1=num2str(ExpNum);
% plot(rf,'g')
% hold
% plot(rf,'.b')
% index=find(rf<Threshold);
% plot(index,rf(index),'.r')
% plot(Threshold*ones(1,length(rf)),'y')
% s=['Experiment ', n1, ' sensors energies log ratio'];
% title( s );
% grid        

        


end

