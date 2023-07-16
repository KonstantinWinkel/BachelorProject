/*
 * TestBlinky.cpp
 *
 */

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_pwm.h"
#include "hal/hal_uart.h"

#include <stdio.h>

namespace RODOS {
	extern HAL_UART uart_stdout;
}

HAL_GPIO green(GPIO_060);
HAL_GPIO orange(GPIO_061);
HAL_GPIO red(GPIO_062);
HAL_GPIO blue(GPIO_063);

HAL_PWM motor1(PWM_IDX00); //PE9

HAL_UART testUART(UART_IDX2);

class Test: public StaticThread<>
{

public:

	void init()
	{
		green.init(1,1,0);
		orange.init(1,1,0);
		red.init(1,1,0);
		blue.init(1,1,0);

		motor1.init(1000, 1000);

		testUART.init(115200);
	}


	void run()
	{

		uint32_t temp = 500;

		while(1){
			green.setPins(~green.readPins());

			if(temp == 1000) temp = 0;
			else temp += 100;
			motor1.write(temp);

			AT(NOW() + 1*SECONDS);		
			PRINTF("dinge geschehen");
			testUART.write("abcd\n", 5);
		}
	}

};

Test t;

//taken from IMUExercise
/*

char all[13];
int pos = 0;

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
	int nAcc;
	int nGyro;

public:
    UartTransmitter(const char* name) :    Thread(name) {
    }

    void init(){
        uart_stdout.config(UART_PARAMETER_ENABLE_DMA,1);
    }

    void run() {
        while (1) {
        	if(all[0] == 'k'){
            	uart_stdout.write("Message Timeout\n",16);
        	}
        	else{
				//if(validate) execute();
				execute();
        	}

        	for(int i = 0; i < 13; i++){
				all[i] = 'k';
			}
			pos = 0;
        	suspendCallerUntil(NOW()+5000*MILLISECONDS);
        }
    }


    void execute(){
    	char co[2] = {all[1], all[2]};
    	all[2] = ' ';
    	for(int i = 0; i < sizeof(all); i++){
    			if(all[i] == '#' ){
    				all[i] = ' ';
    				break;
    			}
    		}
    	int data;
    	sscanf(all, "%*s%d", &data);

    	if(co[0] == 'T' && co[1] == 'E'){
    		orange.setPins(~orange.readPins());
		}
    	else{
    		uart_stdout.write("Message Corrupted\n",18);
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
	    for(int i = 0; i < 13; i++){
	    	all[i] = 'k';
	    }
	}

	void run() {
		char ch[13];

		while (1) {
			if (uart_stdout.read(ch,13) >0){
				//xprintf("character received: %c\n",ch[0]);
				all[pos] = ch[0];
				pos++;
			}
			if(all[pos-1] == '#'){
				uartTX.resume();
			}

			uart_stdout.suspendUntilDataReady();
		}
	}
};
UartReceiver uartRX("UartTestRX");
*/
