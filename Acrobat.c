/*

USB_Interrupt.c
Team 14 - Lab 6
Wireless Communications

*/ 
#include "m_general.h"
#include "m_usb.h"
#include "m_bus.h"
#include "m_rf.h"

//#define CHIP 1


#ifndef CHIPA
volatile bool connected = false;
volatile bool buffer_full = false;
volatile bool first = true;
volatile bool current_adc = false;

volatile int p_f = 20;
volatile int i_f = 1;
volatile int d_f = 32;

volatile int p_b = 7;
volatile int i_b = 1;
volatile int d_b = -8; //-5

volatile int equil = 0;
volatile int equil_acc = 0;
volatile int theta = 10;
volatile int theta_dot = 11;
volatile int PWM = 12;

volatile int sumErr = 0;
volatile int lastErr = 0;

volatile int cycle = 0;

volatile bool reset = false;


// function prototypes
void init(void);
void calibrate(void);
void drive_motors(void);
void get_analog_values(void);
void forward(void);
void backward(void);
void set_duty(unsigned int);




int main(void)
{
    init();
    while(1)
    {
		
		calibrate();


		while(!reset)
		{
			get_analog_values();
			drive_motors();
		}
		reset = false;
    }
	

}

void init(void)
{
	m_red(ON);
	m_clockdivide(0);
	m_usb_init();
	
	set(DDRD, 7);
	set(PORTD, 7);

	set(DDRF, 7);
	set(PORTF, 7);


	
    //PWM INITIALIZATION

	clear(TCCR3B,CS32);                                                //sets clock source to 16MHz/8
    set(TCCR3B,CS31);                                                //sets clock source to 16MHz/8
	set(TCCR3B,CS30);                                                //sets clock source to 16MHz/8
    

    clear(TCCR3B,WGM33);                                                //sets timer mode to: up to ICR3, PWM mode
    set(TCCR3B,WGM32);                                                //sets timer mode to: up to ICR3, PWM mode
    clear(TCCR3A,WGM31);                                                //sets timer mode to: up to ICR3, PWM mode
	set(TCCR3A,WGM30);                                            //sets timer mode to: up to ICR3, PWM mode

	sei();
    set(TIMSK3, TOIE3);
	set(TIMSK3, OCIE3A);
	set(DDRD, 0);

	set(TCCR3A,COM3A1);                                                //sets output mode to: set at OCR3A, clear at rollover (255)   
    clear(TCCR3A,COM3A0);                                                //sets output mode to: set at OCR3A, clear at rollover (255)
    
	set_duty(0);
	

	m_disableJTAG();


	// ADC

	// Set to external AREF
	clear(ADMUX, REFS1);
	set(ADMUX, REFS0);

	// Set ADC prescaler
	set(ADCSRA, ADPS2);
	set(ADCSRA, ADPS1);
	set(ADCSRA, ADPS0);
	
	set(DIDR0, ADC4D);
	set(DIDR0, ADC5D);
	

	m_disableJTAG();  

	clear(ADCSRA, ADATE);
	clear(ADCSRA, ADEN);


    
	// enables the system
	set(ADCSRA, ADEN);

	
	m_red(OFF);


}



ISR(TIMER3_OVF_vect)
{
	m_green(ON);
	set(PORTD, 0);
}

ISR(TIMER3_COMPA_vect)
{
	m_green(OFF);
	clear(PORTD, 0);
}


void calibrate(void)
{
	m_red(ON);
	m_wait(2000);
	m_wait(2000);

	// MUX selection
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// starts the conversion
	set(ADCSRA, ADSC);

	while(!check(ADCSRA, ADIF));


	equil = ADC;




	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// starts the conversion
	set(ADCSRA, ADSC);

	while(!check(ADCSRA, ADIF));
	equil_acc = ADC;


	m_red(ON);
	m_wait(50);
	m_red(OFF);
	m_wait(50);
	m_red(ON);
	m_wait(50);
	m_red(OFF);
	m_wait(50);
	m_red(ON);
	m_wait(50);
	m_red(OFF);
	m_wait(50);


}
	
void get_analog_values()
{
	
	// MUX selection
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// starts the conversion
	set(ADCSRA, ADSC);

	while(!check(ADCSRA, ADIF));

	theta = ADC;
	theta += 32; //HARDLY FALLS BACKWARDS AT 33 (32 is much more evenly spread balace-wise)
	// MUX selection
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	set(ADMUX, MUX0);

	// starts the conversion
	set(ADCSRA, ADSC);

	while(!check(ADCSRA, ADIF));
	theta_dot = ADC;

}


/*NUMBER SETS WORTH RETURNING TO: theta =+32,bufferF = 7,bufferB = 19,p=1,i=0,d=0;
*/


void drive_motors(void)
{
	int bufferF = 12;	//DEFINITELY FALLS FORWARD AT 25 - MUCH LESS SO AT 18
	int bufferB = 12;  	//HARDLY FALLS BACKWARDS AT 25 WITH THETA =+ 32; WHEN bufferF DECREASED, bufferB LESS EFFECTIVE

	int p = 0;
	int i = 0;
	int d = 0;

	if(theta<equil-bufferF){
		forward();
		//theta=+32,bufferF=4,bufferB=10,p=20,i=0,d=2,DIV=16 was decent
		p = p_f;
		i = i_f;
		d = d_f;
	}
	
	if(theta>equil+bufferB)
	{
		backward();
		p = p_b;
		i = i_b;
		d = d_b;

	}

	
	

	int DIV = 1;

	int PID = p * (equil-theta) + i*(sumErr + equil - theta) + d*((equil-theta) - lastErr);
//	int PID = p * (equil-theta) + i*(sumErr + equil - theta) + d*(theta_dot- equil_acc);

	if(cycle % 1000 == 0)
	{
		m_usb_tx_int((theta - equil)-lastErr);
		m_usb_tx_string("Theta dot\n");
		
	}


	if( PID < 0)
		PID *= -1;
	
	if(PID > 0xFF)
			PID = 0xff;

	if(theta<equil-bufferF){
		
	}
	else
	{
		if(theta>equil+bufferB)
		{


		}
		else
		{
			set_duty(0);
		}
	}

	cycle++;

/*	if(theta<equil-bufferF){
		forward();
		if(PID > 0)
			PID = 2550;
		else
			PID -= 2*PID;

	}
	else
	{
		if(theta>equil+bufferB)
		{
			backward();
			if(PID < 0)
				PID = 2550;
		}
		else
		{
			set_duty(0);
		}
	}
*/
	set_duty(PID/DIV);	

	lastErr = equil-theta;

	sumErr +=lastErr;
	
	

  	/*int bufferF = 50;
	int bufferB = 10;                      
    //float degSec = (theta_dot*3.0/1023) / ( 0.0037);
    //in acceptable range
    if(theta >= equil-bufferF && theta <= equil+bufferB)
    {
        PWM = 0;
        clear(DDRC,6);
		set_duty(0);
    }
    //forward direction    
    if(theta<equil-bufferF)
    {
		forward();
        PWM = (p * theta) + (d * theta_dot);
        set_duty();
        set(PORTB,2);
        set(DDRC,6);
    }
    //backward direction less sensitive
    if(theta>equil+bufferB)
    {
		backward();
        PWM = (p * theta) + (d * theta_dot);
        set_duty();
        clear(PORTB,2);
        set(DDRC,6);
    }
      */
}

void forward(void)
{
	set(PORTF, 7);
	set(PORTD, 7);
}

void backward(void)
{
	clear(PORTF, 7);
	clear(PORTD, 7);
}



void set_duty(unsigned int duty)
{

	if(cycle % 1000 == 0)
	{
		m_usb_tx_int(duty);
		m_usb_tx_string("PID\n");
		
	}
	OCR3A = 5 * duty / 2 + 55;
	if(duty < 10)
		OCR3A = 0;
	if(duty > 100)
		OCR3A = 0xff;
/*	if((duty + 80)> 255)
		OCR3A = 0xFF;
	else
		OCR3A = duty + 80;
	if(duty < 5)
		OCR3A = 0;?*/
}

#endif

