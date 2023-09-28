#include "rodos.h"
#include "../include/VL53L4CD_api.h"
#include "../include/VL53L4CD_calibration.h"
#include "../include/platform.h"
#include "../include/topics.h"

#include "hal/hal_gpio.h"

HAL_GPIO green(GPIO_060);

#define MedianLenght 20
#define CalibLength 100
#define XZero 85
#define YZero 99

LIDARDATA FilteredValues;
LIDARDATA RawValues;

class LIDARReader : public StaticThread <> {

    private:
        //for basic reading
        Dev_t dev = 0x0029;
	    uint8_t status, loop, isReady;
        VL53L4CD_ResultsData_t results;	

        //for median filter
        int16_t RawXValues[MedianLenght];
        int16_t RawYValues[MedianLenght];
        
        uint8_t RawIndex[2] = {0, 0};

        int16_t BIAS_X = 0;
        int16_t BIAS_Y = 0;

    //my code

    public:

        LIDARReader() : StaticThread("LIDAR Reader", 5 ){}


        //Util Methods
        void readAndPrint(){
            status = VL53L4CD_CheckForDataReady( dev, &isReady );

            if( isReady ) {
                VL53L4CD_ClearInterrupt( dev );

                /* Read measured distance. RangeStatus = 0 means valid data */
                VL53L4CD_GetResult( dev, &results );
                if(results.range_status <= 2) MedianFilter(getCurrentI2C()-1, results.distance_mm);
                //PRINTF( "%d: Status = %6u, Distance = %6u\n", getCurrentI2C()-1, results.range_status, FilteredValues.xDistance);
            }
        }

        void MedianFilter(uint8_t index, uint16_t newValue){
            if(index == 1) RawXValues[RawIndex[index]] = newValue - BIAS_X;
            if(index == 0) RawYValues[RawIndex[index]] = newValue - BIAS_Y; // -(newValue + YZero);

            int16_t sum = 0;
            for(uint8_t i = 0; i < MedianLenght; i++){
                if(index == 1) sum += RawXValues[i];
                if(index == 0) sum += RawYValues[i];
            }
            //PRINTF("%d\n", sum/MedianLenght);

            if(index == 1){
                FilteredValues.xDistance = sum/MedianLenght;
                RawValues.xDistance = RawXValues[RawIndex[index]];
            }
            
            if(index == 0){
                FilteredValues.yDistance = sum/MedianLenght;
                RawValues.yDistance = RawYValues[RawIndex[index]]; //-(newValue + YZero);
            }

            RawIndex[index]++;
            if(RawIndex[index] >= MedianLenght) RawIndex[index] = 0;
        }

        //Thread Methods

        void init(){
            VL53L4CD_init();
            green.init(1,1,0);

            int16_t x_lib_offset = 0;
            int16_t y_lib_offset = 0;

            setCurrentI2C(1);
            //VL53L4CD_CalibrateOffset(dev, XZero, &x_lib_offset, 100);

            setCurrentI2C(2);
            //VL53L4CD_CalibrateOffset(dev, XZero, &y_lib_offset, 100);


            int32_t sumX = 0;
            int32_t sumY = 0;

            //find bias
            for(int i = 0; i < CalibLength; i++){
                green.setPins(~green.readPins());

                setCurrentI2C(1);
                readAndPrint();

                setCurrentI2C(2);
                readAndPrint();
                sumX += RawValues.xDistance;
                sumY += RawValues.yDistance;
	        }

            BIAS_X = sumX / CalibLength;
            BIAS_Y = sumY / CalibLength;

            for(int i = 0; i < MedianLenght; i++) RawXValues[i] = RawYValues[i] = 0;

        }

        void run() {
            setCurrentI2C(1);
            VL53L4CD_SensorInit( dev );
            status = VL53L4CD_StartRanging( dev );

            setCurrentI2C(2);
            VL53L4CD_SensorInit( dev );
            status = VL53L4CD_StartRanging( dev );

	        TIME_LOOP(0*SECONDS, 100 * MILLISECONDS){
                green.setPins(~green.readPins());

                setCurrentI2C(1);
                readAndPrint();

                setCurrentI2C(2);
                readAndPrint();

                LIDAR_Filtered_Topic.publish(FilteredValues);
                LIDAR_Raw_Topic.publish(RawValues);
	        }
        }

} Ranging;