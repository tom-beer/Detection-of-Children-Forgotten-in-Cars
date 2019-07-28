function [flag_out, prob, labelOut] = Detect_with_hysteresis(labels,HystLevel)

KidInCnt = 0;
KidInCar = 0;
if ~exist('HystLevel','var')
   HystLevel = 10; 
end
HystLevel = min(HystLevel, floor(length(labels))); 
flag = NaN(1,length(labels));
labelOut = labels;

for ptr = 1:length(labels)

    if strcmp(labels(ptr),'Kid In') == 1
        KidInCnt = KidInCnt+1;
    else
        KidInCnt = KidInCnt-1;
    end
    
    if KidInCnt <= 0
        KidInCnt = 0;
        KidInCar = 0;
    else
        if KidInCnt >= HystLevel
            KidInCnt = HystLevel;
            KidInCar = 1;
        end
    end
    flag(ptr) = KidInCar;
    if KidInCar == 1
        labelOut{ptr} = 'Kid In';
    else
        labelOut{ptr} = 'No Kid';
    end
end
    
prob=int16((100*sum(flag))/(length(flag)-(HystLevel-1)));

if prob > 50
    flag_out = 1;
else
    flag_out = 0;
    prob = 100-prob;
end

end

