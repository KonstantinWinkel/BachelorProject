#define AccADDR 0x6B //also includes gyro
#define MagADDR 0x1E

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"

#include "../include/topics.h"

#include "math.h"
#define R2D 57.2958

HAL_GPIO LED(GPIO_033);

HAL_I2C AnglesIMU(I2C_IDX2);

//Calibration, Y then Z then X

class IMU_Angles_Reader : public StaticThread <>
{
    private:
        IMUDATA data;

        //Buffers for data to be read into
        uint8_t DATA_L[1] = {0};
        uint8_t DATA_H[1] = {0};

        /*
         *  Buffers for all processed values
         */
        
        //GYRO
        float GYR_CALIB_VALS[3] = {0.0, 0.0, 0.0}; //fill through experiments ?
        float GYR_REAL_VALS[3] = {0.0, 0.0, 0.0};
        int16_t GYR_RAW_VALS[3] = {0, 0, 0};

        //ACCELEROMETER
        float ACC_CALIB_VALS[3] = {0.011, 0.050, 0.079}; //fill through experiments ?
        float ACC_REAL_VALS[3] = {0.0, 0.0, 0.0};
        int16_t ACC_RAW_VALS[3] = {0, 0, 0};

        //MAGNETOMETER
        //TODO how tf does this calib work????
        float MAG_CALIB_MAX[3] = {0.0, 0.0, 0.0};
        float MAG_CALIB_MIN[3] = {0.0, 0.0, 0.0};
        float MAG_REAL_VALS[3] = {0.0, 0.0, 0.0};
        int16_t MAG_RAW_VALS[3] = {0, 0, 0};

        bool is_calibrating = true;

        /*
         *  REGISTER ADDRESSES
         */
        
        //GYRO
        uint8_t GYR_X_L[1] = {0x18};
        uint8_t GYR_X_H[1] = {0x19};
        uint8_t GYR_Y_L[1] = {0x1A};
        uint8_t GYR_Y_H[1] = {0x1B};
        uint8_t GYR_Z_L[1] = {0x1C};
        uint8_t GYR_Z_H[1] = {0x1D};

        //ACCELLEROMETER
        uint8_t ACC_X_L[1] = {0x28};
        uint8_t ACC_X_H[1] = {0x29};
        uint8_t ACC_Y_L[1] = {0x2A};
        uint8_t ACC_Y_H[1] = {0x2B};
        uint8_t ACC_Z_L[1] = {0x2C};
        uint8_t ACC_Z_H[1] = {0x2D};

        //MAGNETOMETER
        uint8_t MAG_X_L[1] = {0x28};
        uint8_t MAG_X_H[1] = {0x29};
        uint8_t MAG_Y_L[1] = {0x2A};
        uint8_t MAG_Y_H[1] = {0x2B};
        uint8_t MAG_Z_L[1] = {0x2C};
        uint8_t MAG_Z_H[1] = {0x2D};

    public:
    
        IMU_Angles_Reader() : StaticThread("IMU Angles Reader", 100){

        }


        void read_GYR(){
            AnglesIMU.writeRead(AccADDR, GYR_X_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(AccADDR, GYR_X_H, 1, DATA_H, 1);
            GYR_RAW_VALS[0] = DATA_L[0] + (DATA_H[0] << 8);

            AnglesIMU.writeRead(AccADDR, GYR_Y_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(AccADDR, GYR_Y_H, 1, DATA_H, 1);
            GYR_RAW_VALS[1] = DATA_L[0] + (DATA_H[0] << 8);

            AnglesIMU.writeRead(AccADDR, GYR_Z_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(AccADDR, GYR_Z_H, 1, DATA_H, 1);
            GYR_RAW_VALS[2] = DATA_L[0] + (DATA_H[0] << 8);

            for(int j = 0; j < 3; j++){
                GYR_REAL_VALS[j] = (70/1000) * GYR_RAW_VALS[j] - GYR_CALIB_VALS[j];
            }
        }

        void read_ACC(){
            AnglesIMU.writeRead(AccADDR, ACC_X_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(AccADDR, ACC_X_H, 1, DATA_H, 1);
            ACC_RAW_VALS[0] = DATA_L[0] + (DATA_H[0] << 8);

            AnglesIMU.writeRead(AccADDR, ACC_Y_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(AccADDR, ACC_Y_H, 1, DATA_H, 1);
            ACC_RAW_VALS[1] = DATA_L[0] + (DATA_H[0] << 8);

            AnglesIMU.writeRead(AccADDR, ACC_Z_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(AccADDR, ACC_Z_H, 1, DATA_H, 1);
            ACC_RAW_VALS[2] = DATA_L[0] + (DATA_H[0] << 8);

            for(int j = 0; j < 3; j++){
                ACC_REAL_VALS[j] = (0.061/1000) * ACC_RAW_VALS[j] - ACC_CALIB_VALS[j];
            }
        }

        void read_MAG(){
            AnglesIMU.writeRead(MagADDR, MAG_X_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(MagADDR, MAG_X_H, 1, DATA_H, 1);
            MAG_RAW_VALS[0] = DATA_L[0] + (DATA_H[0] << 8);

            AnglesIMU.writeRead(MagADDR, MAG_Y_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(MagADDR, MAG_Y_H, 1, DATA_H, 1);
            MAG_RAW_VALS[1] = DATA_L[0] + (DATA_H[0] << 8);

            AnglesIMU.writeRead(MagADDR, MAG_Z_L, 1, DATA_L, 1);
            AnglesIMU.writeRead(MagADDR, MAG_Z_H, 1, DATA_H, 1);
            MAG_RAW_VALS[2] = DATA_L[0] + (DATA_H[0] << 8);

            for(int j = 0; j < 3; j++){
                if(!is_calibrating) MAG_REAL_VALS[j] = ((0.14/1000) * MAG_RAW_VALS[j] - MAG_CALIB_MIN[j]) / (MAG_CALIB_MAX[j] - MAG_CALIB_MIN[j]) * 2 - 1;
                else MAG_REAL_VALS[j] = (0.14/1000) * MAG_RAW_VALS[j];
            }
        }

        void show_next_calib(int type){
            switch (type)
            {
                case 0:
                    for(int i = 0; i < 6; i++){
                        LED.setPins(~LED.readPins());
                        suspendCallerUntil(NOW() + 500*MILLISECONDS);
                    }
                    break;
                
                case 1:
                    for(int i = 0; i < 20; i++){
                        LED.setPins(~LED.readPins());
                        suspendCallerUntil(NOW() + 100*MILLISECONDS);
                    }
                default: break;
            }
        }

        void calib_acc(int cycles){
            float sumX1 = 0, sumX2 = 0;
            float sumY1 = 0, sumY2 = 0;
            float sumZ1 = 0, sumZ2 = 0;
            
            for(int axis = 0; axis < 3; axis++){

                LED.setPins(0);
                suspendCallerUntil(NOW() + 10*SECONDS);
                LED.setPins(1);

                for (int i = 0; i < cycles; i++)
                {

                    read_ACC();

                    //calculate accelerations based on raw values and calibration values
                    for(int i = 0; i < 3; i++){
                        ACC_REAL_VALS[i] = (0.061/1000) * ACC_RAW_VALS[i];
                    }

                    switch(axis){
                        case 0:
                            sumY1 += ACC_REAL_VALS[1];
                            sumZ1 += ACC_REAL_VALS[2];
                            break;
                        case 2:
                            sumX1 += ACC_REAL_VALS[0];
                            sumZ2 += ACC_REAL_VALS[2];
                            break;
                        case 1:
                            sumX2 += ACC_REAL_VALS[0];
                            sumY2 += ACC_REAL_VALS[1];
                            break;
                    }
                }

                suspendCallerUntil(NOW() + 2*SECONDS);
            }

            ACC_CALIB_VALS[0] = (sumX1 + sumX2)/(2*cycles);
            ACC_CALIB_VALS[1] = (sumY1 + sumY2)/(2*cycles);
            ACC_CALIB_VALS[2] = (sumZ1 + sumZ2)/(2*cycles);

            PRINTF("CALIB VALUES: %f %f %f", ACC_CALIB_VALS[0], ACC_CALIB_VALS[1], ACC_CALIB_VALS[2]);
        }

        void calib_gyro(int cycles){

            float GYR_SUM[3] = {0.0, 0.0, 0.0};

            LED.setPins(1);
            for(int i = 0; i < cycles; i++){
                read_GYR();

                for(int j = 0; j < 3; j++){
                    GYR_SUM[j] += (70/1000) * GYR_RAW_VALS[j];
                }
            }

            for(int j = 0; j < 3; j++){
                GYR_CALIB_VALS[j] = GYR_SUM[j] / cycles;
            }

            LED.setPins(0);
        }

        void calib_mag(int time){
            LED.setPins(1);

            for(int i = 0; i < time*1000; i++){
                read_MAG();
                for(int j = 0; j < 3; j++){
                    if(MAG_REAL_VALS[j] < MAG_CALIB_MIN[j]) MAG_CALIB_MIN[j] = MAG_REAL_VALS[j];
                    if(MAG_REAL_VALS[j] > MAG_CALIB_MAX[j]) MAG_CALIB_MAX[j] = MAG_REAL_VALS[j];
                }
                suspendCallerUntil(NOW() + 1*MILLISECONDS);
            }

            LED.setPins(0);

        }

        void calib_all(int cycles_acc, int cycles_gyro, int time_mag){
            show_next_calib(0);
            calib_acc(cycles_acc);

            show_next_calib(0);
            calib_gyro(cycles_gyro);
            
            show_next_calib(0);
            calib_mag(time_mag);

            show_next_calib(1);
            is_calibrating = false;
        }

        void init(){
            LED.init(1,1,0);
            AnglesIMU.init(400000);

            uint8_t INIT_REG_GYR[2] = {0x10, 0b10011000};
            uint8_t INIT_REG_ACC[2] = {0x20, 0b10000011}; //Address: 0x20, value 0b10000011 - see LuRI Lab for more info
            uint8_t INIT_REG_MAG_1[2] = {0x20, 0b00011100};
            uint8_t INIT_REG_MAG_2[2] = {0x21, 0b01100000};
            uint8_t INIT_REG_MAG_3[2] = {0x22, 0b00000000};
            
            AnglesIMU.write(AccADDR, INIT_REG_GYR, 2);
            AnglesIMU.write(AccADDR, INIT_REG_ACC, 2);
            AnglesIMU.write(MagADDR, INIT_REG_MAG_1, 2);
            AnglesIMU.write(MagADDR, INIT_REG_MAG_2, 2);
            AnglesIMU.write(MagADDR, INIT_REG_MAG_3, 2);
        }

        void run(){
            
            calib_all(50,50, 10);

            TIME_LOOP(0*SECONDS, 50 * MILLISECONDS){

                /*
                 *  READ REGISTERS FROM IMU
                 */

                //GYRO
                read_GYR();

                //ACCELLEROMETER
                read_ACC();

                //MAGNETOMETER
                read_MAG();

                //publish values
                data.xAngularVelocity = GYR_REAL_VALS[0];
                data.yAngularVelocity = GYR_REAL_VALS[1];
                data.zAngularVelocity = GYR_REAL_VALS[2];

                data.xForce = ACC_REAL_VALS[0];
                data.yForce = ACC_REAL_VALS[1];
                data.zForce = ACC_REAL_VALS[2];

                data.xMagneticFlux = MAG_REAL_VALS[0];
                data.yMagneticFlux = MAG_REAL_VALS[1];
                data.zMagneticFlux = MAG_REAL_VALS[2];

                Full_IMU_Topic.publish(data);
            }
        }

} imu_angles_reader;