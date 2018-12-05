#ifndef __UTIL_H
#define __UTIL_H

#include "MK64F12.h"
#include "stdio.h"
#include "uart.h"

/*From clock setup 0 in system_MK64f12.c*/
#define DEFAULT_SYSTEM_CLOCK 20485760u /* Default System clock value */

// LED GPIO numbers.
// Port B
#define LED_RED     22
// Port E
#define LED_GREEN   26
// Port B
#define LED_BLUE    21

// Port c
#define SW2 6
// Port A
#define SW3 4

#define SW2_IN      (!(PTC->PDIR & (1 << 6)))
#define SW3_IN      (!(PTA->PDIR & (1 << 4)))
#define PRINTF(A, ...) sprintf(str,A, ##__VA_ARGS__);uart_put(str)
#define D_PRINTF(A, ...) if(DEBUG)PRINTF(A, ##__VA_ARGS__)
//#define PRINT_INIT
//#define DEBUG
// Enable streaming camera data over UART
//#define DEBUG_CAM_DATA

void delay(int millis);
void init_LEDs(void);
void LED_disable(void);

void init_switches(void);

#endif /* __UTIL_H */
