function  prepare_samples_h_file( ExpNum )

n1=num2str(ExpNum);
TextSamplesFilePath = fullfile( '..\TextSamples');
Sensor1Text=[TextSamplesFilePath, '\Experiment_', n1, '_Sensor1.h'];
Sensor2Text=[TextSamplesFilePath, '\Experiment_', n1, '_Sensor2.h'];


[y1, y2]= Prepare_Data_freq_2( ExpNum );

% print N samples to a text files
N=32;
fid1 = fopen(Sensor1Text, 'w');
fid2 = fopen(Sensor2Text, 'w');

fprintf (fid1,'Sensor1[]={\n');
fprintf (fid2,'Sensor2[]={\n');

for j=1:1:N-1
    fprintf(fid1,'%2.5f, ', y1(j));
    fprintf(fid2,'%2.5f, ', y2(j));

    if mod(j,5)==0
        fprintf (fid1,'\n');
        fprintf (fid2,'\n');
    end
end

fprintf(fid1,'%2.5f\n};', y1(N));
fprintf(fid2,'%2.5f\n};', y2(N));

fclose(fid1);
fclose(fid2);



end

