clear all; close all; clc;

FirstExp=1;
LastExp=135;
n1=num2str(FirstExp);
n2=num2str(LastExp);

Fs=1000;
cutoff = 12;
NumberOfCoeffs=250;

W=Wavelet;

RawDataPath = fullfile( '..\Samples');
SegmentsFilePath = fullfile( '..\Segments');
RresultsFilePath = fullfile( '..\Results');

ResultsFile=[RresultsFilePath, '\Experiments_', n1, '_to_', n2, '_Machine_Learning_Results.txt'];


Ground_TrouthFile=[RawDataPath, '\Ground_Truth.xlsx'];
Ground_Trouth = xlsread(Ground_TrouthFile);


segmentlen=1024;
WindowLen=4*segmentlen;

ptr=1; tcnt=1;
for ExpNum=FirstExp:LastExp
    if Ground_Trouth(ExpNum)== -1
        s=': **** No Ground Trouth  ****';
    else
        [y1, y2]= Prepare_Data_freq_2( ExpNum );
        len=length(y1);
        TestExp(tcnt)=ExpNum;
        for segment=1:segmentlen:len-WindowLen
            cord=segment:segment+WindowLen-1;
            y1(cord)=y1(cord)-mean(y1(cord)); % DC removal
            y1(cord)=y1(cord)/max(abs(y1(cord))); %Normalization
            s=y1(cord);
           

            m1=median(abs(s));
            index=find(abs(s)>4*(m1));
            l = length(index);
            r(ptr) = 100*length(index)/length(s);
            
%             fig=figure;
%             t=0:1/Fs:length(s)/Fs-1.0/Fs;
%             t=t+(segment/Fs)-1.0/Fs;
%             plot(t,s,'.g');
%             hold;
%             plot(t(index),s(index),'.r');
%             grid
%             ptrTxt= num2str(ptr);
%             SegTxt= num2str(floor(segment/segmentlen)+1);
%             TitleText=['ptr ', ptrTxt, ' segment ' SegTxt ];
%             title(TitleText);
%             xlabel('Time [Sec]');
%             if  l > 0
%                str1=sprintf('Peaks= %1.2f%%', r(ptr));
%                legend('Segmant', str1);
%             end

%             FigFile=[SegmentsFilePath, '\Experiment ', ExpTxt, ' Segment ' SegTxt];
%             print(fig,FigFile,'-dpng')
           
            TrainArr(1:WindowLen,ptr) = y1(cord);
%             TrainArr(1:WindowLen,ptr) = lpfft_ML(TrainArr(1:WindowLen,ptr), cutoff, Fs );
            RespArr(ptr) = Ground_Trouth(ExpNum);
            ExpArr(ptr) = ExpNum;
            if r(ptr) < 10
                ptr=ptr + 1;
            end
        end
        tcnt=tcnt+1
    end
end

NumberofPeaks=ptr;
if r(end) < 10
    NumberofPeaks=ptr-1;
end

cmap=colormap(hsv(NumberofPeaks));
figure
hold on;
for j=1:NumberofPeaks
    plot(TrainArr(:,j),'color',cmap(j,:));
end

species={'Kid In', 'No Kid'};
for j=1:NumberofPeaks
    WAV2(:,j)=W*(TrainArr(:,j));
end

figure
hold on;
index=find(RespArr==1);
for j=1:length(index)
    plot(WAV2(:,index(j)),'xr');
end

index=find(RespArr==0);
for j=1:length(index)
    plot(WAV2(:,index(j)),'og');
end
axis([0,NumberOfCoeffs,-5,5])
title('Signals Wavelet Coefficients - No Kid ');


for j=1:NumberofPeaks
    if RespArr(j) == 0
        species{j}='No Kid';
    else
        species{j}='Kid In';
    end
end


for tcnt=1:length(TestExp)
    
    index=find(ExpArr ~= TestExp(tcnt));
    X=WAV2(1:NumberOfCoeffs,index)';
    Y=species(index)';


    lda = fitcdiscr(X,Y);
    ldaClass = resubPredict(lda);
    [ldaResubCM,grpOrder] = confusionmat(Y,ldaClass);

    index=find(ExpArr == TestExp(tcnt));
    [label score cost] = predict(lda,WAV2(1:NumberOfCoeffs,index)');
    l=length(index);
    clear c;
    for j=1:l
        c(j)=strcmp(label(j),species(index(j)));
    end
    hit=sum(c);
    Success_Ratio(tcnt)=100*hit/l;
    [flag_out(tcnt), prob(tcnt) ]=Detect_with_hysteresis(label);
end
index=find(Ground_Trouth(TestExp)==0);
Success_Ratio(index)=100-Success_Ratio(index);

figure
% stem(TestExp,Success_Ratio,'r')
index=find(Ground_Trouth(TestExp)==1);
stem(TestExp(index),Success_Ratio(index),'r')
hold on;
o1=ones(1,length(TestExp));
m1=median(Success_Ratio(index));
plot(TestExp,o1*m1,'r');

index=find(Ground_Trouth(TestExp)==0);
stem(TestExp(index),Success_Ratio(index),'g')
m2=median(Success_Ratio(index));
plot(TestExp,o1*m2,'g');
% plot(TestExp,o1*median(Success_Ratio(index)),'g');
xlabel('Experiment Number');
ylabel('Prediction Success');
title('Single Sensor Machine Learning Raw Results');
s1=sprintf('median=%3.0f%%',m1);
s2=sprintf('median=%3.0f%%',m2);
legend('Kid In', s1, 'No Kid',s2);

% --- NEXT:  run "control" on c vector 

figure
index=find(Ground_Trouth(TestExp)==1);
stem(TestExp(index),prob(index),'r')
hold on;
o1=ones(1,length(TestExp));
m1=median(prob(index));
plot(TestExp,o1*double(m1),'r');

index=find(Ground_Trouth(TestExp)==0);
stem(TestExp(index),prob(index),'g')
m2=median(prob(index));
plot(TestExp,o1*double(m1),'g');
xlabel('Experiment Number');
ylabel('Prediction with Hystheresis Success');
title('Single Sensor Machine Learning Results with Hystheresis');
s1=sprintf('median=%3.0f%%',m1);
s2=sprintf('median=%3.0f%%',m2);
legend('Kid In', s1, 'No Kid',s2);

% --- print results to file -----

figure
hold on;
indexA=find(Ground_Trouth(TestExp)==1); % Kid in car
if isempty(indexA) ~=1
    index1=find(flag_out(indexA) == 1); % Found OK
    if isempty(index1) ~=1
        stem(TestExp(indexA(index1)),prob(indexA(index1)),'r')
    end
    index1=find(flag_out(indexA) == 0);  % Misdetection
    if isempty(index1) ~=1
        stem(TestExp(indexA(index1)),prob(indexA(index1)),'--r')
    end
end
indexA=find(Ground_Trouth(TestExp)==0); % No Kid in car
if isempty(indexA) ~=1
    index1=find(flag_out(indexA) == 0); % Found OK
    if isempty(index1) ~=1
        stem(TestExp(indexA(index1)),prob(indexA(index1)),'g')
    end
    index1=find(flag_out(indexA) == 1);  % False Alarm
    if isempty(index1) ~=1
        stem(TestExp(indexA(index1)),prob(indexA(index1)),'--g')
    end
end
xlabel('Experiment Number');
ylabel('Prediction with Hystheresis Success Ratio');
title('Single Sensor Machine Learning Results with Hystheresis');
legend('Kid In', 'Misdetection', 'No Kid', 'False Alarm');




fid = fopen(ResultsFile, 'w');
stype = '*********** Machine Learning Based Detection ***********';
fprintf('\n %s \n', stype);
fprintf(fid, '\r\n \r\n %s \r\n', stype);
miss=0;
falsea=0;

for tcnt=1:length(TestExp)
    probstr=sprintf(': P=%3d%%', prob(tcnt));

    if flag_out(tcnt) == 1
        if Ground_Trouth(TestExp(tcnt))== 1
            s=' Kid forgotten in the car';
        else
            s=' * FALSE * No kid in the car *';
            falsea=falsea + 1;
        end
    else
        if Ground_Trouth(TestExp(tcnt))== 0
            s=' No kid in the car';
        else
            s=' * MISS * Kid FORGOTTEN in the car! *';
            miss=miss+1;
        end
    end
    entext=num2str(TestExp(tcnt));
    s1=[' Experiment_', entext, probstr, s];
    fprintf('%s \n', s1);
    fprintf(fid, '%s \r\n', s1);
end

fprintf('\n******** Batch Results *********\n');
fprintf('Miss Detections %d False Alarms %d \n', miss, falsea);
fprintf(fid,'\r\n******** Batch Results *********\r\n');
fprintf(fid,'Miss Detections %d False Alarms %d \n', miss, falsea);

fclose(fid);




        
