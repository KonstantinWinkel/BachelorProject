RAD2DEG = 57.2958

%-
%-  READ DATA FROM CSV FILE
%-

%index mapping: 1 = time, 2/3 = x/y angle, 4/5 = x/y velocity, 6/7 = x/y controller output, 8/9 = x/y IMU reading, 10/11 = LIDAR Filtered, 12/13 = LIDAR Raw
M_1 = "csv_files/camera_x_output.csv";
M_2 = "csv_files/camera_y_output.csv";
M_3 = "csv_files/controller_x_output.csv"; %clean?
M_4 = "csv_files/controller_y_output.csv"; %clean?
M_5 = "csv_files/uart_output.csv";

X_1 = csvread(M_1);
X_2 = csvread(M_2);
X_3 = csvread(M_3);
X_4 = csvread(M_4);
X_5 = csvread(M_5);

%-
%-  PLOT ALL DATA
%-

figure(1);
plot(X_1(:,1), RAD2DEG * X_1(:,2));
hold on;
plot(X_2(:,1), RAD2DEG * X_2(:,2));
hold off;
title('Angle');
xlabel('Time [s]');
ylabel('Angle [Degree]');
legend('X', 'Y');

figure(2);
plot(X_1(:,1), RAD2DEG * X_1(:,3));
hold on;
plot(X_2(:,1), RAD2DEG * X_2(:,3));
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

figure(5)
plot(X_5(:,1), 0.57 * X_5(:,6) + 50);
%plot(X_5(:,1), 0.5 * X_5(:,6) + 50);
hold on;
%plot(X_5(:,1), 0.5 * X_5(:,7));
plot(X_5(:,1), 0.5 * X_5(:,7) - 50);
hold off;
title('Position of pen from LIDAR');
xlabel('Time [s]');
ylabel('Position [cm]');
legend('X', 'Y');


%-
%-  TIME ANALYSIS
%-

n = 1 %disregard first n values -> makes graph nicer

xCameraDiff = []
yCameraDiff = []
xControllerDiff = []
yControllerDiff = []
UARTDiff = []

for i = n:(size(X_1(:, 1)) - 1)
  xCameraDiff(1, end + 1) = X_1(i+1, 1) - X_1(i, 1);
  xCameraDiff(2, end) = i - n;
endfor

for i = n:(size(X_2(:, 1)) - 1)
  yCameraDiff(1, end + 1) = X_2(i+1, 1) - X_2(i, 1);
  yCameraDiff(2, end) = i - n;
endfor

for i = 1:(size(X_3(:, 1)) - 1) %DONT DISREGARD VALUES HERE
  xControllerDiff(1, end + 1) = X_3(i+1, 1) - X_3(i, 1);
  xControllerDiff(2, end) = i;
endfor

for i = 1:(size(X_4(:, 1)) - 1) %DONT DISREGARD VALUES HERE
  yControllerDiff(1, end + 1) = X_4(i+1, 1) - X_4(i, 1);
  yControllerDiff(2, end) = i;
endfor

for i = n:(size(X_5(:, 1)) - 1)
  UARTDiff(1, end + 1) = X_5(i+1, 1) - X_5(i, 1);
  UARTDiff(2, end) = i - n;
endfor

figure(7);
plot(xCameraDiff(2, :), xCameraDiff(1, :));
hold on;
plot(yCameraDiff(2, :), yCameraDiff(1, :));
hold on;
%plot(xControllerDiff(2, :), xControllerDiff(1, :));
%hold on;
%plot(yControllerDiff(2, :), yControllerDiff(1, :));
%hold on;
plot(UARTDiff(2, :), UARTDiff(1, :));
hold off;
title('deltaTime for each component');
xlabel('Time [s]');
ylabel('Time [s]');
%legend('Image Processing X', 'Image Processing Y', 'Controller X', 'Controller Y', 'UART');
legend('Image Processing X', 'Image Processing Y', 'UART');

average = zeros(5, 1);
error = zeros(5, 1);

average(1, 1) = mean(xCameraDiff(1,:));
average(2, 1) = mean(yCameraDiff(1,:));
average(3, 1) = mean(xControllerDiff(1,:));
average(4, 1) = mean(yControllerDiff(1,:));
average(5, 1) = mean(UARTDiff(1,:));

%for now some are commented out because no data has yet been generated
error(1, 1) = std(xCameraDiff(1,:))/sqrt(size(xCameraDiff(1,:), 1));
error(2, 1) = std(yCameraDiff(1,:))/sqrt(size(yCameraDiff(1,:), 1));
error(3, 1) = std(xControllerDiff(1,:))/sqrt(size(xControllerDiff(1,:), 1));
error(4, 1) = std(yControllerDiff(1,:))/sqrt(size(yControllerDiff(1,:), 1));
error(5, 1) = std(UARTDiff(1,:))/sqrt(size(UARTDiff(1,:), 1));

disp('Time analysis');
disp(['ImageProcessing X: ', num2str(average(1, 1)), ' +- ', num2str(error(1, 1)), ' s']);
disp(['ImageProcessing Y: ', num2str(average(2, 1)), ' +- ', num2str(error(2, 1)), ' s']);
disp(['Controller X: ', num2str(average(3, 1)), ' +- ', num2str(error(3, 1)), ' s']);
disp(['Controller Y: ', num2str(average(4, 1)), ' +- ', num2str(error(4, 1)), ' s']);
disp(['UART: ', num2str(average(5, 1)), ' +- ', num2str(error(5, 1)), ' s']);