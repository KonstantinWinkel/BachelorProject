%-
%-  READ DATA FROM CSV FILE
%-

%index mapping: 1 = time, 2/3 = x/y angle, 4/5 = x/y velocity, 6/7 = x/y controller output, 8/9 = x/y IMU reading, 10/11 = LIDAR Filtered, 12/13 = LIDAR Raw
M_1 = "camera_x_output_clean.csv"; 
M_2 = "camera_y_output_clean.csv";
M_3 = "controller_x_output.csv"; %clean?
M_4 = "controller_y_output.csv"; %clean?
M_5 = "uart_output_clean.csv";

X_1 = csvread(M_1);
X_2 = csvread(M_2);
X_3 = csvread(M_3);
X_4 = csvread(M_4);  
X_5 = csvread(M_5);

%-
%-  PLOT ALL DATA
%-

figure(1);
plot(X_1(:,1), X_1(:,2));
hold on;
plot(X_2(:,1), X_2(:,2));
hold off;
title('Angle');
xlabel('Time [s]');
ylabel('Angle [Degree]');
legend('X', 'Y');

figure(2);
plot(X_1(:,1), X_1(:,3));
hold on;
plot(X_2(:,1), X_2(:,3));
hold off;
title('Angular velocity');
xlabel('Time [s]');
ylabel('Angular velocity [Degree/s]');
legend('X', 'Y');

figure(3);
plot(X_3(:,1), X_3(:,2));
hold on;
plot(X_4(:,1), X_4(:,2));
hold off;
title('Actuator position as calculated by the controller');
xlabel('Time [s]');
ylabel('Actuator position [Degree]');
legend('X', 'Y');

figure(4);
plot(X_5(:,1), X_5(:,2));
hold on;
plot(X_5(:,1), X_5(:,3));
hold off;
title('Force on the pen as messured by the IMU');
xlabel('Time [s]');
ylabel('Force [Newton]');
legend('X', 'Y');

figure(5);
plot(X_5(:,1), X_5(:,4));
hold on;
plot(X_5(:,1), X_5(:,5));
hold off;
title('Filtered Lidar values');
xlabel('Time [s]');
ylabel('Distance [mm]');
legend('X', 'Y');

figure(6);
plot(X_5(:,1), X_5(:,6));
hold on;
plot(X_5(:,1), X_5(:,7));
hold off;
title('Raw Lidar values');
xlabel('Time [s]');
ylabel('Distance [mm]');
legend('X', 'Y');

%-
%-  TIME ANALYSIS
%-

xCameraDiff = []
yCameraDiff = []
xControllerDiff = []
yControllerDiff = []
UARTDiff = []

for i = 1:(size(X_1(:, 1)) - 1)
  xCameraDiff(end + 1) = X_1(i+1, 1) - X_1(i, 1);
endfor

for i = 1:(size(X_2(:, 1)) - 1)
  yCameraDiff(end + 1) = X_2(i+1, 1) - X_2(i, 1);
endfor

for i = 1:(size(X_3(:, 1)) - 1)
  xControllerDiff(end + 1) = X_3(i+1, 1) - X_3(i, 1);
endfor

for i = 1:(size(X_4(:, 1)) - 1)
  yControllerDiff(end + 1) = X_4(i+1, 1) - X_4(i, 1);
endfor

for i = 1:(size(X_5(:, 1)) - 1)
  UARTDiff(end + 1) = X_5(i+1, 1) - X(i, 1);
endfor

figure(5);
plot(linspace(0, size(xCameraDiff, 1), size(xCameraDiff,1)), xCameraDiff(:, 1));
hold on;
plot(linspace(0, size(yCameraDiff, 1), size(yCameraDiff,1)), yCameraDiff(:, 1));
hold on;
plot(linspace(0, size(xControllerDiff, 1), size(xControllerDiff,1)), xControllerDiff(:, 1));
hold on;
plot(linspace(0, size(yControllerDiff, 1), size(yControllerDiff,1)), yControllerDiff(:, 1));
hold on;
plot(linspace(0, size(UARTDiff, 1), size(UARTDiff,1)), UARTDiff(:, 1));
hold off;
title('deltaTime for each component');
xlabel('Time [s]');
ylabel('Time [s]');
legend('Image Processing X', 'Image Processing Y', 'Controller X', 'Controller Y', 'UART');

average = zeros(5, 1);
error = zeros(5, 1);

average(1, 1) = mean(xCameraDiff);
average(2, 1) = mean(yCameraDiff);
average(3, 1) = mean(xControllerDiff);
average(4, 1) = mean(yControllerDiff);
average(5, 1) = mean(UARTDiff);

%for now some are commented out because no data has yet been generated
error(1, 1) = std(xCameraDiff)/sqrt(size(xCameraDiff, 1));
error(2, 1) = std(yCameraDiff)/sqrt(size(yCameraDiff, 1));
error(3, 1) = std(xControllerDiff)/sqrt(size(xControllerDiff, 1));
error(4, 1) = std(yControllerDiff)/sqrt(size(yControllerDiff, 1));
error(5, 1) = std(UARTDiff)/sqrt(size(UARTDiff, 1));

disp('Time analysis');
disp(['ImageProcessing X: ', num2str(average(1, 1)), ' +- ', num2str(error(1, 1)), ' s']);
disp(['ImageProcessing Y: ', num2str(average(2, 1)), ' +- ', num2str(error(2, 1)), ' s']);
disp(['Controller X: ', num2str(average(3, 1)), ' +- ', num2str(error(3, 1)), ' s']);
disp(['Controller Y: ', num2str(average(4, 1)), ' +- ', num2str(error(4, 1)), ' s']);
disp(['UART: ', num2str(average(5, 1)), ' +- ', num2str(error(5, 1)), ' s']);