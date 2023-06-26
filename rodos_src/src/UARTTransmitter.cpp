#include "rodos.h"
#include "hal/hal_gpio.h"

#include "../include/util.h"
#include "../include/topics.h"

HAL_GPIO red(GPIO_062);

CommBuffer<IMUDATA> IMU_Buffer;
CommBuffer<LIDARDATA> LIDAR_Filtered_Buffer;
CommBuffer<LIDARDATA> LIDAR_Raw_Buffer;

Subscriber IMU_Subscriber(IMU_Topic, IMU_Buffer);
Subscriber LIDAR_Filtered_Subscriber(LIDAR_Filtered_Topic, LIDAR_Filtered_Buffer);
Subscriber LIDAR_Raw_Subscriber(LIDAR_Raw_Topic, LIDAR_Raw_Buffer);

IMUDATA imudata;
LIDARDATA lidar_filtered_data;
LIDARDATA lidar_raw_data;

class UARTTransmitter : public StaticThread <> {

    public:
        UARTTransmitter() : StaticThread("UARTTransmitter", 100 ){}

        void init(){
            red.init(1,1,0);
        }

        char xIMU[9], yIMU[9], zIMU[9], xLIDARFiltered[5], yLIDARFiltered[5], xLIDARRaw[5], yLIDARRaw[5];

        void run() {
            while(1){
                red.setPins(~red.readPins());

                IMU_Buffer.get(imudata);
                LIDAR_Filtered_Buffer.get(lidar_filtered_data);
                LIDAR_Raw_Buffer.get(lidar_raw_data);
                
                toChars(imudata.xForce, xIMU);
                toChars(imudata.yForce, yIMU);
                toChars(imudata.zForce, zIMU);
                toChars(lidar_filtered_data.xDistance, xLIDARFiltered);
                toChars(lidar_filtered_data.yDistance, yLIDARFiltered);
                toChars(lidar_raw_data.xDistance, xLIDARRaw);
                toChars(lidar_raw_data.yDistance, yLIDARRaw);

                PRINTF("%s%s%s%s%s%s%s", xIMU, yIMU, zIMU, xLIDARFiltered, yLIDARFiltered, xLIDARRaw, yLIDARRaw);

                //DEBUG OUTPUT
                //PRINTF("UARTransmitter-----------------\n");
                //PRINTF("IMU:\n%f %f %f\n", imudata.xForce, imudata.yForce, imudata.zForce);
                //PRINTF("LIDAR Filtered:\n%d %d\n", lidar_filtered_data.xDistance, lidar_filtered_data.yDistance);
                //PRINTF("LIDAR Raw:\n%d %d\n", lidar_raw_data.xDistance, lidar_raw_data.yDistance);
                

                suspendCallerUntil(NOW() + 100 * MILLISECONDS);
            }
        }

} uarttransmitter;