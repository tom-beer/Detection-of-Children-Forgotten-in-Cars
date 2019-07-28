clear all; close all; clc;

Fs=1000;

FirstExp=1;
LastExp=134;
n1=num2str(FirstExp);
n2=num2str(LastExp);

RawDataPath = fullfile( '..\Samples');
RresultsFilePath = fullfile( '..\Results');
ResultsFile=[RresultsFilePath, '\Experiments_', n1, '_to_', n2, '_Control_Results.txt'];
fid = fopen(ResultsFile, 'w');

Ground_TrouthFile=[RawDataPath, '\Ground_Truth.xlsx'];
Ground_Trouth = xlsread(Ground_TrouthFile);


stype = '*********** Energy Based Detection ***********';
fprintf('\n %s \n', stype);
fprintf(fid, '\r\n \r\n %s \r\n', stype);
miss=0;
falsea=0;
for ExpNum=FirstExp:LastExp
    if Ground_Trouth(ExpNum)== -1
        s=': **** No Ground Trouth  ****';
    else
%         [flag, prob]= Child_Detect_Ener_2_freq( ExpNum );
        [flag, prob]= Control_3( ExpNum );

        probstr=sprintf(': P=%3d%%', prob);

        if flag == 1
            if Ground_Trouth(ExpNum)== 1
                s=' Kid forgotten in the car';
            else
                s=' * FALSE * No kid in the car *';
                falsea=falsea+1;
            end
        else
            if Ground_Trouth(ExpNum)== 0
                s=' No kid in the car';
            else
                s=' * MISS * Kid FORGOTTEN in the car! *';
                miss=miss+1;
            end
        end
        entext=num2str(ExpNum);
        s1=[' Experiment_', entext, probstr, s];
        fprintf('%s \n', s1);
        fprintf(fid, '%s \r\n', s1);
    end
end


fprintf('\n******** Batch Results *********\n');
fprintf('Miss Detections %d False Alarms %d \n', miss, falsea);
fprintf(fid,'\r\n******** Batch Results *********\r\n');
fprintf(fid,'Miss Detections %d False Alarms %d \n', miss, falsea);
 
fclose(fid);


