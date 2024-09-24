#include <stdio.h>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <time.h>

int toggle();
void main(){
	pigpio_start(0, 0);
	int total = 0;
       for (int i = 0; i < 1000; i++) {
	   total+= toggle();
	  // sleep(4);
       }
       total = total / 1000;
       printf("%d", total);
}
//averages sending one bit between each of the four tx's and then runs it 1000 times to output the average bps of the machine
int toggle() {	
	int status = gpio_read(0, 21);
	int start = 0;
	int end = 0;
	int time = 0;
	//loop through all receivers and measure their input speed
	for (int i = 21; i <= 27; i+= 2){
		if (status == 1) { //if already on
			start = clock();
			gpio_write(0, i, 0);
	       		end = clock();
			time += end - start;
			//printf("%d \n", time);
		}
		else{
			start = clock();
			gpio_write(0, i, 1);
			end = clock();
			time += end - start;
		}

	}
	return (time / 4);
}
