//c++ things
//#include <string.h>
#include <stdlib.h>

//RODOS things
#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"

#include "util.h"

#define MessageLength 19

HAL_PWM servo1(PWM_IDX00); //PE9
HAL_PWM servo2(PWM_IDX01); //PE11
HAL_PWM servo3(PWM_IDX02); //PE13

HAL_GPIO orange(GPIO_061); //this threads "Controll LED"
HAL_GPIO redMotor(GPIO_062); //reserved for calculations

float servoX = 0;
float servoY = 0;

namespace RODOS {
	extern HAL_UART uart_stdout;
}

class MotorController : public StaticThread<>
{
    private:
        

    public:
        void init(){
            servo1.init(50, 4000);
            servo2.init(50, 4000);
            servo3.init(50, 4000);

			orange.init(1,1,0);
        }

		unsigned int ServoSet(float percentage) //0% = -60° , 100% = 60°
    	{
        	return (unsigned int)(1820 - 2 * (-60+6/5*percentage));
    	}

		float P2A(float percentage){
			return percentage * 120 - 60;
		}

		unsigned int calculatePWM(float Angle)
        {
            return (unsigned int)(1920 - 2 * Angle);
        }

        void run(){
			while(1){
				orange.setPins(~orange.readPins());
				servo1.write(calculatePWM(P2A(servoX)));
				servo2.write(calculatePWM(P2A(servoX)));
				servo3.write(calculatePWM(P2A(servoY)));

				AT(NOW() + 10*MILLISECONDS);
			}
        }
};

MotorController motorcontroller;


//black magic from here on out
char all[MessageLength];
int pos = 0;

char *eptr;

class UartIOEventReceiver : public IOEventReceiver{
	public:
		void onWriteFinished() {
			static uint32_t ledLevel = 0;
			ledLevel ^= 1;
		}

		void onDataReady() {
			static uint32_t ledLevel = 0;
			ledLevel ^= 1;
		}
};
UartIOEventReceiver uartIOEventReceiver;


class UartTransmitter: public Thread {

	private:

	public:
		UartTransmitter(const char* name) : Thread(name) {
		}

		void init(){
			uart_stdout.config(UART_PARAMETER_ENABLE_DMA,1);
		}

		void run() {
			while (1) {
				if(all[0] == 'k'){
					PRINTF("Message Timeout\n");
				}
				else{
					//PRINTF("%s\n", all);

					if(all[0] != 'X' || all[9] != 'Y') continue; //XaaaaaaaaYaaaaaipd#

					//get X Value
					char xString[8] = "";
					for(int i = 0; i < 8; i++){
						xString[i] = all[1+i];
					}
					servoX = toFloat(xString);
					
					//get Y Value
					char yString[8] = "";
					for(int i = 0; i < 8; i++){
						yString[i] = all[10+i];
					}
					servoY = toFloat(yString);

					PRINTF("%f %f \n", servoX, servoY);

				}

				for(int i = 0; i < MessageLength; i++){
					all[i] = 'k';
				}
				pos = 0;
				suspendCallerUntil(NOW()+5000*MILLISECONDS);
			}
		}
};
UartTransmitter uartTX("UartTestTX");


class UartReceiver: public Thread{
	public:
		UartReceiver(const char* name) :	Thread(name) {
		}

		void init(){
			uart_stdout.setIoEventReceiver(&uartIOEventReceiver);
			for(int i = 0; i < MessageLength; i++){
				all[i] = 'k';
			}
		}

		void run() {
			char ch[MessageLength];

			while (1) {
				if (uart_stdout.read(ch,MessageLength) > 0){
					//PRINTF("%c",ch[0]);   
					all[pos] = ch[0];
					pos++;
				}

				if(all[pos-1] == '#'){
					PRINTF("Response Length: %d Message: %s\n", strlen(all), all);
					uartTX.resume();
				}

				uart_stdout.suspendUntilDataReady();
			}
		}
};
UartReceiver uartRX("UartTestRX");