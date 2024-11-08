/*
 * File:   ADC.c
 * Author: Dylan Wenaas, Isabelle Gaudet, David Anez
 *
 * Created on November 2, 2024, 6:26 PM
 */


#include "xc.h"
#include "ADC.h"
#include "clkChange.h"

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
    init_ADC();

    AD1CON1bits.SAMP = 1;
    
    while (AD1CON1bits.DONE == 0) {}
    ADCvalue = ADC1BUF0;
    
    AD1CON1bits.SAMP = 0;
    
    return (ADCvalue);  
}

void IOinit() {
           
    newClk(500);
    
    //T3CON config
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T3CONbits.TCKPS = 3; // set prescaler to 1:256
    T3CONbits.TCS = 0; // use internal clock
    T3CONbits.TSIDL = 0; //operate in idle mode
    IPC2bits.T3IP = 2; //7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; //enable timer interrupt

    /* Let's set up some I/O */
    TRISBbits.TRISB8 = 0;
    LATBbits.LATB8 = 0;
    
    TRISAbits.TRISA4 = 1;
    CNPU1bits.CN0PUE = 1;
    CNEN1bits.CN0IE = 1;
    
    TRISBbits.TRISB4 = 1;
    CNPU1bits.CN1PUE = 1;
    CNEN1bits.CN1IE = 1;
    
    TRISAbits.TRISA2 = 1;
    CNPU2bits.CN30PUE = 1;
    CNEN2bits.CN30IE = 1;
    
    /* Let's clear some flags */
    PB1_event = 0;
    mode = 0;
    tmr3_event = 0;
    
    IPC4bits.CNIP = 6;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
    
    /* Let's set up our UART */    
    InitUART2();
    
    TMR3 = 0;
    PR3 = 980;
    T3CONbits.TON = 1;
    
}

void sendMode0(uint16_t result) {
    char bar[34] = "*";

    int num = 32 * ((double) result / 0x3ff);
    int i = 0;
    for (i = 1; i < num + 1; i++) {
        bar[i] = '*';
    }
    bar[i] = ' \0';

    Disp2String("\033[2J"); // Clears the terminal
    Disp2String("\033[H"); // Sets cursor to top left of terminal
    Disp2String("Mode 0: ");
    Disp2String(bar);
    Disp2Hex(result);
}
    
void sendMode1 (uint16_t result) {
    Disp2Hex(result);
    Disp2String("\r\n");
}


void IOcheck() {
    
    if (tmr3_event) { // Clock interrupt to run once a second
        tmr3_event = 0;

        uint16_t result = do_ADC();
        
        if (mode == 0) {
            // Mode 0 State
            sendMode0(result);
        } else {
            // Mode 1 State
            sendMode1(result);
        }
    }

    if (PB1_event) {
        mode ^= 1;
        PB1_event = 0;
    }
    Idle();

}