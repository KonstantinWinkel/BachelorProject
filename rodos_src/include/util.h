#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

static float toFloat(char* c){
	unsigned long floatBits = 0;

	for(unsigned int i = 0; i < 8; i++){
		unsigned long currentNumber = 0;
		currentNumber = c[i];
		currentNumber -= 'a';

		floatBits += currentNumber << i*4;
	}
	
	float result = *(float*)&floatBits;
    return result;
}

static uint16_t toUINT16(char* c){
	uint16_t result = 0;

	for(unsigned int i = 0; i < 4; i++){
		uint16_t currentNumber = 0;
		currentNumber = c[i];
		currentNumber -= 'a';

		result += currentNumber << i*4;
	}

	return result;
}

static void toChars(float f, char * result){
	unsigned long floatBits = *(unsigned long*)&f;
	for(unsigned int i = 0; i < 8; i++){
		uint8_t currentNumber = 0; // from 0 to 16;

		for(unsigned int j = 0; j < 4; j++){
			if((floatBits >> (j + i*4))& 1U){
				currentNumber |= 1U << j;
			}
		}	
		currentNumber += 'a';

		result[i] = currentNumber;
	}

	result[8] = '\0';
}

static void toChars(uint16_t u, char * result){
	
	for(unsigned int i = 0; i < 4; i++){
		uint8_t currentNumber = 0;

		for(unsigned int j = 0; j < 4; j++){
			if((u >> (j + 4*i))& 1U){
				currentNumber |= 1U << j;
			}
		}
		currentNumber += 'a';

		result[i] = currentNumber;
	}

	result[4] = '\0';
}

#endif