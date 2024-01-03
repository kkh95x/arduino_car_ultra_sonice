#include <avr/io.h>
#define F_CPU 16000000UL

#include "util/delay.h"


uint8_t speed25 = 63;
uint8_t speed50 = 127;
uint8_t speed75 = 180;
uint8_t speed100 = 255;

void init() {
	DDRB=0xff;
	PORTB=0xff;
	DDRD |= (1 << PORTD0) | (1 << PORTD1) | (1 << PORTD2) | (1 << PORTD3);
	//For ULtraSonic pins
	DDRD |= (1 << PORTD4); // Set trigger pin (PORTD4) as output
	DDRD &= ~(1 << PORTD5); // Set echo pin (PORTD5) as input
	     //0baabb--ww
	TCCR1A=0b10100001;
	      //0b00-wwccc
	TCCR1B=	0b00001010;
	//WGM: 0101 --> Fast PWM, 8-bit  ;TOP=0x00FF ;Update of OCR1x at BOTTOM; TOV1 Flag Set on TOP
	//CS: 010 --> 16 / 8 (From prescaler) ???? ????? ?????? 16/8=2
	//COM1A: 10
	//COM1B: 10
	//--> Clear OC1A/OC1B on Compare Match,
	//--> Set OC1A/OC1B at BOTTOM (non-inv.)
	
	// Set initial state move forward with speed 100
	PORTD &=~(1<<PORTD7);
	PORTD &= ~(1 << PORTD0);
	PORTD |= (1 << PORTD1);
	PORTD &= ~(1 << PORTD2);
	PORTD |= (1 << PORTD3);
	OCR1A = speed100;
	OCR1B = speed100;
	
	
}

void setMotorSpeed(uint8_t speed) {
	OCR1A = speed;
	OCR1B = speed;
}

int measureDistance() {
	PORTD |= (1 << PORTD4);  // Set trigger pin high
	_delay_us(10);           // Wait for 10 microseconds
	PORTD &= ~(1 << PORTD4); // Set trigger pin low

	uint16_t timeout = 60000;
	while (!(PIND & (1 << PORTD5)) && timeout > 0) {
		_delay_us(1);
		timeout--;
	}
	if (timeout == 0) {
		return 9999;
	}

	uint16_t startTime = 0;
	timeout = 60000;
	while (PIND & (1 << PORTD5) && timeout > 0) {
		_delay_us(1);
		startTime++;
		timeout--;
	}
	if (timeout == 0) {
		return 9999;
	}

	return startTime * 0.01715;
}

int main() {
	init();
	
	
	while (1) {
		// Move forward
		
		int distance = measureDistance();
		
		if (distance < 10) {
			// Stop the car
			PORTD|=(1<<PORTD7);
			PORTD &= ~(1 << PORTD0);
			PORTD &= ~(1 << PORTD1);
			PORTD &= ~(1 << PORTD2);
			PORTD &= ~(1 << PORTD3);
			_delay_ms(1000);
			//Back
			OCR1A = speed75;  // Adjust the value as needed
			OCR1B = speed75;  // Adjust the value as needed
			PORTD |= (1 << PORTD0);
			PORTD &= ~(1 << PORTD1);
			PORTD |= (1 << PORTD2);
			PORTD &= ~(1 << PORTD3);
			_delay_ms(1500);
			OCR1A = speed100;  // Adjust the value as needed
			OCR1B = speed100;  // Adjust the value as needed
			//// Stop the car
			PORTD &= ~(1 << PORTD0);
			PORTD &= ~(1 << PORTD1);
			PORTD &= ~(1 << PORTD2);
			PORTD &= ~(1 << PORTD3);
			//// Left
			_delay_ms(1500);
			
			PORTD |= (1 << PORTD0);
			PORTD &= ~(1 << PORTD1);
			PORTD &= ~(1 << PORTD2);
			PORTD |= (1 << PORTD3);
			
			_delay_ms(1000);
			
			//// Stop the car
			PORTD &= ~(1 << PORTD0);
			PORTD &= ~(1 << PORTD1);
			PORTD &= ~(1 << PORTD2);
			PORTD &= ~(1 << PORTD3);
			_delay_ms(1000);
			
			} else {
			// Move the car forward
			PORTD&=~(1<<PORTD7);
			PORTD &= ~(1 << PORTD0);
			PORTD |= (1 << PORTD1);
			PORTD &= ~(1 << PORTD2);
			PORTD |= (1 << PORTD3);
		}
	}

	return 0;
}
