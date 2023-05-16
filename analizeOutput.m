%-
%-  READ DATA FROM CSV FILE
%-

%index mapping: 1 = time, 2/3 = x/y angle, 4/5 = x/y velocity, 6/7 = x/y controller output, 8/9 = x/y IMU reading
M_1= "output.csv"; 
X_1= csvread(M_1);

%-
%-  PREPROCESS DATA FOR PLOT AND CALCULATIONS
%-

%array definitions for datapoints
AX = [];
AY = [];
VX = [];
VY = [];
CX = [];
CY = [];
FX = [];
FY = [];

%array definitions for time of datapoints
TAX = [];
TAY = [];
TVX = [];
TVY = [];
TCX = [];
TCY = [];
TFX = [];
TFY = [];

%init values for datapoints, mostly 0
AX (1) = X_1(1,2);
AY (1) = X_1(1,3);
VX (1) = X_1(1,4);
VY (1) = X_1(1,5);
CX (1) = X_1(1,6);
CY (1) = X_1(1,7);
FX (1) = X_1(1,8);
FY (1) = X_1(1,9);

%init values for time of datapoints, mostly 0
TAX (1) = X_1(1,1);
TAY (1) = X_1(1,1);
TVX (1) = X_1(1,1);
TVY (1) = X_1(1,1);
TCX (1) = X_1(1,1);
TCY (1) = X_1(1,1);
TFX (1) = X_1(1,1);
TFY (1) = X_1(1,1);

for i = 1:size(X_1, 1)
  
  %fill AngleX values and times
  if AX(end) != X_1(i, 2)
    TAX(end + 1) = X_1(i, 1);
    AX(end + 1) = X_1(i, 2);
  endif
  
  %fill AngleY values and times
  if AY(end) != X_1(i, 3)
    TAY(end + 1) = X_1(i, 1);
    AY(end + 1) = X_1(i, 3);
  endif
  
  %fill VelocityX values and times
  if VX(end) != X_1(i, 4)
    TVX(end + 1) = X_1(i, 1);
    VX(end + 1) = X_1(i, 4);
  endif
  
  %fill VelocityY values and times
  if VY(end) != X_1(i, 5)
    TVY(end + 1) = X_1(i, 1);
    VY(end + 1) = X_1(i, 5);
  endif
  
  %fill ControllerX values and times
  if CX(end) != X_1(i, 6)
    TCX(end + 1) = X_1(i, 1);
    CX(end + 1) = X_1(i, 6);
  endif
  
  %fill ControllerY values and times
  if CY(end) != X_1(i, 7)
    TCY(end + 1) = X_1(i, 1);
    CY(end + 1) = X_1(i, 7);
  endif
  
  %fill ForceX values and times
  if FX(end) != X_1(i, 8)
    TFX(end + 1) = X_1(i, 1);
    FX(end + 1) = X_1(i, 8);
  endif
  
  %fill ForceY values and times
  if FY(end) != X_1(i, 9)
    TFY(end + 1) = X_1(i, 1);
    FY(end + 1) = X_1(i, 9);
  endif
  
endfor

%-
%-  PLOT ALL DATA
%-

figure(1);
plot(TAX, AX);
hold on;
plot(TAY, AY);
hold off;
title('Angle');
xlabel('Time [s]');
ylabel('Angle [Degree]');
legend('X', 'Y');

figure(2);
plot(TVX, VX);
hold on;
plot(TVY, VY);
hold off;
title('Angular velocity');
xlabel('Time [s]');
ylabel('Angular velocity [Degree/s]');
legend('X', 'Y');

figure(3);
plot(TCX, CX);
hold on;
plot(TCY, CY);
hold off;
title('Actuator position as calculated by the controller');
xlabel('Time [s]');
ylabel('Actuator position [Degree]');
legend('X', 'Y');

figure(4);
plot(TFX, FX);
hold on;
plot(TFY, FY);
hold off;
title('Force on the pen as messured by the IMU');
xlabel('Time [s]');
ylabel('Force [Newton]');
legend('X', 'Y');

%-
%-  TIME ANALYSIS
%-

xDiff = []
yDiff = []
controllerDiff = []
ImuDiff = []

for i = 1:(size(TAX(:)) - 1)
  xDiff(end + 1) = TAX(i+1) - TAX(i);
endfor

for i = 1:(size(TAY(:)) - 1)
  yDiff(end + 1) = TAY(i+1) - TAY(i);
endfor

for i = 1:(size(TCX(:)) - 1)
  controllerDiff(end + 1) = TCX(i+1) - TCX(i);
endfor

for i = 1:(size(TFX(:)) - 1)
  ImuDiff(end + 1) = TFX(i+1) - TFX(i);
endfor

figure(5);
plot(linspace(0, size(xDiff, 1), size(xDiff,1)), xDiff(:, 1));
hold on;
plot(linspace(0, size(yDiff, 1), size(yDiff,1)), yDiff(:, 1));
hold on;
plot(linspace(0, size(controllerDiff, 1), size(controllerDiff,1)), controllerDiff(:, 1));
hold on;
plot(TFX(1:end-1), ImuDiff);
hold off;
title('deltaTime for each component');
xlabel('Time [s]');
ylabel('Time [s]');
legend('Image Processing X', 'Image Processing Y', 'Controller', 'IMU Readings');

average = zeros(4, 1);
error = zeros(4, 1);

average(1, 1) = mean(xDiff);
average(2, 1) = mean(yDiff);
average(3, 1) = mean(controllerDiff);
average(4, 1) = mean(ImuDiff);

%for now some are commented out because no data has yet been generated
error(1, 1) = std(xDiff)/sqrt(size(xDiff, 1));
error(2, 1) = std(yDiff)/sqrt(size(yDiff, 1));
error(3, 1) = std(controllerDiff)/sqrt(size(controllerDiff, 1));
error(4, 1) = std(ImuDiff)/sqrt(size(ImuDiff, 1));

disp('Time analysis');
disp(['ImageProcessing X: ', num2str(average(1, 1)), ' +- ', num2str(error(1, 1)), ' s']);
disp(['ImageProcessing Y: ', num2str(average(2, 1)), ' +- ', num2str(error(2, 1)), ' s']);
disp(['Controller: ', num2str(average(3, 1)), ' +- ', num2str(error(3, 1)), ' s']);
disp(['IMU Readings: ', num2str(average(4, 1)), ' +- ', num2str(error(4, 1)), ' s']);