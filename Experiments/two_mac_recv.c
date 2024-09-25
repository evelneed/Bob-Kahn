#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>

void main() {
	
	pigpio_start(0,0);
	int recv[4];
	int ready_check[4];
	int pattern[1000];
	int i = 0;
	int z = 0;
	/*while ((gpio_read(0,20)) != 0) {
		ready_check[z] = gpio_read(0,20);
		if ( z >= 4) {
			z == 0;
			//printf("%d", ready_check[z]);
			continue;
		}
		z++;
			
	}
	*/
	while (i < 1000) {
		pattern[i] = gpio_read(0,20);
		i++;
	}	
	//read through and print the inputed bits
	printf("recieved sequence: ");
	for (int c= 0; c < 1000; c++){
		printf("%d", pattern[c]);
	}
}	 

