function [] = DisplayResults(TestExp, Success_Ratio, prob, flag_out, simParams)


%%
rawDataPath      = fullfile( '..\Samples');
groundTrouthFile = [rawDataPath, '\Ground_Truth.xlsx'];
groundTrouth     = xlsread(groundTrouthFile);

if simParams.general.drawPlots
    cmap = colormap(hsv(numSignals));
    hold on;
    for j=1:numSignals
        plot(TrainArr(:,j),'color',cmap(j,:));
    end
    
    
    index = find(groundTrouth(TestExp)==1);
    % if simParams.general.drawPlots
    figure
    % stem(TestExp,Success_Ratio,'r')
    stem(TestExp(index),Success_Ratio(index),'r')
    hold on;
    o1 = ones(1,length(TestExp));
    m1 = median(Success_Ratio(index));
    plot(TestExp,o1*m1,'r');
    % end
    index = find(groundTrouth(TestExp)==0);
    stem(TestExp(index),Success_Ratio(index),'g')
    m2 = median(Success_Ratio(index));
    plot(TestExp,o1*m2,'g');
    % plot(TestExp,o1*median(Success_Ratio(index)),'g');
    xlabel('Experiment Number');
    ylabel('Prediction Success');
    title('Single Sensor Machine Learning Raw Results');
    s1 = sprintf('median=%3.0f%%',m1);
    s2 = sprintf('median=%3.0f%%',m2);
    legend('Kid In', s1, 'No Kid',s2);
end

% --- NEXT:  run "control" on c vector
index = find(groundTrouth(TestExp)==1);
if simParams.general.drawPlots
    figure
    stem(TestExp(index),prob(index),'r')
    hold on;
    o1 = ones(1,length(TestExp));
    m1 = median(prob(index));
    plot(TestExp,o1*double(m1),'r');
    
    
    index=find(groundTrouth(TestExp)==0);
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
    indexA=find(groundTrouth(TestExp)==1); % Kid in car
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
    
    indexA=find(groundTrouth(TestExp)==0); % No Kid in car
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
end


resultsFilePath  = fullfile( '..\Results');
resultsFile      = [resultsFilePath, '\Experiments_', num2str(simParams.general.firstExp), '_to_', num2str(simParams.general.lastExp),...
    '_Machine_Learning_Results.txt'];
fid = fopen(resultsFile, 'w');
stype = '*********** Machine Learning Based Detection ***********';
fprintf('\n %s \n', stype);
fprintf(fid, '\r\n \r\n %s \r\n', stype);
miss=0;
falsea=0;

for tcnt=1:length(TestExp)
    probstr=sprintf(': P=%3d%%', prob(tcnt));
    
    if flag_out(tcnt) == 1
        if groundTrouth(TestExp(tcnt))== 1
            s=' Kid forgotten in the car';
        else
            s=' * FALSE * No kid in the car *';
            falsea=falsea + 1;
        end
    else
        if groundTrouth(TestExp(tcnt))== 0
            s=' No kid in the car';
        else
            s=' * MISS * Kid FORGOTTEN in the car! *';
            miss=miss+1;
        end
    end
    entext=num2str(TestExp(tcnt));
    s1=[' Experiment_', entext, probstr, s];
%     fprintf('%s \n', s1);
%     fprintf(fid, '%s \r\n', s1);
end

fprintf('\n******** Batch Results *********\n');
fprintf('Miss Detections %d False Alarms %d \n', miss, falsea);
fprintf(fid,'\r\n******** Batch Results *********\r\n');
fprintf(fid,'Miss Detections %d False Alarms %d \n', miss, falsea);

fclose(fid);

end