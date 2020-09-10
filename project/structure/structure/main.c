#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include "system.h"
#include "matrix_send.h"
#include "task_scheduler.h"
#include "io.c"
#include <string.h>
#include <stdlib.h>

void CONTROLLER();
const unsigned char CLEAR = 0x00;
ROW ROW_0, ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7;
ROW *ROWS[8] = { &ROW_0, &ROW_1, &ROW_2, &ROW_3, &ROW_4, &ROW_5, &ROW_6, &ROW_7};

//PADDLE TO BE USE INSIDE THE GAME
PADDLE PAD1;
PADDLE *PADDLE1[1] = {&PAD1};

//BALL TO BE USED INSIDE THE GAME
BALL ball;
BALL *BALL1[1] = {&ball};


enum SM1_States {WAITING,ROW0,ROW1,ROW2,ROW3,ROW4,ROW5,ROW6,ROW7};
enum SM2_States {INIT, WAIT, PRESS_INC, DEPRESS_INC, PRESS_DEC, DEPRESS_DEC};
enum SM3_States {STEADY, START, MOVE_BALL, GMOV,GWON, RESTORE};
enum SM4_States {BEGAN, Began2, GAME_START, LOOSE, WON};
enum SM5_States {OUT};
	
int SMTick1(int state);
int SMTick2(int state);
int SMTick3(int state);
int SMTick4(int state);
int SMTick5(int state);


int main()
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFB; PORTC = 0x04;
	DDRD = 0xFF; PORTD = 0x00;

	LCD_init();
	PWM_on();
	
	// Period for the tasks
	unsigned long int SMTick1_calc = 1;//display
	unsigned long int SMTick2_calc = 100;//paddle
	unsigned long int SMTick3_calc = 150;//ball.SPEED;
	unsigned long int SMTick4_calc = 100;//game states
	unsigned long int SMTick5_calc = 500; //score

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc  );
	tmpGCD = findGCD(tmpGCD, SMTick3_calc);
	tmpGCD = findGCD(tmpGCD, SMTick4_calc);
	tmpGCD = findGCD(tmpGCD, SMTick5_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;
	
	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
	unsigned long int SMTick5_period = SMTick5_calc/GCD;

	//Declare an array of tasks
	static task task1, task2, task3, task4, task5;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &SMTick1;//Function pointer for the tick.
	
	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = SMTick2_period;//Task Period.
	task2.elapsedTime = SMTick2_period;//Task current elapsed time.
	task2.TickFct = &SMTick2;//Function pointer for the tick.
	
	
	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = SMTick3_period;//Task Period.
	task3.elapsedTime = SMTick3_period; // Task current elasped time.
	task3.TickFct = &SMTick3; // Function pointer for the tick.
	
	// Task 4
	task4.state = -1;//Task initial state.
	task4.period = SMTick4_period;//Task Period.
	task4.elapsedTime = SMTick4_period; // Task current elasped time.
	task4.TickFct = &SMTick4; // Function pointer for the tick.
	
	// Task 5
	task5.state = -1;//Task initial state.
	task5.period = SMTick5_period;//Task Period.
	task5.elapsedTime = SMTick5_period; // Task current elasped time.
	task5.TickFct = &SMTick5; // Function pointer for the tick.
	
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();

	unsigned short i; // Scheduler for-loop iterator

	while(1) {
		CONTROLLER();
		
		for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
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

// controls the Display logic
int SMTick1(int state)
{

	PADDLE_SET(PADDLE1, ROWS);
	switch (state)
	{
		case -1:
		state = WAITING;
		break;
		case WAITING:
		if(begin == 0) state = WAITING;
		else if (begin == 1) state = ROW0;
		break;
		case ROW0:
		state = ROW1;
		break;
		case ROW1:
		state = ROW2;
		break;
		case ROW2:
		state = ROW3;
		break;
		case ROW3:
		state = ROW4;
		break;
		case ROW4:
		state = ROW5;
		break;
		case ROW5:
		state = ROW6;
		break;
		case ROW6:
		state = ROW7;
		break;
		case ROW7:
		state = ROW0;
		break;
		default:
		state = -1;
		break;
	}
	switch (state) {
		case ROW0:
		transmit_dataA1(ROWS[0]->Y); //Port A
		transmit_data(ROWS[0]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW1:
		transmit_dataA1(ROWS[1]->Y); //Port A
		transmit_data(ROWS[1]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW2:
		transmit_dataA1(ROWS[2]->Y); //Port A
		transmit_data(ROWS[2]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW3:
		transmit_dataA1(ROWS[3]->Y); //Port A
		transmit_data(ROWS[3]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW4:
		transmit_dataA1(ROWS[4]->Y); //Port A
		transmit_data(ROWS[4]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW5:
		transmit_dataA1(ROWS[5]->Y); //Port A
		transmit_data(ROWS[5]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW6:
		transmit_dataA1(ROWS[6]->Y); //Port A
		transmit_data(ROWS[6]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		case ROW7:
		transmit_dataA1(ROWS[7]->Y); //Port A
		transmit_data(ROWS[7]->X, 2); //Port B
		transmit_dataA2(Write7Seg(LIVES)); //Port B
		break;
		default:
		break;
	}
	transmit_data(0x00, 1); //Port A
	transmit_data(0xFF, 2); //Port B
	return state;
}

//controls the PADDLE logic
int SMTick2(int state) {

	switch (state)
	{
		case -1:
			state = INIT;
			break;
		case INIT:
			if(begin == 0 ) state = INIT;
			else if (begin == 1 ) state = WAIT;
			break;
		case WAIT:
			if(begin == 0) state = INIT;
			else if(  Controller_Keys & NONE) state = WAIT;
			else if(  Controller_Keys & SNES_RIGHT) state = PRESS_INC;
			else if( Controller_Keys & SNES_LEFT) state = PRESS_DEC;
			break;
		case PRESS_INC:
			if(begin == 0) state = INIT;
			if( Controller_Keys & SNES_RIGHT) state = PRESS_INC;
			else state = DEPRESS_INC;
			break;
		case PRESS_DEC:
			if(begin == 0) state = INIT;
			if( Controller_Keys & SNES_LEFT) state = PRESS_DEC;
			else state = DEPRESS_DEC;
			break;
		case DEPRESS_INC:
			state = WAIT;
			break;
		case DEPRESS_DEC:
			state = WAIT;
			break;
		default:
			state = -1;
			break;
	}
	switch (state) {
		case INIT:
			PADDLE_SET(PADDLE1, ROWS);
			MIDDLE = 3;
			break;
		case WAIT:
			break;
		case PRESS_DEC:
			//goes left
			if( !GetBit(PAD1.Y,0)) {
				ROW_0.Y >>= 1;
				MIDDLE--;
			}
			break;
		case PRESS_INC:
			//goes right
			if(!GetBit(PAD1.Y,7)) {
				ROW_0.Y <<= 1;
				MIDDLE++;
			}
			break;
		case DEPRESS_DEC:
			break;
		case DEPRESS_INC:
			break;
		default:
			break;
	}
	return state;
}

//controls the BALL logic
int SMTick3(int state) {
	switch (state)
	{
		case -1:
			state = STEADY;
			break;
		case STEADY:
			if(begin == 0) state = STEADY;
			else if (begin == 1 ) state = START;
			break;
		case START:
			if(begin == 0 ) state = STEADY;
			if( Controller_Keys & SNES_A) state = MOVE_BALL;
			else state = START;
			break;
		case MOVE_BALL:
			if(begin == 0 ) state = STEADY;
			else if(BLOCKS(ROWS) == 1) state = GWON;
			else if(ball.GAMEOVER == 1 && LIVES > 0) state = RESTORE;
			else if(ball.GAMEOVER == 1 && LIVES <= 0) state = GMOV;
			else state = MOVE_BALL;
			break;
		case GMOV:
			if(begin == 0 ) state = STEADY;
			state = STEADY;
			break;
		case GWON:
			if(begin == 0) state = STEADY;
			state = STEADY;
			break;
		case RESTORE:
			if(begin == 0) state = STEADY;
			state = START;
			break; 
		default:
			state = -1;
			break;
	}
	switch (state) {
	case STEADY:
		MIDDLE = 3;
		BALL_SET(BALL1,ROWS);
		break;
	case START:
		if(Controller_Keys & SNES_LEFT || Controller_Keys & SNES_RIGHT ){
			ROWS[ball.bit_location_x]->Y = SetBit(ROWS[ball.bit_location_x]->Y, ball.Bit_Location_y, 0);
			ball.Bit_Location_y = MIDDLE;
			ROWS[ball.bit_location_x]->Y = SetBit(ROWS[ball.bit_location_x]->Y, ball.Bit_Location_y, 1);
			ball.UP_DOWN = 1;
		}
		break;
	case MOVE_BALL:
			POSITION(BALL1,ROWS);
			if(ball.GAMEOVER == 0){
				//sets the current y bit off
				ROWS[ball.bit_location_x]->Y = SetBit(ROWS[ball.bit_location_x]->Y, ball.Bit_Location_y, 0);
				
				
				//set the x-Y bit on
				ROWS[ball.bit_location_x + ball.UP_DOWN]->X = SetBit(ROWS[ball.bit_location_x + ball.UP_DOWN]->X, ball.bit_location_x + ball.UP_DOWN, 0 );
				ROWS[ball.bit_location_x + ball.UP_DOWN]->Y = SetBit(ROWS[ball.bit_location_x + ball.UP_DOWN]->Y, ball.Bit_Location_y + ball.LEFT_RIGHT, 1);
				
				ball.bit_location_x += ball.UP_DOWN;
				ball.Bit_Location_y += ball.LEFT_RIGHT;
			}
			break;
		
		case GMOV:
			LOST = 1;
			break;
		case GWON:
			WIN = 1;
			break;
		case RESTORE:
			LIVES--;
			RESUME(ROWS, BALL1, PADDLE1);
			break;
		default:
			break;
	}
	return state;
}

const char* welcome =  "Welcome....  to    Breakout";
const char* score = "SCORE IS: ";
unsigned char press = 0;

//game states
int SMTick4(int state)
{
	switch(state)
	{
		case -1:
			state = BEGAN;
			break;
		case BEGAN:
			if( Controller_Keys & SNES_SELECT )
			{
				OFF(ROWS);
				state = -1;
			}
			else if(Controller_Keys & SNES_START)
			{
				Row_init(ROWS);
				
				begin = 1;
				LIVES = 5;
				BLOCK_NUM = 24;
				ball.GAMEOVER = 0;
				state = GAME_START;
			}
			else state = Began2;
			break;
		case Began2:
			if( Controller_Keys & SNES_SELECT )
			{
				OFF(ROWS);
				LOST = 0;
				state = -1;
			}
			else if(Controller_Keys & SNES_START)
			{
				Row_init(ROWS);
				
				begin = 1;
				LIVES = 5;
				BLOCK_NUM = 24;
				ball.GAMEOVER = 0;
				state = GAME_START;
			}
			else if(press == 0) state = BEGAN;
			break;
		case GAME_START:
			if( Controller_Keys & SNES_SELECT )
			{
				OFF(ROWS);
				state = -1;
			}
			if( LOST == 1 || (Controller_Keys & SNES_R)) state = LOOSE;
			else if(WIN == 1 || (Controller_Keys & SNES_L)) state = WON;
			break;
		case LOOSE:
			if( Controller_Keys & SNES_SELECT )
			{
				OFF(ROWS);
				LOST = 0;
				state = -1;
			}
			else{
				set_PWM(349.23);
				state = LOOSE;
			}				
			
			break;
		case WON:
			if( Controller_Keys & SNES_SELECT )
			{
				OFF(ROWS);
				state = -1;
			}
			else{
				set_PWM(493.88);
				state = WON;
			}				
							
			break;
		default:
			state = -1;
			break;
	}
	
	switch(state)
	{
		case BEGAN:
			OFF(ROWS);
			press = 0;
			LEVEL = 1;
			begin = 0;
			LIVES = 0;
			SCORE = 0;
			ball.GAMEOVER = 1;
			WIN = 0;
			LOST = 0;
			set_PWM(0);
			//LCD_ClearScreen();
			/*LCD_DisplayString(1, welcome);*/
			transmit_dataA2(Write7Seg(0));
			delay_ms(300);
			break;
		case Began2:
			OFF(ROWS);
			begin = 0;
			LIVES = 0;
			ball.GAMEOVER = 1;
			WIN = 0;
			LOST = 0;
			

			
			//LCD_ClearScreen();
			//LCD_DisplayString(1, str);
			transmit_dataA2(Write7Seg(0));
			delay_ms(300);
			break;
		case GAME_START:	
			break;		
		case LOOSE:
			begin = 0;
			LOST = 0;
			GameOver(ROWS);
			break;
		case WON:
			begin = 0;
			WIN = 0;
			GAMEWON(ROWS);
			break;
		default:
			break;
	}
	
	return state;
}


int SMTick5(int state)
{
	switch(state)
	{
		case -1:
			state = OUT;
			break;
		case OUT:
			state = OUT;
			break;
		default:
			state = -1;
			break;
	}
	
	switch(state)
	{
		case OUT:
			break;
	}
	return state;
}