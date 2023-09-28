//c++ things
//#include <string.h>
#include <stdlib.h>

//RODOS things
#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"
#include "hal/hal_uart.h"

#include "../include/util.h"
#include "../include/topics.h"

#define MessageLength 28 //19

HAL_PWM servo1(PWM_IDX00); //PE9
HAL_PWM servo2(PWM_IDX01); //PE11
HAL_PWM servo3(PWM_IDX02); //PE13

HAL_GPIO orange(GPIO_061); //this threads "Controll LED"
HAL_GPIO redMotor(GPIO_062); //reserved for calculations

//desired Angles
float servoX = 0;
float servoY = 0;
float time_frame = 0.1;

//utility variables for smoother motion
float curServoX = 0;
float curServoY = 0;

float xChange = 0;
float yChange = 0;

float lastServoX;
float lastServoY;
float nextServoX;
float nextServoY;

namespace RODOS {
	extern HAL_UART uart_stdout;
}

class MotorController : public StaticThread<>
{
    private:

    public:

		MotorController() : StaticThread("Motor Controller", 1000) {
		}

		int i = 0;

        void init(){
            servo1.init(50, 4000);
            servo2.init(50, 4000);
            servo3.init(50, 4000);

			orange.init(1,1,0);
        }

		float P2A(float percentage){
			return percentage * 120 - 60;
		}

		//for 4k  inc: 1920 - 2*Angle
		//for 8k  inc: 3840 - 4*Angle
		//for 16k inc: 7680 - 8*Angle
		unsigned int calculatePWM(float Angle)
        {
            return (unsigned int)(1920 - 2 * Angle);//orig:4
        }



        void run(){
			
			float tempy = 0, tempx=0;

			
			TIME_LOOP(0*SECONDS, 1 * MILLISECONDS){
				orange.setPins(~orange.readPins());

				curServoX += xChange;
				curServoY += yChange;

				servo1.write(calculatePWM(curServoX));
				servo2.write(calculatePWM(curServoX));
				servo3.write(calculatePWM(curServoY));

				suspendCallerUntil(NOW() + 1 * MILLISECONDS); // was 1
			}
        }
};
MotorController motorcontroller;


//black magic from here on out
char all[MessageLength];
int pos = 0;

char ch[MessageLength];

class UartReceiver: public StaticThread<>{
	public:
		UartReceiver() :	StaticThread("UART Reciever", 1000) {}

		void clearMessage(){
			//PRINTF("%s\n", all);
			for(int i = 0; i < MessageLength; i++){
				all[i] = 'z';
			}
			//setPriority(10);
			//PRINTF("MESSAGE CLEARED: %s\n", all);
			pos = 0;
		}

		void init(){
			uart_stdout.init();
			uart_stdout.config(UART_PARAMETER_ENABLE_DMA,MessageLength);
			clearMessage();
		}


		void run() {
			
			while (1) {
				size_t readLength = uart_stdout.read(all,MessageLength);
				if (readLength > 0){
					//PRINTF("\n");
					//for(int i = 0; i < readLength; i++){
					//	all[i] = ch[i];
					//}
					//PRINTF("\n");
					//PRINTF("%c",ch[0]);   
					//all[pos] = ch[0];
					//if(all[pos] == 'X'){
					//	clearMessage();
					//	all[0] = 'X';
					//}
					//pos++;
				}

				if(all[27] == '#' ){

					
					if(all[0] != 'X' || all[9] != 'Y' || all[18] != 'T' || all[27] != '#'){
						clearMessage();
						continue; //XaaaaaaaaYaaaaaipd#
					}

					//get X & Y Value
					char xString[9] = "";
					char yString[9] = "";
					char tString[9] = "";
					for(int i = 0; i < 8; i++){
						xString[i] = all[1+i];
						yString[i] = all[10+i];
						tString[i] = all[19+i];
					}

					//PRINTF("\n %s %s %s \n", xString, yString, tString);

					//servoX = toFloat(xString);
					//servoY = toFloat(yString);

					curServoX = nextServoX;
					curServoY = nextServoY;

					lastServoX = nextServoX;
					lastServoY = nextServoY;

					nextServoX = toFloat(xString);
					nextServoY = toFloat(yString);
					time_frame = toFloat(tString);

					xChange = (nextServoX - lastServoX)/(time_frame * 1000);
					yChange = (nextServoY - lastServoY)/(time_frame * 1000);

					clearMessage();
				}

				uart_stdout.suspendUntilDataReady();
				//suspendCallerUntil(NOW() + 3 *MILLISECONDS);
			}
			
		}
};
UartReceiver uartRX;