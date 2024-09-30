#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void sendPulses(int bits[], int length);
int* charToBit(char bits[], int length);
void call_back(int pi, unsigned gpio, unsigned level, uint32_t);
void sendHeader();
void startSend();
void setVariables()

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
    setVariables();
    startSend();
}


void main() {
	pigpio_start(0,0);
    set_mode(0, 27, PI_OUTPUT);
	set_mode(0, 20, PI_INPUT);
    //initialize rising and falling callback on port 4 recv
    startSend();
    time_sleep(10);
	//size of tester mess
	time_sleep(3);
    process_results();
	free(input);
}

void startSend(){
    char buffer[100]; //limited at 99 bits for now
	printf("Please enter a series of 0s and 1s to send \n");
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
		//printf("%d ", input[i]);
	}
    sendHeader();
    sendPulses(input, input_length);
}

void sendHeader(){
    gpio_write(0, 27, 0);
    int set_times[4] = {1, 0, 1, 0}; //the header
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
}

void setVariables(){
    //global variables for tick comparison
previous_tick = 0;
base_time = 0;
rising_base_time = 0;
falling_base_time = 0;
first_falling_edge = 1;
first_rising_edge = 1;
//not first edge change identifier  
not_first = 1;
w = 0;
i = 0;
//dynamically allocating memory for the messaging data
results[1000];
results_size = 0;
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
	int call_back_one = callback(0, 20, EITHER_EDGE, call_back);
}
