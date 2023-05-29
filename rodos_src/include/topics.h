#pragma once

struct LIDARDATA
{
    uint16_t xDistance;
    uint16_t yDistance;
};

struct IMUDATA
{
    float xForce;
    float yForce;
    float zForce;
};


extern Topic<LIDARDATA> LIDAR_Topic;
extern Topic<IMUDATA> IMU_Topic;