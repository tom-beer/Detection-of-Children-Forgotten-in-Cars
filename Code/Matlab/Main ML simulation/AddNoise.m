function [TrainArr] = AddNoise(simParams, TrainArr, species)
switch simParams.general.addSynthNoise
    case 'no kid'
        kidSegments   = find(strcmp(species, 'Kid In'));
        noKidSegments = find(strcmp(species, 'No Kid'));
        for iKidSeg = 1:numel(kidSegments)
            noiseSignal = simParams.general.noiseSTD * (TrainArr(:,noKidSegments(randi(numel(noKidSegments)))) + TrainArr(:,noKidSegments(randi(numel(noKidSegments)))));
            TrainArr(:,kidSegments(iKidSeg)) = TrainArr(:,kidSegments(iKidSeg)) + noiseSignal;
        end
    case 'normal'
        for iSeg = 1:size(TrainArr,2)
            noiseSignal = simParams.general.noiseSTD * randn(size(TrainArr(:,iSeg)));
            TrainArr(:,iSeg) = TrainArr(:,iSeg) + noiseSignal;
        end
    otherwise
        
end
end