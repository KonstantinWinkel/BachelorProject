%index mapping: 1 = time, 2/3 = x/y angle, 4/5 = x/y velocity, 6/7 = x/y controller output, 8/9 = x/y IMU reading
M_1= "output.csv"; 
X_1= csvread(M_1);

figure(1);
plot(X_1(:,1),X_1(:,2));
hold on;
plot(X_1(:,1), X_1(:,3));
hold off;
title('Angle');
xlabel('Time [s]');
ylabel('Angle [Degree]');
legend('X', 'Y');

figure(2);
plot(X_1(:,1),X_1(:,4));
hold on;
plot(X_1(:,1), X_1(:,5));
hold off;
title('Angular velocity');
xlabel('Time [s]');
ylabel('Angular velocity [Degree/s]');
legend('X', 'Y');

figure(3);
plot(X_1(:,1),X_1(:,6));
hold on;
plot(X_1(:,1), X_1(:,7));
hold off;
title('Actuator position as calculated by the controller');
xlabel('Time [s]');
ylabel('Actuator position [Degree]');
legend('X', 'Y');

figure(4);
plot(X_1(:,1),X_1(:,8));
hold on;
plot(X_1(:,1), X_1(:,9));
hold off;
title('Force on the pen as messured by the IMU');
xlabel('Time [s]');
ylabel('Force [Newton]');
legend('X', 'Y');

%time analysis
values = zeros(4, 1); %x y controller IMU
xTime = [];
yTime = [];
controllerTime = [];
ImuTime = [];

for i = 1:size(X_1, 1)
  
  if values(1,1) != X_1(i, 2)
    values(1,1) = X_1(i, 2);
    xTime(end+1, 1) = X_1(i, 1);
  endif
  
  if values(2, 1) != X_1(i, 3)
    values(2, 1) = X_1(i, 3);
    yTime(end+1, 1) = X_1(i, 1);
  endif
  
  if values(3, 1) != X_1(i, 2)
    values(3, 1) = X_1(i, 2);
    controllerTime(end+1, 1) = X_1(i, 1);
  endif
  
  if values(4, 1) != X_1(i, 2)
    values(4, 1) = X_1(i, 2);
    ImuTime(end+1, 1) = X_1(i, 1);
  endif
  
endfor

xDiff = zeros(size(xTime, 1) - 1, 1);
yDiff = zeros(size(yTime, 1) - 1, 1);
controllerDiff = zeros(size(controllerTime, 1) - 1, 1);
ImuDiff = zeros(size(ImuTime, 1) - 1, 1);

for i = 1:size(xDiff, 1)
  xDiff(i, 1) = xTime(i+1, 1) - xTime(i, 1);
endfor

for i = 1:size(yDiff, 1)
  yDiff(i, 1) = yTime(i+1, 1) - yTime(i, 1);
endfor

for i = 1:size(controllerDiff, 1)
  controllerDiff(i, 1) = controllerTime(i+1, 1) - controllerTime(i, 1);
endfor

for i = 1:size(ImuDiff, 1)
  ImuDiff(i, 1) = ImuTime(i+1, 1) - ImuTime(i, 1);
endfor

figure(5);
plot(linspace(0, size(xDiff, 1), size(xDiff,1)), xDiff(:, 1));
hold on;
plot(linspace(0, size(yDiff, 1), size(yDiff,1)), yDiff(:, 1));
hold on;
plot(linspace(0, size(controllerDiff, 1), size(controllerDiff,1)), controllerDiff(:, 1));
hold on;
plot(linspace(0, size(ImuDiff, 1), size(ImuDiff,1)), ImuDiff(:, 1));
hold off;
title('deltaTime for each component');
xlabel('Time [s]');
ylabel('Time [s]');
legend('ImageProcessingX', 'ImageProcessingY', 'Controller', 'IMU Readings');

average = zeros(4, 1);
error = zeros(4, 1);

average(1, 1) = mean(xDiff);
average(2, 1) = mean(yDiff);
average(3, 1) = mean(controllerDiff);
average(4, 1) = mean(ImuDiff);

%for now some are commented out because no data has yet been generated
error(1, 1) = std(xDiff)/sqrt(size(xDiff, 1));
%error(2, 1) = std(yDiff)/sqrt(size(yDiff, 1));
%error(3, 1) = std(controllerDiff)/sqrt(size(controllerDiff, 1));
%error(4, 1) = std(ImuDiff)/sqrt(size(ImuDiff, 1));

disp('Time analysis');
disp(['ImageProcessing X: ', num2str(average(1, 1)), ' +- ', num2str(error(1, 1)), ' s']);
%disp(['ImageProcessing Y: ', num2str(average(2, 1)), ' +- ', num2str(error(2, 1)), ' s']);
%disp(['Controller: ', num2str(average(3, 1)), ' +- ', num2str(error(3, 1)), ' s']);
%disp(['IMU Readings: ', num2str(average(4, 1)), ' +- ', num2str(error(4, 1)), ' s']);