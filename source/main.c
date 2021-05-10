/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab #8  Exercise #2
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1rE9BU4ipBRMW9YbkHVYlIZEz-anAscxm/view?usp=sharing
 */

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
	static double current_frequency;
	if(frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }

		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum SMStates {SMStart, Init, WaitButton, Inc, Dec, WaitIncR, WaitDecR} SMState;
enum SMStates2 {SMStart2, Init2, WaitOn, OnBuffer, WaitOff, OffBuffer} SMState2;

unsigned char i = 0x00;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};

void SMFunc() {
	unsigned char inputA = 0x00;
//        unsigned char outputB = 0x00;

 //       outputB = PORTB;
        inputA = ~PINA;
	switch(SMState) {
		case SMStart:
			SMState = Init;
			break;
		case Init:
			SMState = WaitButton;
			break;
		case WaitButton:
			if((inputA & 0x02) == 0x02) {
                                SMState = Dec;
                        }
			if((inputA & 0x04) == 0x04) {
                                SMState = Inc;
                        }
			break;
		case Inc:
			SMState = WaitIncR;
			break;
		case Dec:		
                        SMState = WaitDecR;
			break;
		case WaitIncR:
			if((inputA & 0x04) != 0x04) {
                                SMState = WaitButton;
                        }
			break;
		case WaitDecR:
			if((inputA & 0x02) != 0x02) {
                                SMState = WaitButton;
                        }
			break;
		default:
			SMState = SMStart;
			break;
	}
	switch(SMState) {
                case SMStart:
                        break;
		case Init:
			i = 0x00;
			set_PWM(notes[i]);
			break;
                case WaitButton:
                        break;
		case Inc:
			if(i != 0x07) {
				++i;
				set_PWM(notes[i]);
			}
			break;
		case Dec:
                        if(i != 0x00) {
                                --i;
                                set_PWM(notes[i]);
                        }
                        break;
                case WaitIncR:
                        break;
                case WaitDecR:
                        break;
                default:
                        break;
        }
}


void SMFunc2() {
        unsigned char inputA = 0x00;
//        unsigned char outputB = 0x00;

 //       outputB = PORTB;
        inputA = ~PINA;
        switch(SMState2) {
                case SMStart2:
                        SMState2 = Init2;
                        break;
                case Init2:
                        SMState2 = WaitOn;
                        break;
                case WaitOn:
                        if((inputA & 0x01) == 0x01) {
                                SMState2 = OnBuffer;
                        }
                        break;
                case OnBuffer:
                        if((inputA & 0x01) != 0x01) {
                                SMState2 = WaitOff;
                        }
                        break;
		case WaitOff:
			if((inputA & 0x01) == 0x01) {
                                SMState2 = OffBuffer;
                        }
			break;
                case OffBuffer:
                        if((inputA & 0x01) != 0x01) {
                                SMState2 = WaitOn;
                        }
                        break;
		default:
			SMState2 = SMStart2;
			break;
	}
	switch(SMState2) {
                case SMStart2:
                        break;
                case Init2:
                        break;
                case WaitOn:
                        break;
                case OnBuffer:
                        PWM_on();
                        set_PWM(notes[i]);
                        break;
		case WaitOff:
			break;
                case OffBuffer:
                        PWM_off();
                        break;
		default:
			break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
        DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
	SMState = SMStart;
	SMState2 = SMStart2;
    while (1) {
	SMFunc();
	SMFunc2();
    }
    return 0;
}
