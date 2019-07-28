clear all; close all; clc;

Fs=1000;

FirstExp=131;
LastExp=134;
n1=num2str(FirstExp);
n2=num2str(LastExp);

RawDataPath = fullfile( '..\Samples');
RresultsFilePath = fullfile( '..\Analysis');
Ground_TrouthFile=[RawDataPath, '\Ground_Truth.xlsx'];
Ground_Trouth = xlsread(Ground_TrouthFile);

for ExpNum=FirstExp:LastExp
    if Ground_Trouth(ExpNum)== -1
        s=': **** No Ground Trouth  ****';
    else
        NI_Scope_view( ExpNum )
   end
end



