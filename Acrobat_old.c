//this is the server side com

//first position is request type
// 0 = PID with positions 1, 2, 3 corresponding  to P,I,D, respectively
// 1 = set PID with positions 1,2,3,4,5,6 corresponding to pid;
// 2 = request gyro and accel input
// 3 = receive gyro and accel and pwm input

#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"

#define CHANNEL 1
#define RXADDRESS 0x28
#define TXADDRESS 0x28
#define PACKET_LENGTH 7
#define BLINK(sec) {m_red(ON); m_wait(100); m_red(OFF); m_wait(100);}
#define WAIT(sec) for(int waiti = 0; waiti<sec; waiti++){ m_wait(1000);}

volatile bool connected = false;
volatile bool buffer_full = false;

volatile int p;
volatile int i;
volatile int d;
volatile int angle;
volatile int velo;
volatile int PWM;

char in_buffer[PACKET_LENGTH] = {97,98,99,100,101,102,103};


volatile int current_adc = 0;


// function prototypes
void init(void);
bool checkBuffer();
void send(char a, int b, int c, int d);
void read_PID();
void read_analog();
void sync_PID();
void sync_analog();
void request_angles();

/*int main()
{	
	init();
	request_angles();
	while(1)
	{
		if(checkBuffer())
		{
			switch(in_buffer[0])
			{
				case 0:
					read_PID();
					sync_PID();
					break;
				case 2:
					read_analog();
					sync_analog();
					break;
				default:
					break;
			}
			request_angles();
		}

	
	}

	return 0;

}
*/

bool checkBuffer()
{
	if(buffer_full)
	{
		buffer_full = false;
		return true;
	}
	return false;
}

void init(void)
{
	m_red(ON);
	m_clockdivide(0);
	m_bus_init();
	m_usb_init();
	m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH);
	WAIT(5); // wait for user to pull up log for connection	
	
	send(0,0,0,0); //USB and wireless initialized
	
	while(!m_usb_isconnected());

	m_red(OFF);
	
	//m_usb_tx_string("Waiting for robot to respond...");
	send(0,0,0,1); //Waiting for robot to respond
	while(!connected)
	{
		
		m_rf_send(TXADDRESS, in_buffer, PACKET_LENGTH);
		BLINK(100);
	}
	//m_usb_tx_string("Robot responded. \nRequsting PID variables.\n");	
	send(0,1,0,2); // Robot responded. Requesting PID variables
	BLINK(500);BLINK(500);

	// Set up ADC for PID

	// Set to external AREF
	clear(ADMUX, REFS1);
	set(ADMUX, REFS0);

	// Set ADC prescaler
	set(ADCSRA, ADPS2);
	set(ADCSRA, ADPS1);
	set(ADCSRA, ADPS0);
	
	set(DIDR0, ADC4D);
	set(DIDR0, ADC5D);
	set(DIDR0, ADC6D);
	
	sei();
	set(ADCSRA, ADIE);


	clear(ADCSRA, ADATE);

	clear(ADCSRA, ADEN);
	
	// MUX selection
	clear(ADCSRB, MUX5);
	set(ADMUX, MUX2);
	clear(ADMUX, MUX1);
	clear(ADMUX, MUX0);

	// enables the system
	set(ADCSRA, ADEN);

	// starts the conversion
	set(ADCSRA, ADSC);
	


	m_red(OFF);
	m_green(OFF);
	
	
}
	

ISR(INT2_vect)
{
	m_rf_read(in_buffer, PACKET_LENGTH);
	if(!connected)
	{
		connected = true;
	}
	else
	{
		buffer_full = true;	
	}
}

ISR(ADC_vect)
{
	int temp_p = p;
	int temp_i = i;
	int temp_d = d;
	switch(current_adc)
	{
		case 0:
			current_adc = 1;
			temp_p = ADC;
			clear(ADCSRA, ADEN);
			clear(ADCSRB, MUX5);
			set(ADMUX, MUX2);
			clear(ADMUX, MUX1);
			set(ADMUX, MUX0);
			break;
		case 1:
			current_adc = 2;
			temp_i = ADC;
			clear(ADCSRA, ADEN);
			clear(ADCSRB, MUX5);
			set(ADMUX, MUX2);
			set(ADMUX, MUX1);
			clear(ADMUX, MUX0);
			break;
		default:
			current_adc = 0;
			temp_d = ADC;
			clear(ADCSRA, ADEN);
			clear(ADCSRB, MUX5);
			set(ADMUX, MUX2);
			clear(ADMUX, MUX1);
			clear(ADMUX, MUX0);
			break;
	}

//	send_pid(temp_p, temp_i, temp_d);

	set(ADCSRA, ADEN);
	set(ADCSRA, ADSC);

	set(ADCSRA, ADIF);

}


void send(char a, int b, int c, int d)
{
	m_usb_tx_char(a);
	m_usb_tx_uint(b);
	m_usb_tx_uint(c);
	m_usb_tx_uint(d);
	
}


void read_PID()
{
	int* inp = (int*)&in_buffer[1];
	int* ini = (int*)&in_buffer[3];
	int* ind = (int*)&in_buffer[5];

	p = *inp;
	i = *ini;
	d = *ind;

}

void read_analog()
{
	int* ina = (int*)&in_buffer[1];
	int* inv = (int*)&in_buffer[3];
	int* inp = (int*)&in_buffer[5];

	angle = *ina;
	velo = * inv;
	PWM = * inp;

}

void sync_PID()
{
	send('p', p, i, d);
}

void sync_analog()
{
	send('a', angle, velo, PWM);
}

void request_angles()
{
	char t[7] = {0x02, 0, 0, 0, 0, 0, 0};
	m_rf_send(TXADDRESS, t, PACKET_LENGTH);
}
