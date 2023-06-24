#include "rodos.h"
#include "hal/hal_gpio.h"

#include "../include/util.h"
#include "../include/topics.h"

HAL_GPIO red(GPIO_062);

CommBuffer<IMUDATA> IMU_Buffer;
CommBuffer<LIDARDATA> LIDAR_Buffer;

Subscriber IMU_Subscriber(IMU_Topic, IMU_Buffer);
Subscriber LIDAR_Subscriber(LIDAR_Topic, LIDAR_Buffer);

IMUDATA imudata;
LIDARDATA lidardata;

class UARTTransmitter : public StaticThread <> {

    public:
        UARTTransmitter() : StaticThread("UARTTransmitter", 100 ){}

        void init(){
            red.init(1,1,0);
        }

        char xIMU[9], yIMU[9], zIMU[9], xLIDAR[9], yLIDAR[9];

        void run() {
            while(1){
                red.setPins(~red.readPins());

                IMU_Buffer.get(imudata);
                LIDAR_Buffer.get(lidardata);
                
                toChars(imudata.xForce, xIMU);
                toChars(imudata.yForce, yIMU);
                toChars(imudata.zForce, zIMU);
                toChars(lidardata.xDistance, xLIDAR);
                toChars(lidardata.yDistance, yLIDAR);

                PRINTF("%s%s%s%s%s", xIMU, yIMU, zIMU, xLIDAR, yLIDAR);
                //PRINTF("%f %f %f %d %d\n", imudata.xForce, imudata.yForce, imudata.zForce, lidardata.xDistance, lidardata.yDistance);

                suspendCallerUntil(NOW() + 100 * MILLISECONDS);
            }
        }

} uarttransmitter;