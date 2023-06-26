#include "rodos.h"
#include "../include/VL53L4CD_api.h"
#include "../include/platform.h"
#include "../include/topics.h"

#include "hal/hal_gpio.h"

HAL_GPIO green(GPIO_060);

#define MedianLenght 20

LIDARDATA FilteredValues;
LIDARDATA RawValues;

class LIDARReader : public StaticThread <> {

    private:
        //for basic reading
        Dev_t dev = 0x0029;
	    uint8_t status, loop, isReady;
        VL53L4CD_ResultsData_t results;	

        //for median filter
        uint16_t RawXValues[MedianLenght];
        uint16_t RawYValues[MedianLenght];
        
        uint8_t RawIndex[2] = {0, 0};

    //my code

    public:

        LIDARReader() : StaticThread("LIDAR Reader", 100 ){}

        void init(){
            VL53L4CD_init();
            green.init(1,1,0);
            
            for(int i = 0; i < MedianLenght; i++) RawXValues[i] = RawYValues[i] = 0;
        }

        void MedianFilter(uint8_t index, uint16_t newValue){
            if(index == 0) RawXValues[RawIndex[index]] = newValue;
            if(index == 1) RawYValues[RawIndex[index]] = newValue;

            RawIndex[index]++;
            if(RawIndex[index] >= MedianLenght) RawIndex[index] = 0;

            uint16_t sum = 0;
            for(uint8_t i = 0; i < MedianLenght; i++){
                if(index == 0) sum += RawXValues[i];
                if(index == 1) sum += RawYValues[i];
            }
            //PRINTF("%d\n", sum/MedianLenght);

            if(index == 0){
                FilteredValues.xDistance = sum/MedianLenght;
                RawValues.xDistance = newValue;
                
                //PRINTF("Filtered Value: %d\n", FilteredValues.xDistance );
                return;
            }
            
            if(index == 1){
                FilteredValues.yDistance = sum/MedianLenght;
                RawValues.yDistance = newValue;
                return;
            }
        }

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

        void run() {
            setCurrentI2C(1);
            VL53L4CD_SensorInit( dev );
            status = VL53L4CD_StartRanging( dev );

            setCurrentI2C(2);
            VL53L4CD_SensorInit( dev );
            status = VL53L4CD_StartRanging( dev );

	        while(1) {

                green.setPins(~green.readPins());

                setCurrentI2C(1);
                readAndPrint();

                setCurrentI2C(2);
                readAndPrint();

                LIDAR_Filtered_Topic.publish(FilteredValues);
                LIDAR_Raw_Topic.publish(RawValues);

                suspendCallerUntil( NOW() + 5*MILLISECONDS );
	        }
        }

} Ranging;