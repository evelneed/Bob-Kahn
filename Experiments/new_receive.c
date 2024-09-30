#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

//global variables for tick comparison
uint32_t previous_tick = 0;
uint32_t first_f_tick = 0;
uint32_t first_r_tick = 0;
uint32_t base_time = 0;
uint32_t rising_base_time = 0;
uint32_t falling_base_time = 0;
int first_falling_edge = 0;
int first_rising_edge = 0;
int w = 0;
int results[5]; //come back here for sizing

void sendPulses(int bits[], int length);
int* charToBit(char bits[], int length);

void main() {
	pigpio_start(0,0);
    	set_mode(0, 27, PI_OUTPUT);
	set_mode(0, 20, PI_INPUT);
	gpio_write(0, 27, 0);
	int set_times[4] = {1, 0, 1, 0}; //the header
//	int rising_callback = callback(0, 20, RISING_EDGE, falling);
//	int falling_callback = callback(0, 20, FALLING_EDGE, rising);
	printf("everything initalized \n");
	//the program must start high 
	char buffer[100]; //limited at 99 bits for now
	printf("Please enter a series of 0s and 1s \n");
	scanf("%s", buffer);
	size_t input_length = strlen(buffer);
	char *input_char = (char *)malloc((input_length +1) *sizeof(char)); //allocate the memory
	int *input = (int*)malloc((input_length+1) *sizeof(char)); //make it for ints now
	strcpy(input_char, buffer);
	printf("Input: %s \n", input_char);
//	int *input = charToBit(input_char, input_length);*/
	for (int i = 0; i < input_length; i++){
	//	int ascii = (int) input_char[i];
		input[i] = input_char[i]- '0';
		printf("%d ", input[i]);
	}
	for (int i = 0; i < 4; i++){ //send the header
		gpio_write(0, 27, set_times[i]);
		if (i == 3){
			if (input[0] == 0) {
				usleep(0.1 * 1000000);
			}
			if (input[0] == 1) {
				usleep(0.05 * 1000000); 
			}
		}
		else {
			usleep(0.1 * 1000000);
		}
	}
	sendPulses(input, input_length);
	//size of tester mess
	time_sleep(3);
	free(input);
}

int* charToBit(char bits[], int length){ //come back and make this return a 2d array later, helpful for error correction!
	int* myBits = (int*)malloc(length * sizeof(int) * 8);
	for (size_t i = 0; i < length; i++){
		int asciiVal = (int)bits[i];
		for (int x = 8; x >= 0; x--){
		//	int firstBit = (asciiVal >> (7 - x)) & 1; //and it with a 1 mask to get right bit
			myBits[i * 8 + x] = asciiVal %2;
			int myNum = asciiVal % 2;
			printf("%d", myNum);
			asciiVal /= 2;
		}
	}
	return myBits;
}

void sendPulses(int bits[], int length){
	int gpio=27; //right now this is hardcoded to send from 27, can easily change this though to take in input or something (or send to multiple ports to "broadcast" and communicate with many)
	double delay = 0.1;
//	gpio_write(0, gpio, 0); 
	int state = 0; //starting in low
	for (int bitNum = 0; bitNum < length; bitNum++){ //for every bit, add a pulse to the wave
		printf("bit num: %d \n", bitNum);
		if (bits[bitNum] == 0 && state == 0) { //normal, inc both 0 = rising edge
			gpio_write(0, gpio, 1);
			state = 1;
			if (bitNum != length - 1 && bits[bitNum + 1] ==0){ //if we are entering weirdo land
				usleep((delay / 2) * 1000000);
			}
			else {
				usleep(delay * 1000000);
			}
		}
		else if (bits[bitNum] == 0 && state == 1){ //weird reset one to get in right place, don't increment bits!
			gpio_write(0, gpio, 0);
		    	state = 0;
			bitNum--; //reset the bitnum to cancle out the increment	
			usleep((delay / 2) * 1000000);
		}
		else if (bits[bitNum] == 1 && state ==1){ //normal, inc both
			gpio_write(0, gpio, 0);
			state = 0;
			if (bitNum != length - 1 && bits[bitNum + 1] == 1){ //if we are entering weirdo land
				usleep((delay / 2) * 1000000);
			}
			else {
				usleep(delay * 1000000);
			}
		}
		else if (bits[bitNum] == 1 && state == 0){ //weird reset, don't inc bits!
			gpio_write(0, gpio, 1);
			state = 1;
			bitNum--;
			usleep((delay / 2) * 1000000);
		}
	}
	usleep(0.5 * 1000000);
	if (bits[length - 1] = 0) { //tail
		gpio_write(0, 27, 0);
	}
	else {
		gpio_write(0, 27, 1);
	}
}
