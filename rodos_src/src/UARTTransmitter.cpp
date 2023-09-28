#define WAIT_TIME 10 //10 

#include "rodos.h"
#include "hal/hal_gpio.h"

#include "../include/util.h"
#include "../include/topics.h"

HAL_GPIO red(GPIO_062);

CommBuffer<IMUDATA> IMU_Buffer;
CommBuffer<IMUDATA> Full_IMU_Buffer;
CommBuffer<LIDARDATA> LIDAR_Filtered_Buffer;
CommBuffer<LIDARDATA> LIDAR_Raw_Buffer;
CommBuffer<CONTROLLERDATA> Controller_Buffer;
CommBuffer<ANGLES> Angles_Buffer;

Subscriber IMU_Subscriber(IMU_Topic, IMU_Buffer);
Subscriber Full_IMU_Subscriber(Full_IMU_Topic, Full_IMU_Buffer);
Subscriber LIDAR_Filtered_Subscriber(LIDAR_Filtered_Topic, LIDAR_Filtered_Buffer);
Subscriber LIDAR_Raw_Subscriber(LIDAR_Raw_Topic, LIDAR_Raw_Buffer);
Subscriber Controller_Subscriber(Controller_Topic, Controller_Buffer);
Subscriber Angles_Subscriber(Angles_Topic, Angles_Buffer);

class UARTTransmitter : public StaticThread <> {

    private:
        IMUDATA imu_data;
        IMUDATA full_imu_data;
        LIDARDATA lidar_filtered_data;
        LIDARDATA lidar_raw_data;
        CONTROLLERDATA controller_data;
        ANGLES angles_data;

        char xFloat[9], yFloat[9], zFloat[9];
        char xUInt[5], yUInt[5], zUInt[5];

    public:
        UARTTransmitter() : StaticThread("UARTTransmitter", 100 ){}

        void init(){
            red.init(1,1,0);
        }

        void readAndSend_IMU(){
            if(IMU_Buffer.getOnlyIfNewData(imu_data)){
                toChars(imu_data.xForce, xFloat);
                toChars(imu_data.yForce, yFloat);
                toChars(imu_data.zForce, zFloat); //not transmitted

                PRINTF("0X%sY%sZzzzzzzzz#", xFloat, yFloat);
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);
            }
        }

        void readAndSend_LIDAR_RAW(){
            if(LIDAR_Raw_Buffer.getOnlyIfNewData(lidar_raw_data)){
                toChars(lidar_raw_data.xDistance, xUInt);
                toChars(lidar_raw_data.yDistance, yUInt);

                PRINTF("2X%szzzzY%szzzzZzzzzzzzz#", xUInt, yUInt);
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);
            }
        }

        void readAndSend_Controller(){
            if(Controller_Buffer.getOnlyIfNewData(controller_data)){
                toChars(controller_data.xValue, xFloat);
                toChars(controller_data.yValue, yFloat);

                PRINTF("3X%sY%sZzzzzzzzz#", xFloat, yFloat);
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);
            }
        }

        void readAndSend_Angles(){
            if(Angles_Buffer.getOnlyIfNewData(angles_data)){
                    toChars(angles_data.xAngle, xFloat);
                    toChars(angles_data.yAngle, yFloat);

                    PRINTF("4X%sY%sZzzzzzzzz#", xFloat, yFloat);
                    suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);
                }
        }

        void readAndSend_FullIMU(){
            if(Full_IMU_Buffer.getOnlyIfNewData(full_imu_data)){
                toChars(full_imu_data.xForce, xFloat);
                toChars(full_imu_data.yForce, yFloat);
                toChars(full_imu_data.zForce, zFloat);

                PRINTF("5X%sY%sZ%s#", xFloat, yFloat, zFloat);
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);

                toChars(full_imu_data.xAngularVelocity, xFloat);
                toChars(full_imu_data.yAngularVelocity, yFloat);
                toChars(full_imu_data.zAngularVelocity, zFloat);

                PRINTF("6X%sY%sZ%s#", xFloat, yFloat, zFloat);
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);

                toChars(full_imu_data.xMagneticFlux, xFloat);
                toChars(full_imu_data.yMagneticFlux, yFloat);
                toChars(full_imu_data.zMagneticFlux, zFloat);

                PRINTF("7X%sY%sZ%s#", xFloat, yFloat, zFloat);
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);
            }
        }

        void run() {

            while(1){
                red.setPins(~red.readPins());

                readAndSend_IMU();
                readAndSend_LIDAR_RAW();
                readAndSend_Controller();
                readAndSend_Angles();
                readAndSend_FullIMU();


                //PRINTF("%s%s%s%s%s%s%s#", xIMU, yIMU, zIMU, xLIDARFiltered, yLIDARFiltered, xLIDARRaw, yLIDARRaw);
                //PRINTF("%s%s%s", xIMU, yIMU, zIMU);

                //DEBUG OUTPUT
                //PRINTF("UARTransmitter-----------------\n");
                //PRINTF("IMU:\n%f %f %f\n", imudata.xForce, imudata.yForce, imudata.zForce);
                //PRINTF("LIDAR Filtered:\n%d %d\n", lidar_filtered_data.xDistance, lidar_filtered_data.yDistance);
                //PRINTF("LIDAR Raw:\n%d %d\n", lidar_raw_data.xDistance, lidar_raw_data.yDistance);
                
                //PRINTF("1XaaaazzzzYaaaazzzz#");
                suspendCallerUntil(NOW() + WAIT_TIME * MILLISECONDS);
            }
        }

} uarttransmitter;