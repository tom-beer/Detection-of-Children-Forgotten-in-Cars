function  NI_Scope_view( ExpNum )
close all;
ExpTxt= num2str(ExpNum);
RawDataPath = fullfile( '..\Samples');
RresultsFilePath = fullfile( '..\Analysis');
RawDataFile=[RawDataPath, '\Experiment_', ExpTxt, '.xlsx'];
ResultsFile=[RresultsFilePath, '\Experiment ', ExpTxt, '.txt'];
% fid = fopen(ResultsFile, 'w');


num = xlsread(RawDataFile);
% Fs=1.0/(num(2,1)-num(1,1));
Fs=1000;


fig=figure;
t=0:1/Fs:length(num)/Fs-1.0/Fs;
plot(t,num(:,1))
grid
TitleText=['Experiment ', ExpTxt, ' Ch. 1 raw data'];
title(TitleText);
xlabel('Time [Sec]');
FigFile=[RresultsFilePath, '\Experiment ', ExpTxt, ' Ch. 1 raw data.png'];
print(fig,FigFile,'-dpng')

fig=figure;
plot(t,num(:,2),'r')
grid
TitleText=['Experiment ', ExpTxt, ' Ch. 2 raw data'];
title(TitleText);
xlabel('Time [Sec]');
FigFile=[RresultsFilePath, '\Experiment ', ExpTxt, ' Ch. 2 raw data.png'];
print(fig,FigFile,'-dpng')


x1=num(:,1)-mean(num(:,1)); % DC removal
x2=num(:,2)-mean(num(:,2)); % DC removal

x1pr=Peaks_Removal_Segmental_1(x1);
x2pr=Peaks_Removal_Segmental_1(x2);


fig=figure;
t=0:1/Fs:length(num)/Fs-1.0/Fs;
plot(t,x1pr)
grid
TitleText=['Experiment ', ExpTxt, ' Ch. 1 Peaks Removed'];
title(TitleText);
xlabel('Time [Sec]');
FigFile=[RresultsFilePath, '\Experiment ', ExpTxt, ' Ch. 1 Peaks Removed.png'];
print(fig,FigFile,'-dpng')

fig=figure;
plot(t,x2pr,'r')
grid
TitleText=['Experiment ', ExpTxt, ' Ch. 2 Peaks Removed'];
title(TitleText);
xlabel('Time [Sec]');
FigFile=[RresultsFilePath, '\Experiment ', ExpTxt, ' Ch. 2 Peaks Removed.png'];
print(fig,FigFile,'-dpng')


% f=-Fs/2:Fs/length(num(:,1)):Fs/2-Fs/length(num(:,1));
% l=length(f);
% MaxFreq2Display=40;
% p=round(l*MaxFreq2Display/(Fs));
% ind=l/2+1:l/2+1+p;
% 
% fig=figure;
% F=fftshift(abs(fft(x1)));
% plot(f(ind), F(ind) ,'b');
% TitleText=['Experiment ', ExpTxt, ' Ch. 1 abs frequency'];
% title(TitleText);
% xlabel('Frequency [Hz]');
% grid
% FigFile=[RresultsFilePath, '\Experiment ', ExpTxt, ' Ch. 1 abs frequency.png'];
% print(fig,FigFile,'-dpng')
% 
% fig=figure;
% F=fftshift(abs(fft(x2)));
% plot(f(ind), F(ind) ,'r');
% TitleText=['Experiment ', ExpTxt, ' Ch. 2 abs frequency'];
% title(TitleText);
% xlabel('Frequency [Hz]');
% grid
% FigFile=[RresultsFilePath, '\Experiment ', ExpTxt, ' Ch. 2 abs frequency.png'];
% print(fig,FigFile,'-dpng')
% 
% % ------ Design LPF ---------
% 
% LPF10 = lpf10fir(Fs, 10, 20, 40); % Sampling freq, passband, stopband, attenuation
% 
% % ----------------------------
% y1 = step(LPF10,x1);
% y2 = step(LPF10,x2);
% 
% 
% fig=figure;
% TitleText=['Experiment ', ExpTxt, ' Ch. 1 after passing LPF'];
% FigFile=[RresultsFilePath, '\', TitleText, '.png'];
% plot(t,y1,'b')
% title(TitleText);
% xlabel('Time [Sec]');
% grid
% print(fig,FigFile,'-dpng')
% 
% fig=figure;
% TitleText=['Experiment ', ExpTxt, ' Ch. 1 after passing LPF abs frequency (DC removed)'];
% FigFile=[RresultsFilePath, '\', TitleText, '.png'];
% F=fftshift(abs(fft(y1)));
% plot(f(ind), F(ind) ,'b');
% xlabel('Frequency [Hz]');
% title(TitleText);
% grid
% print(fig,FigFile,'-dpng')
% 
% fig=figure;
% TitleText=['Experiment ', ExpTxt, ' Ch. 2 after passing  LPF'];
% FigFile=[RresultsFilePath, '\', TitleText, '.png'];
% plot(t,y2,'r')
% title(TitleText);
% xlabel('Time [Sec]');
% grid
% print(fig,FigFile,'-dpng')
% 
% 
% 
% fig=figure;
% TitleText=['Experiment ', ExpTxt, ' Ch. 2 after passing LPF abs frequency (DC removed)'];
% FigFile=[RresultsFilePath, '\', TitleText, '.png'];
% F=fftshift(abs(fft(y2)));
% plot(f(ind), F(ind) ,'r');
% xlabel('Frequency [Hz]');
% title(TitleText);
% grid
% print(fig,FigFile,'-dpng')
% 
% 
% % --- Define Bands ---
% 
% p1=[round(l*0.4/(Fs)):round(l*2.5/(Fs))]+l/2+1;
% p2=[round(l*2.5/(Fs)):round(l*6.0/(Fs))]+l/2+1;
% p3=[round(l*6.0/(Fs)):round(l*12.0/(Fs))]+l/2+1;
% 
% % --- band 1  energy ---
% 
% F=fftshift(abs(fft(y1)));
% F1=F(p1); F2=F(p2); F3=F(p3);
% fig=figure;
% TitleText=['Experiment ', ExpTxt, ' Ch.1 frequency bands'];
% FigFile=[RresultsFilePath, '\', TitleText, '.png'];
% F=fftshift(abs(fft(y2)));
% plot(f(p1),F1);
% hold
% plot(f(p2),F2,'r');
% plot(f(p3),F3,'g');
% title(TitleText);
% legend(' Band 1 ', ' Band 2 ' , ' Band 3 ')
% grid
% print(fig,FigFile,'-dpng')
% 
% F1pow=sum(abs(F1).^2)/length(p1); 
% F2pow=sum(abs(F2).^2)/length(p2); 
% F3pow=sum(abs(F3).^2)/length(p3);
% e1t=y1'*y1;
% fprintf('Ch1 total energy in the time domain =\n %6.0f\n', e1t);
% fprintf('Ch. 1 bands energy = \n %8.0f \n %8.0f \n %8.0f \n', F1pow, F2pow, F3pow);
% 
% fprintf(fid,'\r\n******** Experiment_%d Channel 1 results ******\r\n', ExpNum);
% fprintf(fid,'Total energy in the time domain = %6.0f \r\n', e1t);
% fprintf(fid,'Normalized bands energies in the freq domain = %8.0f  %8.0f  %8.0f \r\n', F1pow, F2pow, F3pow);
% 
% % --- band 2  energy ---
% 
% 
% F=fftshift(abs(fft(y2)));
% F1=F(p1); F2=F(p2); F3=F(p3);
% fig=figure;
% TitleText=['Experiment ', ExpTxt, ' Ch.2 frequency bands'];
% FigFile=[RresultsFilePath, '\', TitleText, '.png'];
% plot(f(p1),F1);
% hold
% plot(f(p2),F2,'r');
% plot(f(p3),F3,'g');
% title(TitleText);
% legend(' Band 1 ', ' Band 2 ' , ' Band 3 ')
% grid
% print(fig,FigFile,'-dpng')
% 
% F1pow=sum(abs(F1).^2)/length(p1); 
% F2pow=sum(abs(F2).^2)/length(p2); 
% F3pow=sum(abs(F3).^2)/length(p3);
% e2t=y2'*y2;
% fprintf('Ch2 total energy in the time domain =\n %6.0f\n', e2t);
% fprintf('bands energy = \n %8.0f \n %8.0f \n %8.0f \n', F1pow, F2pow, F3pow);
% 
% fprintf(fid,'\r\n******** Experiment_%d Channel 2 results ******\r\n', ExpNum);
% fprintf(fid,'Total energy in the time domain = %6.0f \r\n', e2t);
% fprintf(fid,'Normalized bands energies in the freq domain = %8.0f %8.0f %8.0f \r\n', F1pow, F2pow, F3pow);
% 

% fclose(fid);




end

