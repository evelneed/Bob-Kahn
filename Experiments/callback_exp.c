#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

//global variables for tick comparison
uint32_t previous_tick = 0;
uint32_t base_time = 0;
uint32_t rising_base_time = 0;
uint32_t falling_base_time = 0;
int first_falling_edge = 1;
int first_rising_edge = 1;
//not first edge change identifier  
int not_first = 1;
int w = 0;
int i = 0;
//dynamically allocating memory for the messaging data
int results[1000];
int results_size = 0;


//callback for rising edges 
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick){
	printf("loop# %d level: %x \n", w,level);

	//get base_time with header for rising and falling edges 
	//setting base time for rising edge comparison time with header
	if(rising_base_time == 0 && first_rising_edge == 0 && level == 0) {
		rising_base_time = tick - previous_tick;
		not_first = 0;
		//printf( "setting rising base: %li loop #: %d \n",rising_base_time,w);
		//printf("base_time: %li  tick: %li  first_tick: %li \n", base_time,tick,first_tick);
	}

	//setting the base_time for falling edge
	if(first_falling_edge && level == 1 && first_rising_edge == 0){
		falling_base_time = tick-previous_tick;
		first_falling_edge = 0; 
		not_first = 0;
		//printf("setting falling base time: %li loop#: %d \n", falling_base_time, w);
	}

	//edge case for start
	if (first_rising_edge && level == 0){
		previous_tick = tick;
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
	if ((previous_tick != 0) && (base_time != 0) && (not_first == 0)){
		//tail recognition 
		if (percent_error > 300) {
			printf("end of message");
		}
		else if (percent_error > 25){ 
			printf(" incorrect transition percent error: %.2f%%  base_time: %li current_time: %li level: %u \n", percent_error,base_time,current_time,level);
		}
		else{
			printf("correct transition  level: %u percent_error: %.2f%% rising_base_time: %li falling-base_time: %li  \n",level,percent_error,rising_base_time,falling_base_time);
			previous_tick = tick;


			//store values
			if (results_size < 1000) {
				results[results_size] = level;
				results_size++;
			}
			else {
				printf("maximum bits reached");
			}	
		}		
	}
	w++;
}

void process_results() {
	if (results_size < 2) {
		printf("not enough bits to process");
		return;
	}

	//shift the array to remove the header bits
	for (int j = 2; j < results_size; j++) {
		results[j - 2] = results[j];
	}
	//reduce size by 2 for the first two and then back for the tail
	results_size -= 2;

	//flip the bits to display original message (0 to 1s, 1s to 0's)
	for (int j = 0; j <results_size; j++) {
		results[j] = results[j] == 0 ? 1 : 0;
	}

	//display results
	printf("message received: \n");
	for (int j = 0; j < results_size; j++) {
		printf("%d,", results[j]);
	}
}
void main() {
	pigpio_start(0,0);
	//initialize rising and falling callback on port 4 recv
	int call_back_one = callback(0, 20, EITHER_EDGE, call_back);
	//header
/*	
	gpio_write(0,27,1);
	sleep(.5);
	gpio_write(0,27,0);
	sleep(.5);
	gpio_write(0,27,1);
	sleep(.5);
	gpio_write(0,27,0);
	sleep(.5);
	//message
	gpio_write(0,27,1);
	sleep(.5);
	gpio_write(0,27,0);
	sleep(.5);
	//tail could be a high held for 4 time signatures
	gpio_write(0,27,1);
	sleep(1);
	gpio_write(0,27,0);
*/	
	time_sleep(10);
	process_results();
}
