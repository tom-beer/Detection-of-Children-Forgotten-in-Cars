function [] = CompatibilityCheck()
windowLen = 4096;
numCoefficients = 250;
segmentSignal = zeros(1,windowLen);
% ldaCoeffs = [lda.Coeffs(1, 2).Linear ; lda.Coeffs(1, 2).Const];
% dlmwrite('ldaCoeffs.txt',ldaCoeffs,'precision','%.10f');
% clear ldaCoeffs
ldaCoeffs = textread('ldaCoeffs.txt');
% [y1, ~] = Prepare_Data_freq_2(expNum);
% y1 = textread(['..\Samples_int\Experiment_' num2str(expNum) '.txt'])';
fullSignal = textread(['..\Samples_int\Experiment_' num2str(78) '.txt']);
for segment = 1 : windowLen/2 : length(fullSignal)-windowLen
    segmentSignal(1:windowLen/2) = segmentSignal(windowLen/2+1:end);
    segmentSignal(windowLen/2+1:end) = fullSignal(segment:segment+windowLen/2-1);
    meanVal = mean(segmentSignal);
    maxVal = max(abs(segmentSignal));
    normSig = (segmentSignal - meanVal) / maxVal; % DC removal & normalization
    wav = daub4_transform(windowLen,normSig);
    ldaRes = ldaCoeffs(end) + wav(1:numCoefficients)'*ldaCoeffs(1:end-1);
    display(ldaRes);
end


% %%
% y1 = textread(['..\Samples_int\Experiment_' num2str(78) '.txt']);
% figure; plot(y1)

end