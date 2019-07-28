function [ flag_out, prob ] = Detect_with_hysteresis(label)

KidInCnt=0;
KidInCar=0;
HystLevel=min(10, floor(length(label))); 
% Only for simulation of short recordings
% In implementation set HystLevel value to 10 or more 

for ptr=1:length(label)

    if strcmp(label(ptr),'Kid In') == 1
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
end
    
prob=int16((100*sum(flag))/(length(flag)-(HystLevel-1)));

if prob > 50
    flag_out=1;
else
    flag_out=0;
    prob=100-prob;
end

end

