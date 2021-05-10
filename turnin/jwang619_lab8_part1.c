/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab #8  Exercise #1
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1rA6xrCBSRh0KqALDznrwMIRWeGodYYhE/view?usp=sharing
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

enum SMStates {SMStart, Wait, Sound1, Sound2, Sound3, WaitOff} SMState;

void SMFunc() {
	unsigned char inputA = 0x00;
//        unsigned char outputB = 0x00;

 //       outputB = PORTB;
        inputA = ~PINA;
	switch(SMState) {
		case SMStart:
			SMState = Wait;
			break;
		case Wait:
			if(inputA == 0x01) {
				SMState = Sound1;
			}
			if(inputA == 0x02) {
                                SMState = Sound2;
                        }
			if(inputA == 0x04) {
                                SMState = Sound3;
                        }
			break;
		case Sound1:
			SMState = WaitOff;
			break;
		case Sound2:
                        SMState = WaitOff;
                        break;
		case Sound3:
                        SMState = WaitOff;
                        break;
		case WaitOff:
			if(inputA != 0x01 && inputA != 0x02 && inputA != 0x04) {
				SMState = Wait;
			}
			break;
		default:
			SMState = SMStart;
			break;
	}
	switch(SMState) {
                case SMStart:
                        break;
                case Wait:
			set_PWM(0);
                        break;
                case Sound1:
                        set_PWM(261.63);
                        break;
                case Sound2:
                        set_PWM(293.66);
                        break;
                case Sound3:
                        set_PWM(329.63);
                        break;
		case WaitOff:
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
	PWM_on();
	SMState = SMStart;
    while (1) {
	SMFunc();
    }
    return 0;
}
