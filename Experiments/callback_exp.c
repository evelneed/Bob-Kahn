#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//global variables for tick comparison
uint32_t previous_tick = 0;
uint32_t first_tick = 0;
uint32_t base_time = 0;
int first_transition = 1;
int w = 0;


//TODO figure out the issues with percent error as it is getting caught on that with the previous tick not changin and so the time difference gets larger and larger
//issue is mainly with previous tick
//check logic again and error lenience with the tick checking 


void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick){
	//setting base comparison time with header
	if(base_time == 0 && first_transition == 0) {
		base_time = tick - first_tick;
		previous_tick = first_tick;
		//printf("base_time: %x  tick: %x  first_tick: %x \n", base_time,tick,first_tick);
	}

	//edge case for start
	if (first_transition){
		first_tick = tick;
		first_transition = 0;
	}

	//get current tick comparison time
	uint32_t current_time = tick - previous_tick;
	//printf("current time: %x, tick: %x, prev_tick %x", current_time, tick, previous_tick);

	//check current time vs base time
	double percent_error = fabs(((double)base_time - current_time) / base_time) * 100.0;
	//printf("checked percent error: base_time: %x current_time: %x percent error %.2f%% \n", base_time, current_time, percent_error);
	
	//check its not the first or second state change
	//then see if it is within 25% error of original time if so record it, if not than it is in between change between re[eat numbers
	if ((previous_tick != 0) && (base_time != 0)){
		if (percent_error > 25){ 
			printf(" incorrect transition percent error: %.2f%%  tick: %x previous_tick: %x base_time: %x curremt_time: %x \n", percent_error,tick,previous_tick,base_time,current_time);
		}
		else{
			printf("correct transition  level: %u percent_error: %.2f%%  \n",level,percent_error);
			//previous_tick = tick;
			//set the base time to the previous time to avoid time drift
			//base_time = current_time;
		}		
	}
	previous_tick = tick;
	w++;
		
}
void main() {	
	pigpio_start(0,0);
	//initialize callback on port for recv
	int bud = callback(0, 20, EITHER_EDGE, call_back);
	
	//tester message with 10 as the header and the rest being a normal message
	int tester_mess[] = {1,0,1,0,0,1,1,0,1,1};
	//size of tester array
	int size = sizeof(tester_mess)/sizeof(tester_mess[0]);

	//loop through and write each to the gpio
	for (int i = 0; i <size; i++) {
		gpio_write(0,27,tester_mess[i]);
		sleep(.0001);
	}
	time_sleep(3);
	}
	 

