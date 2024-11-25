/*
 * File:   main.c
 * Author: Dylan Wenaas, Issy Gaudet, David Anez
 *
 * Created on November 8, 2024, 12:09 PM
 */

// FBS
#pragma config BWRP = OFF               // Table Write Protect Boot (Boot segment may be written)
#pragma config BSS = OFF                // Boot segment Protect (No boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Segment Code Flash Write Protection bit (General segment may be written)
#pragma config GCP = OFF                // General Segment Code Flash Code Protection bit (No protection)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC oscillator (FRC))
#pragma config IESO = OFF               // Internal External Switch Over bit (Internal External Switchover mode disabled (Two-Speed Start-up disabled))

// FOSC
#pragma config POSCMOD = NONE           // Primary Oscillator Configuration bits (Primary oscillator disabled)
#pragma config OSCIOFNC = ON            // CLKO Enable Configuration bit (CLKO output disabled; pin functions as port I/O)
#pragma config POSCFREQ = HS            // Primary Oscillator Frequency Range Configuration bits (Primary oscillator/external clock input frequency greater than 8 MHz)
#pragma config SOSCSEL = SOSCHP         // SOSC Power Selection Configuration bits (Secondary oscillator configured for high-power operation)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor Selection (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscale Select bits (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (WDT prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Windowed Watchdog Timer Disable bit (Standard WDT selected; windowed WDT disabled)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))

// FPOR
#pragma config BOREN = BOR3             // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware; SBOREN bit disabled)
#pragma config PWRTEN = ON              // Power-up Timer Enable bit (PWRT enabled)
#pragma config I2C1SEL = PRI            // Alternate I2C1 Pin Mapping bit (Default location for SCL1/SDA1 pins)
#pragma config BORV = V18               // Brown-out Reset Voltage bits (Brown-out Reset set to lowest voltage (1.8V))
#pragma config MCLRE = ON               // MCLR Pin Enable bit (MCLR pin enabled; RA5 input pin disabled)

// FICD
#pragma config ICS = PGx2               // ICD Pin Placement Select bits (PGC2/PGD2 are used for programming and debugging the device)

// FDS
#pragma config DSWDTPS = DSWDTPSF       // Deep Sleep Watchdog Timer Postscale Select bits (1:2,147,483,648 (25.7 Days))
#pragma config DSWDTOSC = LPRC          // DSWDT Reference Clock Select bit (DSWDT uses LPRC as reference clock)
#pragma config RTCOSC = SOSC            // RTCC Reference Clock Select bit (RTCC uses SOSC as reference clock)
#pragma config DSBOREN = ON             // Deep Sleep Zero-Power BOR Enable bit (Deep Sleep BOR enabled in Deep Sleep)
#pragma config DSWDTEN = ON             // Deep Sleep Watchdog Timer Enable bit (DSWDT enabled)

// #pragma config statements should precede project file includes.

#include <xc.h>
#include <p24F16KA101.h>
#include "clkChange.h"
#include "UART2.h"
#include "ADC.h"
#include "IOs.h"


// global flags
uint8_t PB1_click = 0;
uint8_t PB2_click = 0;
uint8_t PB3_click = 0;

uint8_t tmr2_event = 0;
uint8_t tmr3_event = 0;
state_t mode = OFF;
uint8_t timeOld = 0;
uint8_t isLED = 0;
uint8_t ledPulse = 0;

int main(void) {
    IOinit();
    InitUART2();
    init_ADC();
    // mode = OFF;
    // timeOld = tmr3_event;
    // isLED = 0;
    // LED_BIT = 0;
    // T2CONbits.TON = 0;
    // T3CONbits.TON = 0;

    while (1) {
        // IOcheck will return 1 if a state transition occurs, 0 otherwise
        // This allows IOcheck to once again be called in the while loop
        if (IOcheck())
            continue;
        Idle();
    }
    return 0;
}

// Timer 2 interrupt subroutine
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    IFS0bits.T2IF = 0;
    tmr2_event = 1;
    // T2CONbits.TON = 0; // disable timer

}

void __attribute__((interrupt, no_auto_psv)) _T3Interrupt(void){
    //Don't forget to clear the timer 3 interrupt flag!
    IFS0bits.T3IF = 0;
    
    TMR3 = 0; // reset timer
    tmr3_event += 1;
}

uint8_t button_state = 0;
#define PB1_state   (1 << 0)  // 0b001
#define PB2_state   (1 << 1)  // 0b010 
#define PB3_state   (1 << 2)  // 0b100

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    IFS1bits.CNIF = 0;
    if (PB1 == 0 && (button_state & PB1_state) == 0) {
        button_state |= PB1_state;
    } else if (PB1 == 1 && (button_state & PB1_state) != 0) {
        button_state &= ~PB1_state;
        PB1_click = 1;
    }
    if (PB2 == 0 && (button_state & PB2_state) == 0) {
        button_state |= PB2_state;
    } else if (PB2 == 1 && (button_state & PB2_state) != 0) {
        button_state &= ~PB2_state;
        PB2_click = 1;
    }
    if (PB3 == 0 && (button_state & PB3_state) == 0) {
        button_state |= PB3_state;
    } else if (PB3 == 1 && (button_state & PB3_state) != 0) {
        button_state &= ~PB3_state;
        PB3_click = 1;
    }
}