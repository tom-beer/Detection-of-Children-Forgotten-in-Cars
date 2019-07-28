function SimulateKidML(varargin)
close all; clc;
%% Simulation Parameters
if nargin > 0
    simParams = varargin{1};
else % default simulation parameters
    clear
    % dataset for child detection
    simParams.general.dataset = [{'..\..\..\Data\New Data\Kid\Kindergarden'}       {1} {1};... %  first column for folders
                                 {'..\..\..\Data\New Data\Kid\engineOnParkingKid'} {1} {1};... % second column for ground trouth
                                 {'..\..\..\Data\New Data\Kid\engineOffKid'}       {1} {1};... %  third column is a flag if ok to test on folder (optional, default is 1)
                                 {'..\..\..\Data\New Data\NoKid\Still car'}        {0} {1}];%;...
    % dataset for passenger detection      
%     simParams.general.dataset = [{'..\..\..\Data\New Data\NoKid\Driving bag'}  {0} ;...
%                                  {'..\..\..\Data\New Data\Kid\drivingGrownUp'} {1}];
    
    simParams.general.windowLen      = 4096;  % Input buffer size, defines amount of window overlap
    simParams.general.segmentlen     = simParams.general.windowLen/2; % Processing window
    simParams.general.drawPlots      = false;
    simParams.general.addSynthNoise  = 'false'; % 'normal', 'no kid' or 'none' (no kid means to add a no kid signal to kid signals)
    simParams.general.noiseSTD       = 0;  % std of noise, 0 for std of signal
    simParams.general.useStats       = 1;  % true: add stats vector to featureVec in FeatureExtraction
    simParams.preProc.numMedians     = 1000 ; % threshold for filtering out segments with high median value
    simParams.preProc.try2load       = true; % important! if dataset was changes, need to toggle this off for one time
    simParams.features.method        = 'wavelet';
    simParams.features.waveletName   = 'fk6'; % see doc wfilters for possible filters
    simParams.features.numLevels     = 8;    % wavelet decomposition levels. Possible values: 1 to fix(log2(simParams.general.windowLen))
    simParams.features.numFeatures   = 50;   % Number of input feature coefficients to be used
    simParams.features.try2load      = false;
    simParams.dataClassify.method    = 'knn'; % 'lda', 'svm', 'logistic', 'knn', 'tree', 'naiveBayes'
    simParams.dataClassify.hystLevel = 14;
    simParams.dataClassify.numTrainRuns = 10; % number of times to run the classification block
    simParams.dataClassify.testSetRatio = 0.2; % percentage of test set from whole dataset
    % parameters for lda
    simParams.dataClassify.ldaType        = 'linear'; % 'linear' or 'quadratic' (quadratic doesn't usually work) 
    % parameters for svm and logistic regression
    simParams.dataClassify.lambda         = 'auto'; % 'auto' or numeric value (possibly a vector)
    simParams.dataClassify.regularization = 'ridge'; % 'ridge' (L2) or 'lasso' (L1)
    % parameters for knn
    simParams.dataClassify.distance       = 'cityblock'; % 'cityblock', 'chebychev', 'correlation', 'cosine', 'euclidean', 'jaccard' and more
    simParams.dataClassify.numNeighbors   = 10;
    simParams.dataClassify.distanceWeight = 'squaredinverse'; % 'equal', 'inverse' or 'squaredinverse'
    simParams.dataClassify.exponent       = 2; % p of Lp norm, relevant only for 'minkowski' distance
    % parameters for tree
    simParams.dataClassify.maxNumSplits   = 1500; % default is number of training signals
    simParams.dataClassify.minLeafSize    = 1;
    simParams.dataClassify.minParentSize  = 10;
    
end

%% Data Preprocessing
[TrainArr, ExpArr, ok4TestArr, TestExp, species, auxParams] = DataPreprocessing(simParams);
[TrainArr] = AddNoise(simParams, TrainArr, species);

%% Feature Extraction
features = FeatureExtraction(TrainArr, simParams, auxParams);

%% Data Classification
%parameter and grid to inspect:
parameterName = 'temp';
simName       = 'temp';

% Here are some previously used grids, for convinience
% parameterGrid = {'equal','inverse','squaredinverse'};
% parameterGrid = { 'cityblock', 'chebychev', 'correlation', 'cosine', 'euclidean', 'jaccard' };
% parameterGrid = {'tree', 'lda', 'svm', 'logistic', 'knn', 'naiveBayes'};
parameterGrid = 1;

paramGridLen = numel(parameterGrid);
md = NaN(1,paramGridLen);
fa = NaN(1,paramGridLen);
sr = NaN(1,paramGridLen);

for iParameter = 1:paramGridLen
    disp(['Parameter ' num2str(iParameter) ' out of ' num2str(paramGridLen)]);
    % for parametric run: update parameter:
    currParameter = parameterGrid(iParameter);
    if iscell(currParameter), currParameter = currParameter{1}; end
    simParams.dataClassify.minLeafSize = currParameter;
    % for runs that require re-caculation of features, uncomment the next line:
%     features = FeatureExtraction(TrainArr, simParams, auxParams);
    [currFA,currMD,currSR] = DataClassification(features, species, ExpArr, TestExp, ok4TestArr, simParams);
    md(iParameter) = currMD;
    fa(iParameter) = currFA;
    sr(iParameter) = currSR;
end

%% Display & Analyze results
DisplayPerformance(simName, parameterName, parameterGrid, md, fa, sr)

end