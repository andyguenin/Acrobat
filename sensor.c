#include "m_general.h"

void sensor_init(void)
{
	// Set the reference voltage to 3.3 V
	clear(ADMUX, REFS1);
	clear(ADMUX, REFS0);


	// Set the ADC clock to divid the system clock by 128
	set(ADCSRA, ADPS2);
	set(ADCSRA, ADPS1);
	set(ADCSRA, ADPS0);


	// Disables digital input
	set(DIDR0, ADC4D);
	set(DIDR0, ADC5D);

	// prevent triggerig
	clear(ADCSRA, ADATE);

}

void get_ADC_values(int* accel, int* gyro)
{
	// reads from ADC4 (pin f4)
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// starts the ADC system
	set(ADCSRA, ADEN);
	//Starts the conversion
	set(ADCSRA, ADSC);
	// Waits for the conversion to complete
	while(!check(ADCSRA, ADIF));
	*accel = ADC;
	// Resets the conversion flag
	set(ADCSRA, ADIF);
	

	// reads from ADC4 (pin f4)
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// starts the ADC system
	set(ADCSRA, ADEN);
	//Starts the conversion
	set(ADCSRA, ADSC);
	// Waits for the conversion to complete
	while(!check(ADCSRA, ADIF));
	*gyro = ADC;
	// Resets the conversion flag
	set(ADCSRA, ADIF);
	

		

}
	
