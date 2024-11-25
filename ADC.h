/* 
 * File: ADC.h
 * Author: Dylan Wenaas, David Anez, Issy Gaudet
 */

#ifndef ADC_H
#define	ADC_H

uint16_t do_ADC(void);
void init_ADC();
void sendADCReading(uint16_t reading, uint8_t LED_ON);

#endif	/* ADC_H */

