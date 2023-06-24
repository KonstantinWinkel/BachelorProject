//c++ things
//#include <string.h>
#include <stdlib.h>

//RODOS things
#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"

#include "../include/util.h"

//using namespace twoChars;

#define MessageLength 19

HAL_PWM servo1(PWM_IDX00); //PE9
HAL_PWM servo2(PWM_IDX01); //PE11
HAL_PWM servo3(PWM_IDX02); //PE13

HAL_GPIO orange(GPIO_061); //this threads "Controll LED"
HAL_GPIO redMotor(GPIO_062); //reserved for calculations

//desired Angles
float servoX = 0;
float servoY = 0;

//utility variables for smoother motion
float curServoX = 0;
float curServoY = 0;

float xChange = 0;
float yChange = 0;

uint8_t frameCounter = 0;

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
			
			
			
			while(1){
				orange.setPins(~orange.readPins());
				if(frameCounter < 5){
					curServoX += xChange;
					curServoY += yChange;
					frameCounter++;
				}

				servo1.write(calculatePWM(curServoX));
				servo2.write(calculatePWM(curServoX));
				servo3.write(calculatePWM(curServoY));

				suspendCallerUntil(NOW() + 1 * MILLISECONDS);
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
		UartReceiver() :	StaticThread("UART Reciever", 10) {}

		void clearMessage(){
			for(int i = 0; i < MessageLength; i++){
				all[i] = 'z';
			}
			//setPriority(10);
			//PRINTF("MESSAGE CLEARED: %s\n", all);
			pos = 0;
		}

		void init(){
			uart_stdout.config(UART_PARAMETER_ENABLE_DMA,1);
			clearMessage();
		}


		void run() {
			
			while (1) {
				if (uart_stdout.read(ch,MessageLength) > 0){
					//PRINTF("%c",ch[0]);   
					all[pos] = ch[0];
					if(all[pos] == 'X'){
						clearMessage();
						all[0] = 'X';
					}
					pos++;
				}

				if(all[pos-1] == '#' || pos > 18){
					
					if(all[0] != 'X' || all[9] != 'Y' || all[18] != '#'){
						clearMessage();
						continue; //XaaaaaaaaYaaaaaipd#
					}

					//get X & Y Value
					char xString[8] = "";
					char yString[8] = "";
					for(int i = 0; i < 8; i++){
						xString[i] = all[1+i];
						yString[i] = all[10+i];
					}

					servoX = toFloat(xString);
					servoY = toFloat(yString);

					xChange = (servoX - curServoX)/5.0f;
					yChange = (servoY - curServoY)/5.0f;
					frameCounter = 0;

					clearMessage();
				}

				uart_stdout.suspendUntilDataReady();
			}
			
		}
};
UartReceiver uartRX;