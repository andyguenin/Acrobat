#include "m_general.h"

void forward(void)
{
	clear(PORTC, 6);
	clear(PORTC, 7);

}
void backward(void)
{
	set(PORTC, 6);
	set(PORTC, 7);
}

void set_speed(int d)
{
	// 0 -> 9825
	// 100 -> 65535	
	int s = d;
	if(d < 0)
	{
		s = -d;
		backward();
	}
	else
	{
		forward();
	}
	OCR1A = s * 577 + 9825;	
	if(!s)
		OCR1A = 0;
}

void movement_init(void)
{
	// Set the GPIO
	set(DDRC, 6);
	set(DDRC, 7);


	// Clock Source
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	set(TCCR1B, CS10);

	// Sets the timer mode to go up to OCR1A, PWM mode
	ICR1 = 0xffff;

	set(TCCR1B, WGM13);
	set(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	clear(TCCR1A, WGM10);

	// Sets behavior of B5
	set(TCCR1A, COM1A1);
	clear(TCCR1A, COM1A0);
	OCR1A = 0;
	set(DDRB, 5);

	// Sets behavior of B6
	clear(TCCR1A, COM1B1);
	clear(TCCR1A, COM1B0);

	// Sets behavior of B7
	clear(TCCR1A, COM1C1);
	clear(TCCR1A, COM1C0);


}
