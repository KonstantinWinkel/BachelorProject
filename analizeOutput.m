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

figure(2);
plot(X_1(:,1),X_1(:,4));
hold on;
plot(X_1(:,1), X_1(:,5));
hold off;
title('Angular velocity');
xlabel('Time [s]');
ylabel('Angular velocity [Degree/s]');

figure(3);
plot(X_1(:,1),X_1(:,6));
hold on;
plot(X_1(:,1), X_1(:,7));
hold off;
title('Actuator position as calculated by the controller');
xlabel('Time [s]');
ylabel('Actuator position [Degree]');

figure(4);
plot(X_1(:,1),X_1(:,8));
hold on;
plot(X_1(:,1), X_1(:,9));
hold off;
title('Force on the pen as messured by the IMU');
xlabel('Time [s]');
ylabel('Force [Newton]');
