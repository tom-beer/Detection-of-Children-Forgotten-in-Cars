expNum = 10;
filename = ['..\..\Data\New Data\nokid\Driving bag\experiment_' num2str(expNum) '.xlsx'];
signal1 = xlsread(filename);
signal1 = signal1(:,1);
expNum = 11;
filename = ['..\..\Data\New Data\kid\Drivinggrownup\experiment_' num2str(expNum) '.xlsx'];
signal2 = xlsread(filename);
signal2 = signal2(:,1);
% expNum = 14;
% filename = ['..\..\Data\New Data\nokid\Driving car\experiment_' num2str(expNum) '.xlsx'];
% signal3 = xlsread(filename);
% signal3 = signal3(:,1);
%%
figure; 
ax1 = subplot(2,1,1);
plot(signal1)
% xlabel('Time [samples]'); ylabel('Amplitude'); 
% ylim([-1.7 1.7]);
title('Bag')
ax2 = subplot(2,1,2);
plot(signal2)
% xlabel('Time [samples]'); ylabel('Amplitude'); 
% ylim([-1.7 1.7]);
title('Passenger')
% subplot 313
% plot(signal3)
% xlabel('Time [samples]'); ylabel('Amplitude'); 
% ylim([-1.7 1.7]); title('Experiment 14')
suptitle('Comparison of different kid signals');
linkaxes([ax1,ax2],'x')

%% Count minutes of recording
oldDir = cd;
folder = 'C:\Users\Tom\Dropbox\Tom_Li\Data\New Data\Kid\drivingGrownUp';
numSamples = 0;
cd(folder)
recDir = dir('*.xlsx');
for iExp = 1:numel(recDir)
    filename = fullfile(folder, recDir(iExp).name);
    signal = xlsread(filename);
    signal = signal(:,1);
    numSamples = numSamples + numel(signal);
    disp([num2str(iExp) ' out of ' num2str(numel(recDir))])
end
disp([ num2str(numSamples / (1000*60)) ' Minutes'])
cd(oldDir)