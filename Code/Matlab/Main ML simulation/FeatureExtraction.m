function [features] = FeatureExtraction(data, simParams, auxParams)
% This function extracts relevant features from data to be used in later classification algorithm
% Input:
%         - data:      matrix of preprocessed data. Each colums is a data sample
%                      of length simParams.general.windowLen
%         - method:    type of feature to be extracted
%         - simParams: struct of simulation parameters
% Output:
%         - features:  matrix of extracted features. Each column containts
%                      the features of a data sample

%%
numSignals = auxParams.numSignals;

switch simParams.features.method
    case 'wavelet'
        %         if simParams.features.try2load && exist('..\Results\waveletFeatures.mat','file')
        %             load( '..\Results\waveletFeatures.mat')
        %         else
        for jSignal = 1:numSignals
            signal = data(:,jSignal);
            % this function implements a wavelet transform only with the db4 wavelet:
            % features(:,j) = daub4_transform(windowLen,signal);
            % this function can handle many different wavelets:
            [yc,l] = wavedec(signal,simParams.features.numLevels, simParams.features.waveletName);
            featureVec = yc(1:simParams.features.numFeatures);
            for iLevel = 1: simParams.features.numLevels
                cdi = detcoef(yc,l,iLevel);
                zcr  = numel(find(signal(:).*circshift(signal(:), [-1 0]) <= 0)); % zero crossing rate in time domain
                zcrW = numel(find(cdi(:).*circshift(cdi(:), [-1 0]) <= 0));% zero crossing rate in wavelet domain
                statVec = [max(cdi) min(cdi) mean(cdi) std(cdi) zcr zcrW]; % max(cdi.^2) seems to interfere..
                if (simParams.general.useStats == true)
                    featureVec = [featureVec ; statVec'];
                end
            end
            features(1:numel(featureVec),jSignal) = featureVec;
            onlyWaveletCoeffs(1:length(yc),jSignal) = yc;
        end
        
        if simParams.general.drawPlots
            figure
            hold on;
            index=find(RespArr==1);
            for jSignal=1:length(index)
                plot(onlyWaveletCoeffs(:,index(jSignal)),'xr');
            end
            
            index=find(RespArr==0);
            for jSignal=1:length(index)
                plot(onlyWaveletCoeffs(:,index(jSignal)),'og');
            end
            axis([0,numCoeffs,-5,5])
            title('Signals Wavelet Coefficients - No Kid ');
            %             end
        end
    otherwise
        error('Invalid method!');
end

% in order to save outputs of this function, run:
% save('waveletFeatures','features');
end