#define AccADDR 0x6B

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"

#include "util.h" //converting flote to char* and vice versa

using namespace twoChars;

HAL_GPIO redIMU(GPIO_062); //reserved for calculations
HAL_GPIO blue(GPIO_063); //this threads "Controll LED"

HAL_I2C IMU(I2C_IDX2);

//IMU wirering:
//IMU    STM
//GND -> GND
//3.3 -> 3V
//SCL -> PB10 
//SDA -> PB11

//stripped down version of the IMU excercise from the LuRI Lab. Only accelerometer is needed.
class IMUReader : public StaticThread <>
{

    private:
        float calibrationValues[3]; //fill through experiments
        float realValues[3] = {0.0, 0.0, 0.0};

        int rawValues[3] = {0, 0, 0};

        //Local Adresses of registers
        uint8_t X_L[1] = {0x28};
        uint8_t X_H[1] = {0x29};
        uint8_t Y_L[1] = {0x2A};
        uint8_t Y_H[1] = {0x2B};
        uint8_t Z_L[1] = {0x2C};
        uint8_t Z_H[1] = {0x2D};

    public:

        float adjust(float realValue){
            if(realValue > 1) return realValue-4;
        }

        float squareSum(float x,float y,float z){
            return x*x+y*y+z*z;
        }

        void calibrate(int cycles){
            float sumX = 0;
            float sumY = 0;
            float sumZ = 0;
            for (int i = 0; i < cycles; i++)
            {
                uint8_t DATA_L[1] = {0};
                uint8_t DATA_H[1] = {0};

                IMU.writeRead(AccADDR, X_L, 1, DATA_L, 1);
                IMU.writeRead(AccADDR, X_H, 1, DATA_H, 1);
                rawValues[0] = DATA_L[0] + (DATA_H[0] << 8);

                IMU.writeRead(AccADDR, Y_L, 1, DATA_L, 1);
                IMU.writeRead(AccADDR, Y_H, 1, DATA_H, 1);
                rawValues[1] = DATA_L[0] + (DATA_H[0] << 8);

                IMU.writeRead(AccADDR, Z_L, 1, DATA_L, 1);
                IMU.writeRead(AccADDR, Z_H, 1, DATA_H, 1);
                rawValues[2] = DATA_L[0] + (DATA_H[0] << 8);

                //calculate accelerations based on raw values and calibration values
                for(int i = 0; i < 3; i++){
                    realValues[i] = (0.061/1000) * rawValues[i]  - calibrationValues[i];
                }
                sumX += adjust(realValues[0]);
                sumY += adjust(realValues[1]);
                sumZ += adjust(realValues[2]);
            }
            calibrationValues[0] = sumX/cycles;
            calibrationValues[1] = sumY/cycles;
            calibrationValues[2] = sumZ/cycles;
        }

        void init()
        {
            redIMU.init(1,1,0);
		    blue.init(1,1,0);

            redIMU.setPins(1); //begin of setup
            IMU.init(400000);
            uint8_t INIT_REG[2] = {0x20, 0b10000011}; //Address: 0x20, value 0b10000011 - see LuRI Lab for more info
            IMU.write(AccADDR, INIT_REG, 2);
            redIMU.setPins(0); //end of setup

            calibrate(50);
        }

        void run()
        {
            while(1){
                blue.setPins(~blue.readPins());
                
                //Read Registers from IMU
                uint8_t DATA_L[1] = {0};
                uint8_t DATA_H[1] = {0};

                IMU.writeRead(AccADDR, X_L, 1, DATA_L, 1);
                IMU.writeRead(AccADDR, X_H, 1, DATA_H, 1);
                rawValues[0] = DATA_L[0] + (DATA_H[0] << 8);

                IMU.writeRead(AccADDR, Y_L, 1, DATA_L, 1);
                IMU.writeRead(AccADDR, Y_H, 1, DATA_H, 1);
                rawValues[1] = DATA_L[0] + (DATA_H[0] << 8);

                IMU.writeRead(AccADDR, Z_L, 1, DATA_L, 1);
                IMU.writeRead(AccADDR, Z_H, 1, DATA_H, 1);
                rawValues[2] = DATA_L[0] + (DATA_H[0] << 8);

                //calculate accelerations based on raw values and calibration values
                for(int i = 0; i < 3; i++){
                    realValues[i] = (0.061/1000) * rawValues[i]  - calibrationValues[i];
                }

                //send real accerlation values via UART back
                char xChar[9],yChar[9],zChar[9];
                toChars(adjust(realValues[0]),xChar);
                toChars(adjust(realValues[1]),yChar);
                toChars(adjust(realValues[2]),zChar);
                PRINTF("%s%s%s", xChar, yChar, zChar); //12 + 3*4 = 26 warum auch immer

                AT(NOW() + 100*MILLISECONDS);	
            }
        }
};

IMUReader imureader;
