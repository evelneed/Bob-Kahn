#include <stdio.h>
#include <string.h>
#include "link_layer.h"  // Include the header for the link layer

//get functions from the link_layer
extern void sendPulses(int bits[], int length);
extern void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick);
extern int* charToBit(char bits[], int length);
extern void process_results();
extern void reset_variables();
extern void send_header(int bits[]);


void main() {
    pigpio_start(0, 0);
    set_mode(0, 27, PI_OUTPUT);
    set_mode(0, 22, PI_INPUT);

    // Start receiving via callback
    int call_back_one = callback(0, 22, EITHER_EDGE, call_back);
    int set_times[4] = {1, 0, 1, 0}; // the header

    while (1) {
        if (globals.message_done) {
            globals.message_done = 0;
                // Limit input to 99 characters and ensure no buffer overflow
            char buffer[100];  
            printf("Please enter a series of 0s and 1s (up to 99 bits):\n");
            fgets(buffer, sizeof(buffer), stdin);  // Use fgets to safely get input

            size_t input_length = strlen(buffer);
            
            // Remove newline character from fgets input
            if (buffer[input_length - 1] == '\n') {
                buffer[input_length - 1] = '\0';
                input_length--;
            }

            // Allocate memory for input_char and input arrays
            char *input_char = (char *)malloc((input_length + 1) * sizeof(char));
            int *input = (int *)malloc((input_length) * sizeof(int));

            strcpy(input_char, buffer);
            printf("Input: %s\n", input_char);

            for (int i = 0; i < input_length; i++) {
                input[i] = input_char[i] - '0';
            }

            send_header(input);
            sendPulses(input, input_length);
            time_sleep(1);

            // Reset state for the next message
            reset_variables();

            free(input);
	        free(input_char);
            time_sleep(1);
        }
    }
}


