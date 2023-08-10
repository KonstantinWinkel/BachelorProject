//c++ things
//#include <string.h>
#include <stdlib.h>
#include <limits>

//RODOS things
#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"
#include "hal/hal_uart.h"

#include "../include/util.h"
#include "../include/topics.h"

#define MessageLength 19 //19
#define MAX_POS 5.0
#define MIN_POS -5.0

HAL_PWM servo1(PWM_IDX00); //PE9
HAL_PWM servo2(PWM_IDX01); //PE11
HAL_PWM servo3(PWM_IDX02); //PE13

HAL_GPIO orange(GPIO_061); //this threads "Controll LED"
HAL_GPIO redMotor(GPIO_062); //reserved for calculations
HAL_GPIO button(GPIO_000);

CommBuffer<CONTROLLERDATA> Controller_Buffer;

Subscriber Controller_Subscriber(Controller_Topic, Controller_Buffer);

//desired Angles
float acc_x = 0;
float acc_y = 0;

float vel_x = 0;
float vel_y = 0;

float pos_x = 0;
float pos_y = 0;

namespace RODOS {
	extern HAL_UART uart_stdout;
}


class MotorController : public StaticThread <>
{
    private:

        //float vel_x = 0;
        //float vel_y = 0;

        //float pos_x = 0;
        //float pos_y = 0;

        float constants[3] = {0,0.1,0};
        
    public:
        MotorController() : StaticThread("lmao", 100){}

        void checkBounds(){
			bool X_OUT_OF_BOUNDS = false;
			bool Y_OUT_OF_BOUNDS = false;

			if(pos_x > MAX_POS){
				X_OUT_OF_BOUNDS = true;
				pos_x = MAX_POS;
			}

			if(pos_x < MIN_POS){
				X_OUT_OF_BOUNDS = true;
				pos_x = MIN_POS;
			}

			if(pos_y > MAX_POS){
				Y_OUT_OF_BOUNDS = true;
				pos_y = MAX_POS;
			}

			if(pos_y < MIN_POS){
				Y_OUT_OF_BOUNDS = true;
				pos_y = MIN_POS;
			}

			if(X_OUT_OF_BOUNDS){
				//acc_x = 0;
				vel_x = 0;
			}

			if(Y_OUT_OF_BOUNDS){
				//acc_y = 0;
				vel_y = 0;
			}
		}

        void init(){
            servo1.init(50, 4000);
            servo3.init(50, 4000);
            orange.init(1,1,0);
			redMotor.init(1,1,0);
			button.init(0,1,0);
        }

        void run(){

            CONTROLLERDATA data;

            while(1){
                orange.setPins(~orange.readPins());

                if(~button.readPins()) Controller_Buffer.get(data);
                //vel_x = -2 * data.xAngle;
                //vel_y = -2 * data.yAngle;

                //pos_x = data.xValue;
                //pos_y = data.yValue;

				pos_x += 0.5*acc_x/1000000 + vel_x/1000;
				pos_y += 0.5*acc_y/1000000 + vel_y/1000;

                
                servo1.write(calculatePWM(Pos2A(pos_x)));
                servo3.write(calculatePWM(Pos2A(pos_y)));

                checkBounds();

                suspendCallerUntil(NOW() + 5*MILLISECONDS);


            }
        }

        float Pos2A(float position){
            return position * (50.0f/5.0f);
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

} motorcontroller;

//black magic from here on out
char all[MessageLength];
int pos = 0;

char ch[MessageLength];

class UartReceiver: public StaticThread<>{
	public:
		UartReceiver() :	StaticThread("UART Reciever", 1000) {}

		void clearMessage(){
			for(int i = 0; i < MessageLength; i++){
				all[i] = 'z';
			}
			pos = 0;
		}

		void init(){
			uart_stdout.init();
			uart_stdout.config(UART_PARAMETER_ENABLE_DMA,MessageLength);
			clearMessage();
		}

		void CheckForSpecialValues(float value){

			vel_x = vel_x = 0;
			acc_x = acc_y = 0;

			switch ((int)value)
			{
				case 0: pos_x = 0;       pos_y = 0;       break;
				case 1: pos_x = MAX_POS; pos_y = MIN_POS; break;
				case 2: pos_x = MAX_POS; pos_y = MAX_POS; break;
				case 3: pos_x = MIN_POS; pos_y = MAX_POS; break;
				case 4: pos_x = MIN_POS; pos_y = MIN_POS; break;
					
				default:break;
			}
		}


		void run() {
			
			while (1) {
				size_t readLength = uart_stdout.read(all,MessageLength);


				if(all[18] == '#' ){
					
					if(all[0] != 'X' || all[9] != 'Y' || all[18] != '#'){
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
						//tString[i] = all[19+i];
					}
					
					float value_x = toFloat(xString);
					float value_y = toFloat(yString);

					if(value_x == std::numeric_limits<float>::max()) CheckForSpecialValues(value_y);
					else{
						vel_x = value_x;
						vel_y = value_y;

						//PRINTF("%f %f \n", vel_x, vel_y);
					}

					clearMessage();
				}

				uart_stdout.suspendUntilDataReady();
				//suspendCallerUntil(NOW() + 3 *MILLISECONDS);
			}
			
		}
};
UartReceiver uartRX;
