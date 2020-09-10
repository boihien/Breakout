#ifndef __TASK_SCHEDULER_h__
#define __TASK_SCHEDULER_h__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

//--------Find GCD function --------------------------------------------------
unsigned char findGCD(unsigned char a, unsigned char b)
{
	unsigned char c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned char period; //Task period
	unsigned char elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;


#endif