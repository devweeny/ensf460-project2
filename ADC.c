/*
 * File:   ADC.c
 * Author: Dylan Wenaas, Isabelle Gaudet, David Anez
 *
 * Created on November 2, 2024, 6:26 PM
 */


#include "xc.h"
#include "ADC.h"
#include "clkChange.h"
#include "UART2.h"

void init_ADC() {
    TRISAbits.TRISA3 = 1;

    AD1PCFG = 0xffff;
    AD1CON1bits.SSRC = 0b111;
    AD1CON1bits.FORM = 0;
    AD1CON1bits.ASAM = 0;
    AD1CON1bits.ADON = 1;
    AD1CON1bits.DONE = 1;
    AD1CON2bits.VCFG = 0b000;
    AD1CON2bits.CSCNA = 0;
    AD1CON2bits.SMPI = 0b0000;
    AD1CON2bits.BUFM = 0;
    AD1CON2bits.ALTS = 0;
    AD1CON3bits.ADRC = 0;
    AD1CON3bits.SAMC = 0b00111;
    AD1CHSbits.CH0SA = 0b0101;
    AD1CHSbits.CH0NA = 0;
    AD1PCFGbits.PCFG5 = 0;
    AD1CSSLbits.CSSL5 = 0;
}

uint16_t do_ADC(void) {
    uint16_t ADCvalue;

    AD1CON1bits.SAMP = 1;

    while (AD1CON1bits.DONE == 0) {}
    ADCvalue = ADC1BUF0;

    AD1CON1bits.SAMP = 0;

    return (ADCvalue);
}

void sendADCReading(uint16_t reading, uint8_t LED_ON) {
    Disp2Hex(reading);
    Disp2String(",");
    if (LED_ON) {
        Disp2Dec(reading / 1023.0 * 100);
    }
    else {
        Disp2Dec(0);
    }
    Disp2String("\r\n");
}
