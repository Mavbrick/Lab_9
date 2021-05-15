/* Author: Maverick Bautista
 * Lab Section: 023
 * Assignment: Lab 9  Exercise 3
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://youtu.be/Z9JWtuGPR7Y 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn () {
	
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
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs = 0x00;	//tmp var for 3 leds
unsigned char blinkingLED = 0x00;	//tmp var for blinking led
unsigned char speaker = 0x00;	//tmp var for speaker to activate

unsigned short LED_Time = 300;
unsigned short Blink_Time = 1000;
unsigned short Speaker_Time = 2;

enum SM1_States { Initial1, One, Two, Three } SM1_State; //3 leds

void ThreeLEDsSM() {
	switch(SM1_State) {
		case Initial1:
			SM1_State = One;
		break;

		case One:
			SM1_State = Two;
		break;

		case Two:
			SM1_State = Three;
		break;

		case Three:
			SM1_State = One;
		break;

		default:
			SM1_State = One;
		break;

	}

	switch(SM1_State) {
		case Initial1:
		break;

		case One:
		threeLEDs = 0x01;
		break;

		case Two:
		threeLEDs = 0x02;
		break;

		case Three:
		threeLEDs = 0x04;
		break;

		default:
		break;
	}
}

enum SM2_States { Initial2, On, Off } SM2_State;	//blinking

void BlinkingLEDSM() {
	switch(SM2_State) {
		case Initial2:
			SM2_State = On;
		break;

		case On:
			SM2_State = Off;
		break;

		case Off:
			SM2_State = On;
		break;

		default:
			SM2_State = Off;
		break;
	}

	switch(SM2_State) {
		case Initial2:
		break;

		case On:
		blinkingLED = 0x08;
		break;

		case Off:
		blinkingLED = 0x00;
		break;

		default:
		break;
	}
}

enum SM3_States { Initial3, Wait, Sound, Done } SM3_State;

void SpeakerSM() {
	unsigned char but = ~PINA & 0x04;

	switch(SM3_State) {
		case Initial3:
			SM3_State = Wait;
		break;

		case Wait:
			if(but) {
				SM3_State = Sound;
			}
			else {
				SM3_State = Wait;
			}
		break;

		case Sound:
			if(but) {
				SM3_State = Done;
			}
			else {
				SM3_State = Wait;
			}
		break;

		case Done:
			if(but) {
				SM3_State = Sound;
			}
			else {
				SM3_State = Wait;
			}
		break;

		default:
			SM3_State = Wait;
		break;
	}

	switch(SM3_State) {
		case Initial3:
		break;

		case Sound:
			speaker = 0x10;
		break;

		default:
			speaker = 0x00;
		break;
	}
}

void CombineLEDsSM() {
	PORTB = speaker | threeLEDs | blinkingLED;	
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x10;
    /* Insert your solution below */
	TimerSet(1);
	TimerOn();

	SM1_State = Initial1;
	SM2_State = Initial2;
	SM3_State = Initial3;
    while (1) {
	if(LED_Time >= 300) { //checks for 300 ms
	    ThreeLEDsSM();
	    LED_Time = 0;
	}
	else {
		LED_Time += 1;
	}
	
	if(Blink_Time >= 1000) { //checks for 1000 ms aka 1 sec
	    BlinkingLEDSM();
	    Blink_Time = 0;
	}
	else {
		Blink_Time += 1;
	}
	if(Speaker_Time >= 2) { //checks for 2 ms
		SpeakerSM();
		Speaker_Time = 0;
	}
	else {
		Speaker_Time +=1;
	}

	    CombineLEDsSM();
		
	    while (!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}


