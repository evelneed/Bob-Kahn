#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>

gpioPulse_t* makePulses(char bits[], int length);
int calculateLength(char bits[], int length);

void main() {
	pigpio_start(0,0);
	set_mode(0, 27, PI_OUTPUT);
	wave_clear(0);
	int charLen = 5;
	char tester[5] = {0, 1, 1, 1, 0}; //tester sequence
	int newLen = calculateLength(tester, charLen); //get new length based on repeats
	printf("%d \n", newLen); //free heap somewhere
	//make pulse wave
	gpio_write(0,27,0);
	gpioPulse_t* pulses = makePulses(tester, newLen);
	wave_add_generic(0, newLen, pulses);
	free(pulses);
	int wave_id = wave_create(0);
	int send;
	printf("%d \n", wave_get_pulses(0)); 
	if (wave_id >= 0){ //if succesfully created wave
		send = wave_send_once(0, wave_id);
	}
	printf("%d \n", pulses[0].gpioOff);
	printf("%d \n", pulses[1].gpioOff);
	printf("%d \n", pulses[2].gpioOff);
	printf("%d \n", pulses[3].gpioOff);
	printf("%d \n", pulses[4].gpioOff);
	printf("%d \n", pulses[5].gpioOff);
	printf("%d \n", pulses[6].gpioOff);
}

int calculateLength(char bits[], int length){//counts how many bits will be needed for encoded
	int count = length;
	for (int i = 0; i < length-1; i++){
		if (bits[i] == bits[i+1]){
			count++;
		}
	}
	return count;	
}


gpioPulse_t* makePulses(char bits[], int length){
	int gpio=27; //right now this is hardcoded to send from 27, can easily change this though to take in input or something (or send to multiple ports to "broadcast" and communicate with many)
	gpioPulse_t* pulses = (gpioPulse_t*)malloc(sizeof(gpioPulse_t) * length);
	int bitNum = 0;
	int delay = 2000000000; //come back here
	int state = 0; //starting in low state, come back here
	for (int i = 0; i < length; i++){ //for every bit, add a pulse to the wave
		if (bits[bitNum] == 0 && state == 0) { //normal, inc both 
			pulses[i].gpioOn = 1<<gpio;
			pulses[i].gpioOff = 0; //turn off port
			if (bits[bitNum + 1] == 0){ // if we are entering weird half time
				pulses[i].usDelay = delay / 2;
			}
			else {
				pulses[i].usDelay = delay;} 
			state = 1;
			bitNum++;
		}
		else if (bits[bitNum] == 0 && state == 1){ //weird reset one to get in right place, don't increment bits!
			pulses[i].gpioOn = 0;
			pulses[i].gpioOff = 1<<gpio;
			pulses[i].usDelay = delay/2; //might need to make this half, talk to oliver
		        state = 0;	
		}
		else if (bits[bitNum] == 1 && state ==1){ //normal, inc both
			pulses[i].gpioOn = 0;
			pulses[i].gpioOff = 1<<gpio;
			if (bits[bitNum + 1] == 1){ //if we are entering weirdo land
				pulses[i].usDelay = delay / 2;
			}
			else {
				pulses[i].usDelay = delay;
			}
			state = 0;
			bitNum++;
		}
		else if (bits[bitNum] == 1 && state == 0){ //weird reset, don't inc bits!
			pulses[i].gpioOn = 1<<gpio;
			pulses[i].gpioOff = 0;
			pulses[i].usDelay = delay/2;
			state = 1;
		}
	}
	return pulses;
}

