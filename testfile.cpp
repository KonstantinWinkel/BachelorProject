/*
 * TestBlinky.cpp
 *
 */

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "hal/hal_pwm.h"

HAL_GPIO green(GPIO_060);
HAL_GPIO orange(GPIO_061);
HAL_GPIO red(GPIO_062);
HAL_GPIO blue(GPIO_063);

HAL_PWM motor1(PWM_IDX00);


class Test: public StaticThread<>
{

public:

	 void init()
	 {
		green.init(1,1,0);
		orange.init(1,1,0);
		red.init(1,1,0);
		blue.init(1,1,0);

		motor1.init(5000, 1000);
	 }


void run()
{

	uint32_t temp = 0;

	while(1){
		green.setPins(~green.readPins());

		if(temp == 1000) temp = 0;
		else temp += 100;
		motor1.write(temp);

		AT(NOW() + 1*SECONDS);		
		PRINTF("dinge geschehen");
	}
}

};

Test t;
