function [fa,md,sr] = DataClassification(features, species, ExpArr, TestExp, ok4TestArr, simParams)
tempFA = 0;
tempMD = 0;
tempSR = 0;
numRuns = simParams.dataClassify.numTrainRuns;
for iRun = 1:numRuns
    [currFA,currMD] = DataClassificationInternal(features, species, ExpArr, TestExp, ok4TestArr, simParams);
    currSR = 100-currFA-currMD;
    tempMD = tempMD+currMD;
    tempFA = tempFA+currFA;
    tempSR = tempSR+currSR;
end
md = tempMD/numRuns;
fa = tempFA/numRuns;
sr = tempSR/numRuns;
end
%%
function [fa,md]=DataClassificationInternal(features, species, ExpArr, TestExp, ok4TestArr, simParams)
        %% Setup
        hystLevel        = simParams.dataClassify.hystLevel;        
        %% Train/Test separation
        dataset4Test   = TestExp(ok4TestArr == 1);
        ok4TestSize    = numel(dataset4Test);
        testSetSize    = floor(ok4TestSize * simParams.dataClassify.testSetRatio); % for ratio classification
        %testSetSize    = 1; % for leave-one-out cross validation
        testSetIndices = dataset4Test(randperm(ok4TestSize, testSetSize));
        
        testExp  = TestExp(testSetIndices);
        trainExp = TestExp(setdiff(TestExp,testSetIndices));
        
        trainSegments = features(:,ismember(ExpArr,trainExp));
        testSegments  = features(:,ismember(ExpArr,testExp));
        trainLabels   = species(ismember(ExpArr,trainExp));
        testLabels    = species(ismember(ExpArr,testExp));
        ExpArrTest    = ExpArr(ismember(ExpArr,testExp));
        
        %% Train the model
        X = trainSegments';
        Y = trainLabels';
        Mdl = 0;
        switch simParams.dataClassify.method
            case 'lda'
                Mdl = fitcdiscr(X, Y, 'DiscrimType', simParams.dataClassify.ldaType);
            case {'svm','logistic'}
                Mdl = fitclinear(X,Y, ...
                    'Learner', simParams.dataClassify.method,...
                    'Lambda', simParams.dataClassify.lambda,...
                    'Regularization', simParams.dataClassify.regularization);
                
            case 'knn'
                if (strcmp(simParams.dataClassify.distance,'minkowski') && isfield(simParams.dataClassify,'exponent'))
                Mdl = fitcknn(X,Y, ...
                    'Distance', simParams.dataClassify.distance,...
                    'NumNeighbors', simParams.dataClassify.numNeighbors,...
                    'DistanceWeight', simParams.dataClassify.distanceWeight,...
                    'Exponent', simParams.dataClassify.exponent);
                else
                     Mdl = fitcknn(X,Y, ...
                    'Distance', simParams.dataClassify.distance,...
                    'NumNeighbors', simParams.dataClassify.numNeighbors,...
                    'DistanceWeight', simParams.dataClassify.distanceWeight);
                end
            case 'tree'
                Mdl = fitctree(X,Y,...
                    'MaxNumSplits', simParams.dataClassify.maxNumSplits,...
                    'MinLeafSize', simParams.dataClassify.minLeafSize,...
                    'MinParentSize', simParams.dataClassify.minParentSize);
            case 'naiveBayes'
                Mdl = fitcnb(X,Y);
            otherwise
        end
        
        %% Predict on test set and run hysteresis
        labels = predict(Mdl,testSegments');
        labelsHist = labels;
        for iTestExp = testExp
            currLabels = labels(ismember(ExpArrTest,iTestExp));
            [~, ~, labelsOut] = Detect_with_hysteresis(currLabels,hystLevel);
            labelsHist(ismember(ExpArrTest,iTestExp)) = labelsOut;
        end
        
        %% Evaluate Performance
        falseAlarms = 0; missDetects = 0;
        for iTestExp = testExp
            currLabels = labels(ismember(ExpArrTest,iTestExp));
            currTestLabels = testLabels(ismember(ExpArrTest,iTestExp));
            singleExpFA = numel(find(strcmp(currLabels(hystLevel:end)','Kid In') & strcmp(currTestLabels(hystLevel:end),'No Kid')));
            singleExpMD = numel(find(strcmp(currLabels(hystLevel:end)','No Kid') & strcmp(currTestLabels(hystLevel:end),'Kid In')));
            falseAlarms = falseAlarms + singleExpFA;
            missDetects = missDetects + singleExpMD;
        end
        fa = 100*falseAlarms/numel(testLabels);
        md = 100*missDetects/numel(testLabels);

        % index = find(strcmp(testLabels,'No Kid')); % i think it's important only
        % for displaying the stem plots
        % Success_Ratio(index) = 100 - Success_Ratio(index); % invert non kid results
        
        %% Visualization of hysteresis effect
        % flags     = zeros(size(labels));
        % flagsHist = zeros(size(labels));
        % flags(strcmp(labels,'Kid In')) = 1;
        % flagsHist(strcmp(labelsHist,'Kid In')) = 1;
        % figure; plot(flags,'r'); hold on; plot(flagsHist,'g'); ylim([-0.1 1.1]);
        
end