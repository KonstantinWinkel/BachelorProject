#define AccADDR 0x6B

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"

#include "../include/topics.h"

#include "math.h"
#define R2D 57.2958

HAL_GPIO LED(GPIO_033);

HAL_I2C AnglesIMU(I2C_IDX2);


class IMU_Angles_Reader : public StaticThread <>
{
    private:
        ANGLES angles_data;
        
        float calibrationValues[3] = {0.011, 0.050, 0.079}; //fill through experiments
        float realValues[3] = {0.0, 0.0, 0.0};

        float xAngle = 0;
        float yAngle = 0;

        float filterYsum = 0;
        float filterXsum = 0;

        int16_t rawValues[3] = {0, 0, 0};

        uint8_t X_L[1] = {0x28};
        uint8_t X_H[1] = {0x29};
        uint8_t Y_L[1] = {0x2A};
        uint8_t Y_H[1] = {0x2B};
        uint8_t Z_L[1] = {0x2C};
        uint8_t Z_H[1] = {0x2D};

    public:
    
        IMU_Angles_Reader() : StaticThread("IMU Angles Reader", 100){}

        void calib_axis(int cycles){
            float sumX1 = 0, sumX2 = 0;
            float sumY1 = 0, sumY2 = 0;
            float sumZ1 = 0, sumZ2 = 0;
            
            for(int axis = 0; axis < 3; axis++){

                LED.setPins(0);
                suspendCallerUntil(NOW() + 10*SECONDS);
                LED.setPins(1);

                for (int i = 0; i < cycles; i++)
                {
                    uint8_t DATA_L[1] = {0};
                    uint8_t DATA_H[1] = {0};

                    AnglesIMU.writeRead(AccADDR, X_L, 1, DATA_L, 1);
                    AnglesIMU.writeRead(AccADDR, X_H, 1, DATA_H, 1);
                    rawValues[0] = DATA_L[0] + (DATA_H[0] << 8);

                    AnglesIMU.writeRead(AccADDR, Y_L, 1, DATA_L, 1);
                    AnglesIMU.writeRead(AccADDR, Y_H, 1, DATA_H, 1);
                    rawValues[1] = DATA_L[0] + (DATA_H[0] << 8);

                    AnglesIMU.writeRead(AccADDR, Z_L, 1, DATA_L, 1);
                    AnglesIMU.writeRead(AccADDR, Z_H, 1, DATA_H, 1);
                    rawValues[2] = DATA_L[0] + (DATA_H[0] << 8);

                    //calculate accelerations based on raw values and calibration values
                    for(int i = 0; i < 3; i++){
                        realValues[i] = (0.061/1000) * rawValues[i];
                    }

                    switch(axis){
                        case 0:
                            sumY1 += realValues[1];
                            sumZ1 += realValues[2];
                            break;
                        case 2:
                            sumX1 += realValues[0];
                            sumZ2 += realValues[2];
                            break;
                        case 1:
                            sumX2 += realValues[0];
                            sumY2 += realValues[1];
                            break;
                    }
                }

                suspendCallerUntil(NOW() + 2*SECONDS);
            }

            calibrationValues[0] = (sumX1 + sumX2)/(2*cycles);
            calibrationValues[1] = (sumY1 + sumY2)/(2*cycles);
            calibrationValues[2] = (sumZ1 + sumZ2)/(2*cycles);

            PRINTF("CALIB VALUES: %f %f %f", calibrationValues[0], calibrationValues[1], calibrationValues[2]);
        }

        void calibrate(int cycles){
            float sumX = 0;
            float sumY = 0;
            float sumZ = 0;
            for (int i = 0; i < cycles; i++)
            {
                uint8_t DATA_L[1] = {0};
                uint8_t DATA_H[1] = {0};

                AnglesIMU.writeRead(AccADDR, X_L, 1, DATA_L, 1);
                AnglesIMU.writeRead(AccADDR, X_H, 1, DATA_H, 1);
                rawValues[0] = DATA_L[0] + (DATA_H[0] << 8);

                AnglesIMU.writeRead(AccADDR, Y_L, 1, DATA_L, 1);
                AnglesIMU.writeRead(AccADDR, Y_H, 1, DATA_H, 1);
                rawValues[1] = DATA_L[0] + (DATA_H[0] << 8);

                AnglesIMU.writeRead(AccADDR, Z_L, 1, DATA_L, 1);
                AnglesIMU.writeRead(AccADDR, Z_H, 1, DATA_H, 1);
                rawValues[2] = DATA_L[0] + (DATA_H[0] << 8);

                //calculate accelerations based on raw values and calibration values
                for(int i = 0; i < 3; i++){
                    realValues[i] = (0.061/1000) * rawValues[i];
                }
                sumX += realValues[0];
                sumY += realValues[1];
                sumZ += realValues[2];
            }
            calibrationValues[0] = sumX/cycles;
            calibrationValues[1] = sumY/cycles + 1;
            calibrationValues[2] = sumZ/cycles;
        }

        void init(){
            LED.init(1,1,0);
            AnglesIMU.init(400000);
            uint8_t INIT_REG[2] = {0x20, 0b10000011}; //Address: 0x20, value 0b10000011 - see LuRI Lab for more info
            AnglesIMU.write(AccADDR, INIT_REG, 2);
        }

        void run(){
            
            calib_axis(50);

            TIME_LOOP(0*SECONDS, 50 * MILLISECONDS){
                //Read Registers from IMU
                uint8_t DATA_L[1] = {0};
                uint8_t DATA_H[1] = {0};

                filterYsum = 0;
                filterXsum = 0;

                for(int i = 0; i < 10; i++){

                    AnglesIMU.writeRead(AccADDR, X_L, 1, DATA_L, 1);
                    AnglesIMU.writeRead(AccADDR, X_H, 1, DATA_H, 1);
                    rawValues[0] = DATA_L[0] + (DATA_H[0] << 8);

                    AnglesIMU.writeRead(AccADDR, Y_L, 1, DATA_L, 1);
                    AnglesIMU.writeRead(AccADDR, Y_H, 1, DATA_H, 1);
                    rawValues[1] = DATA_L[0] + (DATA_H[0] << 8);

                    AnglesIMU.writeRead(AccADDR, Z_L, 1, DATA_L, 1);
                    AnglesIMU.writeRead(AccADDR, Z_H, 1, DATA_H, 1);
                    rawValues[2] = DATA_L[0] + (DATA_H[0] << 8);

                    //calculate accelerations based on raw values and calibration values
                    for(int j = 0; j < 3; j++){
                        realValues[j] = (0.061/1000) * rawValues[j] - calibrationValues[j];
                    }
                    
                    filterYsum += atan2(realValues[0], sqrt(realValues[1] * realValues[1] + realValues[2] * realValues[2])) * R2D;
                    filterXsum += atan2(realValues[2], sqrt(realValues[0] * realValues[0] + realValues[1] * realValues[1])) * R2D;

                }
                

                angles_data.xAngle = filterXsum / 10.0;
                angles_data.yAngle = filterYsum / 10.0;

                Angles_Topic.publish(angles_data);
            }
        }

} lol;
