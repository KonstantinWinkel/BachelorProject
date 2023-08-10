#include "rodos.h"
#include "hal/hal_gpio.h"

#include "../include/util.h"
#include "../include/topics.h"

HAL_GPIO red(GPIO_062);

class UARTTransmitter : public StaticThread <> {

    private:
        IMUDATA imudata;
        LIDARDATA lidar_filtered_data;
        LIDARDATA lidar_raw_data;
        CONTROLLERDATA controller_data;
        ANGLES angles_data;

        char xIMU[9], yIMU[9], zIMU[9];
        char xLIDARFiltered[5], yLIDARFiltered[5];
        char xLIDARRaw[5], yLIDARRaw[5];
        char xController[9], yController[9];
        char xAngle[9], yAngle[9];

    public:
        UARTTransmitter() : StaticThread("UARTTransmitter", 100 ){}

        void init(){
            red.init(1,1,0);
        }

        void run() {

            CommBuffer<IMUDATA> IMU_Buffer;
            CommBuffer<LIDARDATA> LIDAR_Filtered_Buffer;
            CommBuffer<LIDARDATA> LIDAR_Raw_Buffer;
            CommBuffer<CONTROLLERDATA> Controller_Buffer;
            CommBuffer<ANGLES> Angles_Buffer;

            Subscriber IMU_Subscriber(IMU_Topic, IMU_Buffer);
            Subscriber LIDAR_Filtered_Subscriber(LIDAR_Filtered_Topic, LIDAR_Filtered_Buffer);
            Subscriber LIDAR_Raw_Subscriber(LIDAR_Raw_Topic, LIDAR_Raw_Buffer);
            Subscriber Controller_Subscriber(Controller_Topic, Controller_Buffer);
            Subscriber Angles_Subscriber(Angles_Topic, Angles_Buffer);

            while(1){
                red.setPins(~red.readPins());

                IMU_Buffer.get(imudata);
                LIDAR_Filtered_Buffer.get(lidar_filtered_data);
                LIDAR_Raw_Buffer.get(lidar_raw_data);
                Controller_Buffer.get(controller_data);
                Angles_Buffer.get(angles_data);
                
                toChars(imudata.xForce, xIMU);
                toChars(imudata.yForce, yIMU);
                toChars(imudata.zForce, zIMU); //not transmitted

                toChars(lidar_filtered_data.xDistance, xLIDARFiltered);
                toChars(lidar_filtered_data.yDistance, yLIDARFiltered);
                toChars(lidar_raw_data.xDistance, xLIDARRaw);
                toChars(lidar_raw_data.yDistance, yLIDARRaw);

                toChars(controller_data.xValue, xController);
                toChars(controller_data.yValue, yController);

                toChars(angles_data.xAngle, xAngle);
                toChars(angles_data.yAngle, yAngle);

                PRINTF("0X%sY%s#", xIMU, yIMU);
                suspendCallerUntil(NOW() + 10 * MILLISECONDS);

                PRINTF("1X%szzzzY%szzzz#", xLIDARFiltered, yLIDARFiltered);
                suspendCallerUntil(NOW() + 10 * MILLISECONDS);

                PRINTF("2X%szzzzY%szzzz#", xLIDARRaw, yLIDARRaw);
                suspendCallerUntil(NOW() + 10 * MILLISECONDS);

                PRINTF("3X%sY%s#", xController, yController);
                suspendCallerUntil(NOW() + 10 * MILLISECONDS);

                PRINTF("4X%sY%s#", xAngle, yAngle);
                suspendCallerUntil(NOW() + 10 * MILLISECONDS);


                //PRINTF("%s%s%s%s%s%s%s#", xIMU, yIMU, zIMU, xLIDARFiltered, yLIDARFiltered, xLIDARRaw, yLIDARRaw);
                //PRINTF("%s%s%s", xIMU, yIMU, zIMU);

                //DEBUG OUTPUT
                //PRINTF("UARTransmitter-----------------\n");
                //PRINTF("IMU:\n%f %f %f\n", imudata.xForce, imudata.yForce, imudata.zForce);
                //PRINTF("LIDAR Filtered:\n%d %d\n", lidar_filtered_data.xDistance, lidar_filtered_data.yDistance);
                //PRINTF("LIDAR Raw:\n%d %d\n", lidar_raw_data.xDistance, lidar_raw_data.yDistance);
                

                //suspendCallerUntil(NOW() + 50 * MILLISECONDS);
            }
        }

} uarttransmitter;