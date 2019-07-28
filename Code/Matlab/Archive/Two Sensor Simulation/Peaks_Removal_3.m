function  [y]= Peaks_Removal_3(x)

y=x-mean(x); %DC removal

AmpLimit=10*median(abs(y));
index=find(y>AmpLimit);
y(index)=AmpLimit;
index=find(y< -AmpLimit);
y(index)= -AmpLimit;


% figure
% a=AmpLimit*ones(1,length(x));
% b=-a;
% plot(x,'r')
% hold
% plot(a,'g');
% plot(b,'y');
% plot(y,'b');

end


        
        
        
    
