#include "m_general.h"
#include "movement.h"
#include "sensor.h"

#define WAIT(n) {for(int wait_n = 0; wait_n < n; wait_n++){ m_wait(1000); } }

int gyro, gyro_init;
int accel, accel_init;
float sumErr;

//int pidc_f[] = {12,2,10,10};
//int pidc_b[] = {7,2,7,10};

int pidc_f[] = {12,5,11,10};
int pidc_b[] = {12,5,11,10};

//int b_front = 10;
//int b_back = 25;

int b_front = 10;
int b_back = 10;
void init(void);

int main()
{
	
	init();
	
	WAIT(3);
	get_ADC_values(&accel_init, &gyro_init);
	//accel_init -= 10;
	m_red(ON);

	while(1)
	{
		get_ADC_values(&accel, &gyro);
		int gyro_diff = gyro - gyro_init;
		int accel_diff = accel - accel_init;
	//	if((sumErr + accel_diff >= sumErr && accel_diff >= 0) || (sumErr + accel_diff <= sumErr && accel_diff <= 0))
	//		sumErr += accel_diff;
		sumErr -= (accel_diff)/1000.0;

		int p,i,d, c;

		if(accel<accel_init)
		{
			p = -pidc_f[0];
			i = pidc_f[1];
			d = pidc_f[2];
			c = pidc_f[3];
		}
		else
		{
			p = -pidc_b[0];
			i = pidc_b[1];
			d = pidc_b[2];
			c = pidc_b[3];
		}

		if(accel < accel_init + b_back && accel > accel_init - b_front)
		{
			p = 0;
		}

		int control = p * accel_diff + i * ((int)(sumErr)) + d * gyro_diff;
		set_speed(control/c);
	}
}





void init()
{
	m_clockdivide(0);
	m_disableJTAG();

	movement_init();
	sensor_init();
}
