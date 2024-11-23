/*
 * File:   ADC.c
 * Author: Dylan Wenaas, Isabelle Gaudet, David Anez
 *
 * Created on November 2, 2024, 6:26 PM
 */


#include "xc.h"
#include "ADC.h"
#include "clkChange.h"
#include "light_control.h"

uint16_t reading = 0;

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

void IOinit() {

    newClk(8); // set clock to 8 MHz

    //T3CON config
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    T3CONbits.TCKPS = 3; // set prescaler to 1:256
    T3CONbits.TCS = 0; // use internal clock
    T3CONbits.TSIDL = 0; //operate in idle mode
    IPC2bits.T3IP = 2; //7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; //enable timer interrupt

    T2CONbits.T32 = 0;      // operate timer 2 as 16 bit timer
    T2CONbits.TCKPS = 1;    // set prescaler
    T2CONbits.TCS = 0;      // use internal clock
    // T2CONbits.TGATE = 0;
    T2CONbits.TSIDL = 0;    // operate in idle mode
    // Timer 2 interrupt config
    IPC1bits.T2IP = 3; //7 is highest and 1 is lowest pri.
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1; //enable timer interrupt

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
    mode = 0;
    tmr3_event = 0;

    IPC4bits.CNIP = 6;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;

    TMR3 = 0;
    PR3 = 1600; // ~100ms

    T3CONbits.TON = 0;
    clearBLINK();
    clearLED();
    clearTRANSMIT();
    setOFF();
    mode = 0;
}

void sendADCReading(uint16_t reading) {
    Disp2Hex(reading);
    Disp2String("\r\n");
}

uint8_t ledPulse = 0;
void handlePWM() {

    float intensity = (float) do_ADC() / 1024.0;

    // PR2 = 500 gives a period of 1ms
    if (ledPulse) {
        PR2 = (uint16_t)((5000 * (1.0 - intensity)) + 1);
    }
    else {
        PR2 = (uint16_t)((5000 * intensity) + 1);
    }


    ledPulse = !ledPulse;
    LED_BIT = ledPulse && (reading > 1);

    TMR2 = 0;
    T2CONbits.TON = 1;
}

void IOcheck() {
    // state machine
    while (isOFF) {
        if (PB1_click) {
            PB1_click = 0;
            PB3_click = 0;
            setON();
            setLED();
            LED_BIT = 1;
            timeOld = tmr3_event;
            break;
        }
        else if (PB2_click) {
            PB2_click = 0;
            toggleBLINK();
        }

        if (timerEvent) {
            if (isBLINK && tmr3_event % 5 == 0) {
                tmr3_event = 0;
                toggleLED();
            }
        }
        timeOld = tmr3_event;
        LED_BIT = isLED & isBLINK;

        T2CONbits.TON = 0;
        T3CONbits.TON = isBLINK;
        Idle();
    }
    while (isON) {
        if (PB1_click) {
            PB1_click = 0;
            setOFF();
            clearBLINK();
            clearTRANSMIT();
            clearLED();
            timeOld = tmr3_event;
            break;
        }
        else if (PB2_click) {
            PB2_click = 0;
            toggleBLINK();
            if (isNotBLINK) {
                setLED();
            }
        }
        else if (PB3_click) {
            PB3_click = 0;
            toggleTRANSMIT();
        }


        if (timerEvent) {
            if (isBLINK && tmr3_event % 5 == 0) {
                tmr3_event = 0;
                toggleLED();
            }
            if (isTRANSMIT && tmr3_event % 3 == 0) {
                reading = 0;
                if (isLED) {
                    reading = do_ADC();
                }
                sendADCReading(reading);
            }
        }
        timeOld = tmr3_event;

        T2CONbits.TON = isLED || isBLINK;
        T3CONbits.TON = isBLINK || isTRANSMIT;

        if (tmr2_event && isLED) {
            handlePWM();
        }
        LED_BIT = LED_BIT && isLED;

        Idle();
    }
}