#ifndef UTIL_H
#define UTIL_H
namespace twoChars{
static float toFloat(char* c){
	unsigned long floatBits = 0;

	for(unsigned int i = 0; i < 8; i++){
		unsigned long currentNumber = 0;
		currentNumber = c[i];
		currentNumber -= 97;

		floatBits += currentNumber << i*4;
	}
	
	float result = *(float*)&floatBits;
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
		currentNumber += 97;

		result[i] = currentNumber;
	}

	result[8] = '\0';
}
}

/*
static char* toChars(float f){
	char* result;
	long floatBits = *(long*)&f;
	for(int i = 0; i < 8; i++){
		int currentNumber = 0; // from 0 to 16;

		for(int j = 0; j < 4; j++){
			if((floatBits >> (j + i*4))& 1U){
				currentNumber |= 1U << j;
			}
		}	
		currentNumber += 97;

		result[i] = currentNumber;
	}

	result[8] = '\0';
	return result;
}*/
#endif