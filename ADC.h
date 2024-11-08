/* 
 * File: ADC.h
 * Author: Dylan Wenaas, David Anez, Issy Gaudet
 */

#ifndef ADC_H
#define	ADC_H

#include <xc.h> 
#include "UART2.h"

extern uint8_t PB1_event;
extern uint8_t tmr3_event;
extern uint8_t mode;

#define PB1 PORTAbits.RA2

uint16_t do_ADC(void);

void IOinit();
void IOcheck();

        
#endif	/* ADC_H */

