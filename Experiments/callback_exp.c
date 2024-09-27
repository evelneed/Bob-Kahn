#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//global variables for tick comparison
uint32_t previous_tick = 0;
uint32_t first_tick = 0;
uint32_t base_time = 0;
uint32_t rising_base_time = 0;
uint32_t falling_base_time = 0;
int first_falling_edge = 1;
int first_rising_edge = 1;
int w = 0;


//TODO program must start high
//callback for rising edges 
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick){
	printf("loop# %d level: %x \n", w,level);

	//get base_time with header for rising and falling edges 
	//setting base time for rising edge comparison time with header
	if(rising_base_time == 0 && first_rising_edge == 0 && level == 0) {
		rising_base_time = tick - first_tick;
		previous_tick = first_tick;
		printf( "setting rising base: %li loop #: %d \n",rising_base_time,w);
		//printf("base_time: %li  tick: %li  first_tick: %li \n", base_time,tick,first_tick);
	}

	//setting the base_time for falling edge
	if(first_falling_edge && level == 1 && first_rising_edge == 0){
		falling_base_time = tick-previous_tick;
		first_falling_edge = 0; 
		printf("setting falling base time: %li loop#: %d \n", falling_base_time, w);
	}

	//edge case for start
	if (first_rising_edge){
		first_tick = tick;
		first_rising_edge = 0;
	}

	//set base time
	if (level == 0) {
		base_time = rising_base_time;
	}
	else if (level == 1) {
		base_time = falling_base_time;
	}
	else {
		printf("error");
	}

	//get current tick comparison time
	uint32_t current_time = tick - previous_tick;
	//printf("current time: %li, tick: %li, prev_tick %li \n", current_time, tick, previous_tick);

	//check current time vs base time
	double percent_error = fabs(((double)base_time - current_time) / base_time) * 100.0;
	//printf("checked percent error: base_time: %x current_time: %x percent error %.2f%% \n", base_time, current_time, percent_error);
	
	//check its not the first or second state change
	//then see if it is within 25% error of original time if so record it, if not than it is in between change between re[eat numbers
	if ((previous_tick != 0) && (base_time != 0)){
		if (percent_error > 25){ 
			printf(" incorrect transition percent error: %.2f%%  tick: %li curremt_time: %li \n", percent_error,tick ,current_time);
			printf(" rising base time: %li falling base time: %li base time: %li ", rising_base_time, falling_base_time, base_time);
		}
		else{
			printf("correct transition  level: %u percent_error: %.2f%% rising_base_time: %li falling-base_time: %li  \n",level,percent_error,rising_base_time,falling_base_time);
			previous_tick = tick;
			//set the base time to the previous time to avoid time drift
			//base_time = current_time;
		}		
	}
	//previous_tick = tick;
	w++;
}
void main() {
	pigpio_start(0,0);
	//initialize rising and falling callback on port 4 recv
	int rising_callback = callback(0, 20, EITHER_EDGE, call_back);
	//the program must start high
	//tester message the header is 011001 
	int tester_mess[] = {0,1,1,0,0,1,1,0,1,0};
	//size of tester mess
	int size = sizeof(tester_mess)/sizeof(tester_mess[0]);
	//TODO dont rely on setting the state below
	gpio_write(0,27,0);
	//loop through and write to gpio
	for (int i = 0; i < size; i++) {
		gpio_write(0,27,tester_mess[i]);
		sleep(.0001);
	}
	
	time_sleep(3);
}
