/*
 * led_matrix.c
 *
 * Created: 02-Jun-19 11:46:23
 * Author : Boi-Hien Huang
 */ 
#ifndef LED_MATRIX_C
#define LED_MATRIX_C
#include <avr/io.h>
void transmit_data(unsigned char data, unsigned char status) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		if(status == 1) PORTA = 0x08;
		else if(status == 2) PORTB = 0x08;
		else if(status == 3) PORTC = 0x08;
		else if(status == 4) PORTD = 0x08;
		// set SER = next bit of data to be sent.
		if(status == 1) PORTA |= ((data >> i) & 0x01);
		else if(status == 2)  PORTB |= ((data >> i) & 0x01);
		else if(status == 3)  PORTC |= ((data >> i) & 0x01);
		else if(status == 4)  PORTD |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		if(status == 1) PORTA |= 0x02;
		else if(status == 2) PORTB |= 0x02;
		else if(status == 3) PORTC |= 0x02;
		else if(status == 4) PORTD |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	if(status == 1) PORTA |= 0x04;
	else if(status == 2) PORTB |= 0x04;
	else if(status == 3) PORTC |= 0x04;
	else if(status == 4) PORTD |= 0x04;
	// clears all lines in preparation of a new transmission
	if(status == 1) PORTA = 0x00;
	else if(status == 2) PORTB = 0x00;
	else if(status == 3) PORTC = 0x00;
	else if(status == 4) PORTD = 0x00;
}

void transmit_dataA1(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x08;
		// set SER = next bit of data to be sent.
		PORTA |= ((data >> i) & 0x01);
		
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x02;
		
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTA = 0x00;
	
}
void transmit_dataA2(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTA = 0x80;
		// set SER = next bit of data to be sent.
		PORTA |= ((data >> i) & 0x01)  << 4;
		
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTA |= 0x20;
		
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTA |= 0x40;
	// clears all lines in preparation of a new transmission
	PORTA = 0x00;
	
}

void transmit_dataD1(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTD = 0x08;
		// set SER = next bit of data to be sent.
		PORTD |= ((data >> i) & 0x01);
		
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTD |= 0x02;
		
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTD |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTD = 0x00;
	
}
// enum LED_STATES {start, light1, light2 }state;
// 
// void tick(){
// 	switch(state){
// 		case start:
// 			if(controllerKey & SNES_RIGHT){
// 				state = light1;
// 			}
// 			else if(controllerKey & SNES_LEFT){
// 				state = light2;
// 			}
// 			break;
// 		case light1:
// 			state = start;
// 			break;
// 		case light2:
// 			state = start;
// 			break;
// 			
// 	}
// 	switch(state){
// 		case start:
// 			PORTB = 0xF0;
// 			PORTA = 0x0F;
// 			transmit_LED_col_data(0xFF);
// 			break;
// 		case light1:
// 			PORTB = 0xFF;
// 			transmit_LED_col_data(0xFF);
// 			break;
// 		case light2:
// 			PORTA = 0x00;
// 			transmit_LED_row_data(0x00);
// 			break;
// 		
// 	}
// }

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFB; PORTC = 0x04;
    /* Replace with your application code */
    while (1) 
    {
		readSNES();
		tick();
    }
	return 0;
}
#endif