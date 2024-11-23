/* 
 * File: light_control.h
 * Author: Dylan Wenaas, David Anez, Issy Gaudet
 */

#ifndef LIGHT_CONTROL_H
#define	LIGHT_CONTROL_H

#include <xc.h> 
#include "UART2.h"

void handlePWM();

extern uint8_t PB1_click;
extern uint8_t PB2_click;
extern uint8_t PB3_click;
extern uint8_t tmr2_event;
extern uint8_t tmr3_event;
extern uint8_t timeOld;

#define PB1 PORTAbits.RA2
#define PB2 PORTAbits.RA4
#define PB3 PORTBbits.RB4

#define LED_BIT LATBbits.LATB8

extern uint8_t mode;

// State definitions
#define ON       (1 << 0)  // 0b001
#define BLINK    (1 << 1)  // 0b010 
#define TRANSMIT (1 << 2)  // 0b100
#define LED      (1 << 3)  // 0b1000

// State check macros
#define isON           ((mode & ON) != 0)
#define isOFF          (!isON)
#define isBLINK        ((mode & BLINK) != 0)
#define isNotBLINK     (!isBLINK)
#define isTRANSMIT     ((mode & TRANSMIT) != 0)
#define isNotTRANSMIT  (!isTRANSMIT)
#define isLED          ((mode & LED) != 0)

// State control macros
#define setON()        (mode |= ON)
#define setOFF()       (mode &= ~ON)

#define setBLINK()     (mode |= BLINK)
#define clearBLINK()   (mode &= ~BLINK)
#define toggleBLINK()  (mode ^= BLINK)

#define setTRANSMIT()    (mode |= TRANSMIT)
#define clearTRANSMIT()  (mode &= ~TRANSMIT)
#define toggleTRANSMIT() (mode ^= TRANSMIT)

#define setLED()       (mode |= LED)
#define clearLED()     (mode &= ~LED)
#define toggleLED()    (mode ^= LED)

#define timerEvent     (timeOld != tmr3_event)
#endif	/* LIGHT_CONTROL_H */

