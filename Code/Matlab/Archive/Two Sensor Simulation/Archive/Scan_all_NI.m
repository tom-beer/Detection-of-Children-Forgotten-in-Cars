clear all; close all; clc;

Fs=1000;

FirstExp=100;
LastExp=130;
n1=num2str(FirstExp);
n2=num2str(LastExp);

RawDataPath = fullfile( '..\Samples');
RresultsFilePath = fullfile( '..\Analysis');

for ExpNum=FirstExp:LastExp
        NI_Scope_view( ExpNum )
end



