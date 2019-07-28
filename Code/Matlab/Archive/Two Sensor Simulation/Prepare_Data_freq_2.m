function  [x1, x2]= Prepare_Data_freq_2( ExpNum )

ExpTxt= num2str(ExpNum);
RawDataPath = fullfile( '..\Samples');
RawDataFile=[RawDataPath, '\Experiment_', ExpTxt, '.xlsx'];

% ---- Read Sensors Samples and determine sampling frequency ------

num = xlsread(RawDataFile);

% --------- DC removal ------------------

x1=num(:,1);
x2=num(:,2);


end


        
        
        
    
