#ifndef __SERVO_H_
#define __SERVO_H_

#define SERVO_DUTY_LEFT     9.5
#define SERVO_DUTY_CENTER   10.5
#define SERVO_DUTY_RIGHT    11.5

void set_servo_duty(double dutyCycle);
double getServoDuty(void);
void init_servo(void);

#endif /* SERVO_H_ */
