#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// Global variables for tick comparison (receive functionality)
uint32_t previous_tick = 0;
uint32_t base_time = 0;
uint32_t rising_base_time = 0;
uint32_t falling_base_time = 0;
int first_falling_edge = 1;
int first_rising_edge = 1;
int not_first = 1;
int w = 0;
int i = 0;
int results[1000];   // Dynamically allocated for received data
int results_size = 0;
int message_done = 1;

void sendPulses(int bits[], int length);
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick);
int* charToBit(char bits[], int length);
void process_results();

void main() {
	pigpio_start(0,0);
    	set_mode(0, 27, PI_OUTPUT);
	set_mode(0, 22, PI_INPUT);
	// Start receiving via callback
    int call_back_one = callback(0, 22, EITHER_EDGE, call_back);
	int set_times[4] = {1, 0, 1, 0}; //the header
//	int rising_callback = callback(0, 20, RISING_EDGE, falling);
//	int falling_callback = callback(0, 20, FALLING_EDGE, rising);
	//printf("everything initalized \n");
	//the program must start high 
    while(1){
	    if(message_done){
		    message_done = 0;
        char buffer[100]; //limited at 99 bits for now
        printf("Please enter a series of 0s and 1s \n");
        scanf("%s", buffer);
        size_t input_length = strlen(buffer);
        char *input_char = (char *)malloc((input_length +1) *sizeof(char)); //allocate the memory
        int *input = (int*)malloc((input_length+1) *sizeof(char)); //make it for ints now
        strcpy(input_char, buffer);
        printf("Input: %s \n", input_char);
    //	int *input = charToBit(input_char, input_length);
        for (int i = 0; i < input_length; i++){
        //	int ascii = (int) input_char[i];
            input[i] = input_char[i]- '0';
       //     printf("%d ", input[i]);
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
        time_sleep(1);

        // Reset state for the next message
            results_size = 0;
            previous_tick = 0;
            base_time = 0;
            rising_base_time = 0;
            falling_base_time = 0;
            first_falling_edge = 1;
            first_rising_edge = 1;
            not_first = 1;

        free(input);
        time_sleep(1);
    }
    }
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
	int state = 0; //starting in low
	for (int bitNum = 0; bitNum < length; bitNum++){ //for every bit, add a pulse to the wave
		//printf("bit num: %d \n", bitNum);
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
	if (bits[length - 1] == 0) { //tail
		gpio_write(0, 27, 0);
	}
	else {
		gpio_write(0, 27, 1);
	}
}

// Callback for receiving functionality
void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick) {
    //printf("loop# %d level: %x \n", w, level);

    // Set the base time for rising and falling edges
    if (rising_base_time == 0 && first_rising_edge == 0 && level == 0) {
        rising_base_time = tick - previous_tick;
        not_first = 0;
    }

    if (first_falling_edge && level == 1 && first_rising_edge == 0) {
        falling_base_time = tick - previous_tick;
        first_falling_edge = 0;
        not_first = 0;
    }

    if (first_rising_edge && level == 0) {
        previous_tick = tick;
        first_rising_edge = 0;
    }

    if (level == 0) {
        base_time = rising_base_time;
    } else if (level == 1) {
        base_time = falling_base_time;
    }

    uint32_t current_time = tick - previous_tick;
    double percent_error = fabs(((double)base_time - current_time) / base_time) * 100.0;

    if ((previous_tick != 0) && (base_time != 0) && (not_first == 0)) {
        if (percent_error > 300) {
            process_results();
            printf("End of message\n");
        } else if (percent_error > 25) {
           // printf("Incorrect transition percent error: %.2f%% base_time: %li current_time: %li level: %u \n", 
            //       percent_error, base_time, current_time, level);
        } else {
            //printf("Correct transition level: %u percent_error: %.2f%% \n", level, percent_error);
            previous_tick = tick;

            if (results_size < 1000) {
                results[results_size] = level;
                results_size++;
            } else {
                printf("Maximum bits reached\n");
            }
        }
    }
    w++;
}

// Process received results
void process_results() {
    if (results_size < 2) {
        printf("Not enough bits to process\n");
        return;
    }

    // Shift the array to remove the header bits
    for (int j = 2; j < results_size; j++) {
        results[j - 2] = results[j];
    }

    results_size -= 2;

    // Flip the bits to display original message (0 to 1s, 1s to 0s)
    for (int j = 0; j < results_size; j++) {
        results[j] = results[j] == 0 ? 1 : 0;
    }

    // Display the results
    printf("Message received: \n");
    for (int j = 0; j < results_size; j++) {
        printf("%d,", results[j]);
    }
    message_done = 1;
}
