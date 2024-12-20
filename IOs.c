/*
 * File:   IOs.c
 * Author: Dylan Wenaas, Isabelle Gaudet, David Anez
 *
 * Created on November 20, 2024, 3:41 PM
 */


#include "xc.h"
#include "IOs.h"
#include "ADC.h"
#include "UART2.h"
#include "clkChange.h"

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

  IPC4bits.CNIP = 6;
  IFS1bits.CNIF = 0;
  IEC1bits.CNIE = 1;

  TMR3 = 0;
  PR3 = 1600; // ~100ms

}

void handlePWM() {

  uint16_t reading = do_ADC();
  float intensity = (float)do_ADC() / 1023.0;

  // PR2 = 5000 gives a period of ~10ms
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
int IOcheck() {
  switch (mode) {
  case OFF:
    // LED is completely off, waiting for button press to change state
    if (PB1_click) {
      // Transition to ON state and clear PB1_click 
      timeOld = tmr3_event;
      PB1_click = 0;
      PB3_click = 0; // Clear PB3_click in case it was set but not processed as it should do nothing in this state
      mode = ON;
      isLED = 1;
      return 1;
    }
    else if (PB2_click) {
      // Transition to OFF state and clear PB2_click
      PB2_click = 0;
      PB3_click = 0; // Clear PB3_click in case it was set but not processed as it should do nothing in this state
      mode = OFF_BLINK;
      return 1;
    }
    timeOld = tmr3_event;
    isLED = 0;
    LED_BIT = 0;
    T2CONbits.TON = 0;
    T3CONbits.TON = 0;
    break;
  case OFF_BLINK:
    // LED blinks at fixed rate while system is off
    if (PB1_click) {
      // Transition to ON state and clear PB1_click
      timeOld = tmr3_event;
      PB1_click = 0;
      PB3_click = 0; // Clear PB3_click in case it was set but not processed as it should do nothing in this state
      mode = ON;
      isLED = 1;
      return 1;
    }
    else if (PB2_click) {
      // Transition to OFF state and clear PB2_click
      PB2_click = 0;
      PB3_click = 0; // Clear PB3_click in case it was set but not processed as it should do nothing in this state
      mode = OFF;
      return 1;
    }
    if (timerEvent && tmr3_event % 5 == 0) {
      // Toggle LED every 5 timer events (~500ms)
      tmr3_event = 0;
      isLED = !isLED;
    }
    timeOld = tmr3_event;
    LED_BIT = isLED;
    T2CONbits.TON = 0;
    T3CONbits.TON = 1;
    break;
  case ON:
    // LED is on with PWM intensity based on ADC reading
    if (PB1_click) {
      // Transition to OFF state and clear PB1_click
      PB1_click = 0;
      timeOld = tmr3_event;
      mode = OFF;
      return 1;
    }
    else if (PB2_click) {
      // Transition to ON_BLINK state and clear PB2_click
      PB2_click = 0;
      mode = ON_BLINK;
      return 1;
    }
    else if (PB3_click) {
      // Transition to ON_TRANSMIT state and clear PB3_click
      PB3_click = 0;
      mode = ON_TRANSMIT;
      return 1;
    }
    T2CONbits.TON = 1;
    T3CONbits.TON = 0;

    if (tmr2_event) {
      // Every ~10ms the pwm should be called 
      handlePWM();
    }
    break;
  case ON_BLINK:
    // LED blinks with PWM intensity while system is on
    if (PB1_click) {
      // Transition to OFF state and clear PB1_click
      PB1_click = 0;
      timeOld = tmr3_event;
      mode = OFF;
      return 1;
    }
    else if (PB2_click) {
      // Transition to ON state and clear PB2_click
      PB2_click = 0;
      mode = ON;
      return 1;
    }
    else if (PB3_click) {
      // Transition to ON_BLINK_TRANSMIT state and clear PB3_click
      PB3_click = 0;
      mode = ON_BLINK_TRANSMIT;
      return 1;
    }
    if (timerEvent && tmr3_event % 5 == 0) {
      // Toggle LED every 5 timer events (~500ms)
      tmr3_event = 0;
      isLED = !isLED;
    }
    timeOld = tmr3_event;

    T2CONbits.TON = 1;
    T3CONbits.TON = 1;

    if (tmr2_event && isLED) {
      // Every ~10ms the pwm should be called 
      handlePWM();
    }
    LED_BIT = LED_BIT && isLED;
    break;
  case ON_TRANSMIT:
    // Transmits ADC readings while LED is on with PWM
    if (PB1_click) {
      // Transition to OFF state and clear PB1_click
      PB1_click = 0;
      timeOld = tmr3_event;
      mode = OFF;
      return 1;
    }
    else if (PB2_click) {
      // Transition to ON_BLINK_TRANSMIT state and clear PB2_click
      PB2_click = 0;
      mode = ON_BLINK_TRANSMIT;
      return 1;
    }
    else if (PB3_click) {
      // Transition to ONT state and clear PB3_click
      PB3_click = 0;
      mode = ON;
      return 1;
    }
    if (timerEvent && tmr3_event % 3 == 0) {
      // Every ~300ms send an ADC and Intensity reading via UART
      sendADCReading(do_ADC(), isLED);
    }
    isLED = 1;
    timeOld = tmr3_event;

    T2CONbits.TON = 1;
    T3CONbits.TON = 1;

    if (tmr2_event) {
      // Every ~10ms the pwm should be called 
      handlePWM();
    }
    break;
  case ON_BLINK_TRANSMIT:
    // Transmits ADC readings while LED blinks with PWM
    if (PB1_click) {
      // Transition to OFF state and clear PB1_click
      PB1_click = 0;
      timeOld = tmr3_event;
      mode = OFF;
      return 1;
    }
    else if (PB2_click) {
      // Transition to ON_TRANSMIT state and clear PB2_click
      PB2_click = 0;
      mode = ON_TRANSMIT;
      return 1;
    }
    else if (PB3_click) {
      // Transition to ON_BLINK state and clear PB3_click
      PB3_click = 0;
      mode = ON_BLINK;
      return 1;
    }
    if (timerEvent) {
      if (tmr3_event % 5 == 0) {
        // Toggle LED every 5 timer events (~500ms)
        isLED = !isLED;
        tmr3_event = 0;
      }
      if (tmr3_event % 3 == 0) {
        // Every ~300ms send an ADC and Intensity reading via UART
        sendADCReading(do_ADC(), isLED);
      }
    }
    timeOld = tmr3_event;

    T2CONbits.TON = 1;
    T3CONbits.TON = 1;

    if (tmr2_event && isLED) {
      // Every ~10ms the pwm should be called
      handlePWM();
    }
    LED_BIT = LED_BIT && isLED;

    break;
  }
  return 0;
}