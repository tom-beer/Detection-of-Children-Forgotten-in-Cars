function DisplayPerformance(simName, parameterName, parameterGrid, md, fa, sr)
paramGridLen = numel(parameterGrid);
figure;
subplot(2,1,1)
if iscell(parameterGrid)
    plot(1:paramGridLen,md,'-oy', 1:paramGridLen,fa,'-oc', 1:paramGridLen,fa+md,'-or','LineWidth',3);
    set(gca, 'Xtick', 1:paramGridLen, 'Xticklabel',parameterGrid);
else
    plot(parameterGrid,md,'-oy', parameterGrid,fa,'-oc', parameterGrid,fa+md,'-or','LineWidth',3)
end
title('Error Rate [percentage]','FontSize',18)
xlabel(parameterName,'FontSize',16)
ylabel('percent','FontSize',16)
legend('Miss detects','False alarms', 'total errors')
grid on; grid minor;

subplot(2,1,2)
if iscell(parameterGrid)
    plot(1:paramGridLen,sr,'-ob','LineWidth',3);
    set(gca, 'Xtick', 1:paramGridLen, 'Xticklabel',parameterGrid);
else
    plot(parameterGrid,sr,'-ob','LineWidth',3)
end
title('Success Rate [percentage]','FontSize',18)
xlabel(parameterName,'FontSize',16)
ylabel('percent','FontSize',16)

filePath = ['..\..\..\Results\',simName,'.fig'];
set(gcf,'name',simName)
savefig(filePath);

disp(['Total Success ratio: ' num2str(max(sr))])
% disp(['False Alarm Rate: ' num2str(fa)])
% disp(['Miss Detect Rate: ' num2str(md)])
end