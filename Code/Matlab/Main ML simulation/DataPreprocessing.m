function [TrainArr, ExpArr, ok4TestArr, TestExp, species, auxParams] = DataPreprocessing(simParams)
% TODO:
% waitbar
% seperate train and testset
dataset    = simParams.general.dataset;
numFolders = size(dataset,1);
firstExp   = 0; auxParams.numSignals = 0;
TrainArr = []; ExpArr = []; TestExp = []; species = []; ok4TestArr = [];
for iFolder = 1:numFolders
    folderName   = dataset{iFolder,1};
    groundTrouth = dataset{iFolder,2};
    ok4Test      = 1;
    if (size(dataset,2) > 2)
        ok4Test = dataset{iFolder,3};
    end
    [currTrainArr, currExpArr, currOk4TestArr, currTestExp, currSpecies, currNumSignals, lastExp] = DataPreprocessingInternal(folderName, groundTrouth, ok4Test, simParams, firstExp);
    TrainArr = [TrainArr currTrainArr]; ExpArr = [ExpArr currExpArr]; TestExp = [TestExp currTestExp]; species = [species currSpecies];
    auxParams.numSignals = auxParams.numSignals + currNumSignals; ok4TestArr = [ok4TestArr currOk4TestArr];
    firstExp  = lastExp;
end

TrainArr = AddNoise(simParams, TrainArr, species);

%%
    function [TrainArr, ExpArr, ok4TestArr, TestExp, species, numSignals, lastExp] = DataPreprocessingInternal(folderName, groundTrouth, ok4Test, simParams, firstExp)
        if simParams.preProc.try2load && exist(fullfile(folderName, 'DataPreprocessingOut.mat'),'file')
            load(fullfile(folderName, 'DataPreprocessingOut.mat'))
            return;
        end
        
        recDir = dir([folderName filesep '*.xlsx']);
        numExps = numel(recDir);
        
        ptr  = 1; % counter initialization
        tcnt = 1; % counter initialization
        windowLen     = simParams.general.windowLen;
        segmentSignal = zeros(1,windowLen);
        segmentlen    = simParams.general.segmentlen;  % Input buffer size, defines amount of window overlap
        
        for iExp = 1:numExps % loop on all data recordings
            if (groundTrouth == 1 || groundTrouth == 0) % if exists a ground truth
                expName       = recDir(iExp).name;
                expNum        = firstExp + iExp;
                filename      = fullfile(folderName,expName);
                signal        = xlsread(filename);
                fullSignal    = double(signal(:,1));
                expLen        = length(fullSignal);
                TestExp(tcnt) = expNum;
                
                for segment = 1:segmentlen:expLen-windowLen % loop on all segments
                    segmentSignal(1:windowLen/2)     = segmentSignal(windowLen/2+1:end);
                    segmentSignal(windowLen/2+1:end) = fullSignal(segment:segment+windowLen/2-1);
                    meanVal = mean(segmentSignal);
                    
                    maxVal1 = max(abs(segmentSignal));
                    segmentSignal = (segmentSignal - meanVal) / maxVal1; % DC removal & normalization
                    m1     = median(abs(segmentSignal));
                    index  = find(abs(segmentSignal) > simParams.preProc.numMedians*(m1));
                    l      = length(index);
                    r(ptr) = 100*length(index) / segmentlen;
                    
                    
                    TrainArr(1:windowLen,ptr) = segmentSignal;
                    RespArr(ptr) = groundTrouth;
                    ExpArr(ptr) = expNum;
                    
                    if r(ptr) < 10
                        ptr=ptr + 1;
                    end
                end
                tcnt = tcnt+1;
                disp(num2str(expNum))
            end
        end
        numSignals = ptr;
        if r(end) < 10
            numSignals = ptr-1;
        end
        
        species = cell(1,numSignals);
        for j=1:numSignals
            if RespArr(j) == 0
                species{j} = 'No Kid';
            else
                species{j} = 'Kid In';
            end
        end
        lastExp = firstExp + numExps;
        ok4TestArr = ok4Test * ones(size(TestExp));
        % in order to save outputs of this function, run:
        oldDir = cd;
        cd(folderName)
        save('DataPreprocessingOut', 'TrainArr', 'ExpArr', 'ok4TestArr', 'TestExp', 'species', 'numSignals','lastExp');
        cd(oldDir);
    end

end