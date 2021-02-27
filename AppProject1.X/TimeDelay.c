
#include "TimeDelay.h"

void TMR2config(void)
{
    IFS0bits.T2IF = 0;          //clear Timer2 interrupt flag
    IEC0bits.T2IE = 1;          //enable interrupt for Timer2
    IPC1bits.T2IP = 0b111;      //set priority level - only one interrupt so 111 is fine
    T2CONbits.TSIDL = 0;        //continue operation in Idle mode
    T2CONbits.T32 = 0;          //use Timer2 as standalone 16 bit timer
    T2CONbits.TCS = 0;          //use internal clock
    T2CONbits.TCKPS = 0b01;     //set prescaler to 1:8 for fun/to test
}

void delay_ms(uint16_t time_ms)
{   
    /* Configure Timer2 */
    TMR2config();
    
    /* set PR2 register */
    PR2 = time_ms * 2;  // time_ms * 32000 Hz /1000 / 2 / prescaler(1:8)
    
    /* turn on Timer2 */
    T2CONbits.TON = 1;
    
    Idle();             //go to idle mode until Timer2 interrupt occurs
    
    return;
}

void __attribute__((interrupt, no_auto_psv))_T2Interrupt(void)
{
    IFS0bits.T2IF = 0;  // clear Timer2 interrupt flag
    T2CONbits.TON = 0;  // turn off Timer2
    TMR2 = 0;           // reset TMR2 register
    
    return;
}