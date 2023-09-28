#pragma once

struct LIDARDATA
{
    int16_t xDistance;
    int16_t yDistance;
};

struct IMUDATA
{
    float xForce;
    float yForce;
    float zForce;

    float xAngularVelocity;
    float yAngularVelocity;
    float zAngularVelocity;

    float xMagneticFlux;
    float yMagneticFlux;
    float zMagneticFlux;
};

struct ANGLES
{
    float xAngle;
    float yAngle;
};

struct CONTROLLERDATA{
    float xValue;
    float yValue;
};


extern Topic<LIDARDATA> LIDAR_Filtered_Topic;
extern Topic<LIDARDATA> LIDAR_Raw_Topic;
extern Topic<IMUDATA> IMU_Topic;
extern Topic<IMUDATA> Full_IMU_Topic;
extern Topic<ANGLES> Angles_Topic;
extern Topic<CONTROLLERDATA> Controller_Topic;