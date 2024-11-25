/* 
 * File: IOs.h
 * Author: Dylan Wenaas, David Anez, Issy Gaudet
 */

#ifndef IOS_H
#define	IOS_H

void handlePWM();
void IOinit();
int IOcheck();

extern uint8_t PB1_click;
extern uint8_t PB2_click;
extern uint8_t PB3_click;
extern uint8_t tmr2_event;
extern uint8_t tmr3_event;
extern uint8_t timeOld;
extern uint8_t isLED;
extern uint8_t ledPulse;

#define PB1 PORTAbits.RA2
#define PB2 PORTAbits.RA4
#define PB3 PORTBbits.RB4

#define LED_BIT LATBbits.LATB8

typedef enum {
  OFF,
  OFF_BLINK,
  ON,
  ON_BLINK,
  ON_TRANSMIT,
  ON_BLINK_TRANSMIT
} state_t;

extern state_t mode;

#define timerEvent     (timeOld != tmr3_event)
#endif	/* LIGHT_CONTROL_H */

