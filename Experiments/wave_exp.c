#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>

gpioPulse_t[] makePulses(char[] bits); 

void main() {
	pigpio_start(0,0);
	printf("%d \n", wave_tx_busy(0));
	wave_clear(0);
	char tester[4] = {0, 1, 1,0}; //tester sequence
	//make pulse wave
	gpioPulse_t pulses[4] = makePulses(tester);
	wave_add_generic(0, 4, pulses);
	int wave_id = wave_create(0);
	if (wave_id >= 0){ //if succesfully created wave
		int send = wave_send_once(0, wave_id);
	}
}

gpioPulse_t[] makePulses(char[] bits){
	int gpio = 25; //right now this is hardcoded to send from 25, can easily change this though to take in input or something (or send to multiple ports to "broadcast" and communicate with many)
	int length = sizeof(bits) / sizeof(bits[1])
	gpioPulse_t[] pulses[length];
	for (int i = 0; i < length; i++){ //for every bit, add a pulse to the wave
		if (bits[i] == 0) { //if it should be off, need to figure out how to send it as off/0, rather than just turn it off but also depends on encoding technqiue 
			pulses[0].gpioOn = 0;
			pulses[0].gpioOff = 1 << 25; //turn off port
			pulses[0].usDelay = 40; //come back here for delay, will be dependent on encoding technique stuff 
		}
		else if (bits[i] == 1){
			pulses[0].gpioOn = 1<< 25;
			pulses[0].gpioOff = 0;
			pulses[0].usDelay = 40; //come back here for delay, will be dependent on encoding technique stuff 
		}
		//won't be anything else, but could check?
	}
	return pulses;
}

