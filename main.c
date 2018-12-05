/*
 * Main method for testing the PWM DC motor control and servo control.
 * PWM signal connected to output pins PC3 and PC4
 * Servo connected to pin PB18
 *
 * Author: Unknown
 * Modified by: John Cowan <jrc9071@rit.edu>
 *              Gabriel Smith <gas6030@rit.edu>
 * Created: Unknown
 * Modified: 10/20/17
 */

#include <stdio.h>
#include <stdbool.h>

#include "MK64F12.h"
#include "util.h"
#include "uart.h"
#include "motor.h"
#include "servo.h"
#include "camera.h"
#include "PID.h"

#define DEBUG 1
#define DEBUG_CAMERA 0


//Global Variables
char str[128];


#define LEFT 1
#define RIGHT 0

//Defines
//Motor speeds
#define STRAIGHT_LEFT 40
#define STRAIGHT_RIGHT STRAIGHT_LEFT - 2
#define LEFT_OUTSIDE_TURN 30
#define RIGHT_OUTSIDE_TURN LEFT_OUTSIDE_TURN - 2
#define LEFT_INSIDE_TURN 20
#define RIGHT_INSIDE_TURN LEFT_INSIDE_TURN - 2
#define DIFF_TURN 10

//Camera sensitivity and track parsing
#define CAMERA_LEFT_INDEX 125
#define CAMERA_RIGHT_INDEX 0
#define CAMERA_THRESHOLD 60000
#define MIN_CONSECUTIVE_DELTAS 1
#define DELTA_VALUE 13000
#define EDGE_DELTA (DELTA_VALUE*DELTA_VALUE)
#define MIN_EDGE_DISTANCE 17

//Camera index values for centering car
//When going straigt (stay in middle)
#define IDEAL_CENTER 70
#define WORST_RIGHT 90
#define WORST_LEFT 50

//Servo values
#define SERVO_LEFT 8
#define SERVO_CENTER 10.4
#define SERVO_RIGHT 12
#define SHARP_TURN ((1.1)*(1.1))


void initialize(void);

int main(void) {
	int track_left;			//Index of left edge
	int found_left;
	int track_right;		//Index of right edge
	int found_right;
	int track_center;		//Calculated index of center
	double turn_error;
	
	int i;
	uint16_t* camData;
	int cam, cam_p, count;
    int edge_counter;
    double pid_slope;
    PID servoPID;
    PID speedPID;
    bool is_turning;
	bool turning_left;
    char str[100];

    // Initialize UART and PWM
    initialize();

	//Initialize servo and motor positions
	double servo_doody = SERVO_CENTER; 
	
	set_motor_duty(0, LEFT);
	set_motor_duty(0, RIGHT);
	set_servo_duty(SERVO_CENTER);
	
	while(!SW3_IN) {}
    while(1) {
		GPIOB_PCOR |= (1 << 22);
		cam=0;
		cam_p=0;
		count = 0;
		found_left = 0;
		found_right = 0;
		
		// send the array over uart
		/* */ //CAMERA data parsing
		if(camReadDone){
			camReadDone = 0;
			camData = CAMERA_getLine();
			cam_p = camData[CAMERA_LEFT_INDEX + 1];
			edge_counter = 0;
			#if DEBUG_CAMERA
				D_PRINTF("|");
			#endif
			for (i = CAMERA_LEFT_INDEX; i > CAMERA_RIGHT_INDEX; i--) {
				cam = camData[i];
				#if DEBUG_CAMERA
					if(cam < CAMERA_THRESHOLD){
						D_PRINTF("#");
					}
					else{D_PRINTF("-");}
				#endif
				
				count += ((cam-cam_p)*(cam-cam_p) >= EDGE_DELTA);
				count *= ((cam-cam_p)*(cam-cam_p) >= EDGE_DELTA);
				
				
				// FINDING LEFT
				if(!found_left){
					if(count >= MIN_CONSECUTIVE_DELTAS){
						track_left=i;
						#if DEBUG_CAMERA
							D_PRINTF("L");
						#endif
						found_left=1;
					}
				}
				// FINDING RIGHT
				else if(!found_right && edge_counter > MIN_EDGE_DISTANCE){
					if(count >= MIN_CONSECUTIVE_DELTAS){
						track_right=i;
						#if DEBUG_CAMERA
							D_PRINTF("R");
						#endif								
						found_right=1;
					}
				}
				else if(!found_right){
					edge_counter++;
				}
				cam_p = cam;
			}
			track_center = ((track_left - track_right) / 2) + track_right;
			// */ //CAMERA data parsing
			#if DEBUG_CAMERA
				D_PRINTF("|\n\r");
			#else
				D_PRINTF("Track Center: %d\n\r", track_center);
			#endif
			
			
			
			/* */ //SERVO steering
			
			if(track_center < WORST_LEFT){
				servo_doody=SERVO_RIGHT;
			}
			else if(track_center <= WORST_RIGHT){
				pid_slope = ((float) (SERVO_RIGHT - SERVO_LEFT)/ (float) (WORST_LEFT - WORST_RIGHT));
				servo_doody = (pid_slope * track_center) + ( SERVO_LEFT - (pid_slope * WORST_RIGHT));
			}
			else{
				servo_doody=SERVO_LEFT;
			}
			set_servo_duty(servo_doody);
			
			/* */ //DIFFERENTIAL turning
			turn_error = (SERVO_CENTER - servo_doody);
			if(turn_error*turn_error > SHARP_TURN){
				if(turn_error < 0){                     //RIGHT TURN (Left wheel on outside should go faster)
					set_motor_duty(LEFT_OUTSIDE_TURN, LEFT);
					set_motor_duty(RIGHT_INSIDE_TURN, RIGHT);
					//MOTOR_SetDutyCycle(OUTSIDE_TURN, DIFF_2);
					//MOTOR_SetDutyCycle(INSIDE_TURN, DIFF_1);
				}
				else{                                   //LEFT TURN (Right wheel on outside should go faster)
					set_motor_duty(LEFT_INSIDE_TURN, LEFT);
					set_motor_duty(RIGHT_OUTSIDE_TURN, RIGHT);
					//MOTOR_SetDutyCycle(OUTSIDE_TURN, DIFF_1);
					//MOTOR_SetDutyCycle(INSIDE_TURN, DIFF_2);
				}
			}
			else{
				set_motor_duty(STRAIGHT_LEFT, LEFT);
				set_motor_duty(STRAIGHT_RIGHT, RIGHT);
			}
		}
	}
}
void initialize(void) {
    // Initialize UART
    init_uart();
    init_LEDs();
    init_switches();
    init_motor();
    init_servo();
    init_camera();
}
