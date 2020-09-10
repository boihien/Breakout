/*
 * main.c.c
 *
 * Created: 01-Jun-19 17:21:49
 * Author : Boi-Hien Huang
 */ 
#include <avr/io.h>
#include "timer.h"
#include "scheduler.h"

#include "songTick.c"
#include "LEDTick.c"
#include "playTick.c"

int main(void){
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	
	double NOTE_D5 = 587;
	double NOTE_F5 = 698;
	double NOTE_D6 = 1175;
	double NOTE_E6 = 1319;
	double NOTE_F6 = 1397;
	double NOTE_C6 = 1047;
	double NOTE_A5 = 880;
	double NOTE_G5 = 784;
	double NOTE_E5 = 659;
	
	sound song_of_storms[] = {	(sound){NOTE_D5, 150, 50}, (sound){NOTE_F5, 150, 50}, (sound){NOTE_D6, 300, 100}, //80
	(sound){NOTE_D5, 150, 50}, (sound){NOTE_F5, 150, 50}, (sound){NOTE_D6, 300, 100},

	(sound){NOTE_E6, 300, 100}, (sound){NOTE_F6, 100, 100}, (sound){NOTE_E6, 100, 100}, //100
	(sound){NOTE_F6, 100, 50}, (sound){NOTE_E6, 100, 100}, (sound){NOTE_C6, 100, 100},
	(sound){NOTE_A5, 200, 200},

	(sound){NOTE_A5, 200, 100}, (sound){NOTE_D5, 200, 100}, (sound){NOTE_F5, 100, 100}, //100
	(sound){NOTE_G5, 100, 100}, (sound){NOTE_A5, 350, 200},

	(sound){NOTE_A5, 300, 100}, (sound){NOTE_D5, 200, 100}, (sound){NOTE_F5, 100, 100},
	(sound){NOTE_G5, 100, 100}, (sound){NOTE_E5, 350, 200},

	(sound){NOTE_D5, 150, 50}, (sound){NOTE_F5, 150, 50}, (sound){NOTE_D6, 300, 100}, //80
	(sound){NOTE_D5, 150, 50}, (sound){NOTE_F5, 150, 50}, (sound){NOTE_D6, 300, 100},

	(sound){NOTE_E6, 300, 100}, (sound){NOTE_F6, 100, 100}, (sound){NOTE_E6, 100, 100}, //100
	(sound){NOTE_F6, 100, 50}, (sound){NOTE_E6, 100, 100}, (sound){NOTE_C6, 100, 100},
	(sound){NOTE_A5, 200, 200}, //300

	(sound){NOTE_A5, 200, 100}, (sound){NOTE_D5, 200, 100}, (sound){NOTE_F5, 100, 100},
	(sound){NOTE_G5, 100, 100}, (sound){NOTE_A5, 300, 100}, (sound){NOTE_A5, 200, 100},
	(sound){NOTE_D5, 300, 2000}
};

song1 = song_of_storms;
end_song1 = song_of_storms + 43;
	
	
	static task task5;
	task *tasks[] = {&task5};
	unsigned char SMTick5_period = 2;
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	task5.state = WaitLED;//Task initial state.
	task5.period = SMTick5_period;//Task Period.
	task5.elapsedTime = SMTick5_period; // Task current elapsed time.
	task5.TickFct = &LED_Tick; // Function pointer for the tic

	TimerSet(1);
	TimerOn();
	unsigned short i; // Scheduler for-loop iterator
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime >= tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	// Error: Program should not exit!
	return 0;
}