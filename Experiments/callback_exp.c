#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <stdlib.h>


void call_back(int pi, unsigned gpio, unsigned level, uint32_t tick){
	printf("this is the gpio: %x \n this is the edge %x \n", gpio, level); 
}
void main() {	
	pigpio_start(0,0);
	int bud = callback(0, 20, EITHER_EDGE, call_back);
	//gpio_write(0, 27, 1);
	//gpio_write(0, 27, 0);
	//int x = gpio_read(0,20);
	//printf("this is it: %d", x);
	//printf("\n %d",bud);
	time_sleep(60);
	}
	 

