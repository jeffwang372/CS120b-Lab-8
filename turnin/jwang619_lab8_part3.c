/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab #8  Exercise #3
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1rHk6A1ASs4QnYnQwboXO2bSTqwIRtMwB/view?usp=sharing
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

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

enum SMStates {SMStart, Init, WaitPlay, WaitNext, PlayNote, WaitTime} SMState;
enum SMStates2 {SMStart2, Init2, WaitOn, IsOn, Check} SMState2;

unsigned char iArray = 0x00;
unsigned char iTimer = 0x00;
double notes[6] = {392.00, 329.63, 293.66, 349.23, 523.25, 440.00};
int playTimes[6] = {10, 8, 15, 5, 8, 15};
int waitTimes[6] = {1, 1, 2, 1, 3, 5};
unsigned char IsFinished = 0x00;

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
			iArray = 0x00;
			iTimer = 0x00;
			SMState = WaitPlay;
			break;
		case WaitPlay:
			if((inputA & 0x01) == 0x01) {
                                SMState = PlayNote;
                        }
			break;
		case WaitNext:
			if(IsFinished == 0x01) {
				IsFinished = 0x00;
				SMState = WaitPlay;
			}
			break;
		case PlayNote:
			if(iArray == 0x06) {
				SMState = WaitNext;
				iArray = 0x00;
                       		iTimer = 0x00;
				break;
			}
			if(!(iTimer < playTimes[iArray])) {
				iTimer = 0;
				SMState = WaitTime;;
			}
			else {
				++iTimer;
			}
			break;
		case WaitTime:		
                        if(!(iTimer < waitTimes[iArray])) {
                                iTimer = 0;
                                SMState = PlayNote;
                                ++iArray;
                        }
                        else {
                                ++iTimer;
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
			break;
                case WaitPlay:
                        break;
		case WaitNext:
			break;
		case PlayNote:
			set_PWM(notes[iArray]);
			break;
		case WaitTime:
                        set_PWM(0);
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
                                SMState2 = IsOn;
                        }
                        break;
                case IsOn:
                        if((inputA & 0x01) != 0x01) {
                                SMState2 = Check;
                        }
                        break;
		case Check:
			if(iArray == 0x00) {
                                SMState2 = WaitOn;
				IsFinished = 0x01;
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
                case IsOn:
                        break;
		case Check:
			break;
		default:
			break;
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
        DDRB = 0xFF; PORTB = 0x00;
	TimerSet(100);
	TimerOn();
    /* Insert your solution below */
	SMState = SMStart;
	SMState2 = SMStart2;
	PWM_on();
    while (1) {
	SMFunc();
	SMFunc2();
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
