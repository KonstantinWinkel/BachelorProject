#include "rodos.h"
#include "../include/topics.h"

Topic<IMUDATA> IMU_Topic(-1, "IMU_Topic");
Topic<LIDARDATA> LIDAR_Filtered_Topic(-1, "LIDAR_Filtered_Topic");
Topic<LIDARDATA> LIDAR_Raw_Topic(-1, "LIDAR_Raw_Topic");