#include <stdio.h>
#include <stdbool.h>

#include "MK64F12.h"
#include "util.h"
#include "uart.h"
#include "motor.h"
#include "servo.h"
#include "camera.h"
#include "PID.h"
#include "test.h"

#define PRINTF(A, ...) char str[100];sprintf(str,A, ##__VA_ARGS__);uart_put(str);

#define SERVO_TEST
#define SERVO_LEFT 4
#define SERVO_CENTER 7
#define SERVO_RIGHT 8

void TEST_servo(void){
	char str[128];
	PRINTF("Servo test mode\n\r");
	float doody=0;
	for(;;){
		switch(uart_getchar()){
			case '2':
				if(doody>0){doody -= 0.1;}
				set_servo_duty(doody);
				break;
			case '3':
				if(doody<100){doody += 0.1;}
				set_servo_duty(doody);
				break;
			case '5':
				if(doody>0){doody -= 1;}
				set_servo_duty(doody);
				break;
			case '6':
				if(doody<100){doody += 1;}
				set_servo_duty(doody);
				break;
			case '8':
				if(doody>0){doody -= 10;}
				set_servo_duty(doody);
				break;
			case '9':
				if(doody<100){doody += 10;}
				set_servo_duty(doody);
				break;
			case 's':
			case 'S':
				PRINTF("Servo to the right ....\n\r");
				for(doody=SERVO_LEFT; doody<=SERVO_RIGHT;doody +=0.1){
					set_servo_duty(doody);
					delay(5);
				}
				PRINTF("Servo to the left ....\n\r");
				for(doody=SERVO_RIGHT; doody>=SERVO_LEFT;doody -=0.1){
					set_servo_duty(doody);
					delay(5);
				}
				break;
		}
		PRINTF("Servo Doody cycle: %f\r\n",doody);
		
	}
}
void TEST_camera(void){}
void TEST_motor(void){
char str[128];
	PRINTF("Motor test mode\n\r");
	float doody=0;
	for(;;){
		switch(uart_getchar()){
			case '2':
				if(doody>0){doody -= 0.1;}
				set_motor_duty(doody);
				break;
			case '3':
				if(doody<100){doody += 0.1;}
				set_motor_duty(doody);
				break;
			case '5':
				if(doody>0){doody -= 1;}
				set_motor_duty(doody);
				break;
			case '6':
				if(doody<100){doody += 1;}
				set_motor_duty(doody);
				break;
			case '8':
				if(doody>0){doody -= 10;}
				set_motor_duty(doody);
				break;
			case '9':
				if(doody<100){doody += 10;}
				set_motor_duty(doody);
				break;
		}
		PRINTF("Motor Doody cycle: %f\r\n",doody);
	}
}