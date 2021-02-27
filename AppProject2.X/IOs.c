
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"
#include "ChangeClk.h"
#include "string.h"
#include "ADC.h"
#include "stdio.h"
extern uint8_t state;
const char Omega = 234;

void IOinit(void)
{
    AD1PCFG = 0xF7DF; // AN5 and AN11 - Analog; Others - Digital
    
    TRISBbits.TRISB4 = 1;   // Set pin RB4 as input
    TRISAbits.TRISA2 = 1;   // Set pin RA2 as input
    TRISAbits.TRISA4 = 1;   // Set pin RA4 as input
    CNPU1bits.CN0PUE = 1;   //enables pull up resistor for RA4
    CNPU1bits.CN1PUE = 1;   //enables pull up resistor for RB4
    CNPU2bits.CN30PUE = 1;  //enables pull up resistor for RA2
    
    /* COnfigure CN IO interrupt */
    CNEN1bits.CN0IE = 1;    //enable CN interrupt for CN0/PB1
    CNEN1bits.CN1IE = 1;    //enable CN interrupt for CN1/PB2
    CNEN2bits.CN30IE = 1;   //enable CN interrupt for CN30/PB3
    
    IPC4bits.CNIP = 0b100;  //set CN interrupt priority
    IFS1bits.CNIF = 0;      // clear CN interrupt flag
    
    IEC1bits.CNIE = 1;      //enable CN interrupt
    
    return;
}

void IOCheck(void)
{
    float valueToDisplay;
    char str[10];
    
    while(state == 1 || state == 2)
    {
        float ADCValue = do_ADC(state) / 1023.0;     //get adc value and divide by 1023.0 to get decimal value
        
        if(state == 1)  //if in voltmeter mode
        {
            valueToDisplay = ADCValue * 3.25;   // Voltage = (ADC / 1023) * VDD
            sprintf(str, "%1.3f", (double)valueToDisplay); // Converts -15.567 stored in f into an array of characters
            NewClk(8);                              //change clk for smooth printing
            Disp2String("VOLTMETER Voltage = ");    //print text to screen
            Disp2String(str);                       //print value to screen
            Disp2String(" V   \r");                   //print unit and return cursor
            NewClk(32);                             //change clk back for power efficiency
        }
        else        //if in ohmmeter mode
        {
            valueToDisplay = ADCValue * 1000 / (1 - ADCValue);  // R_DUT = R_protection * (ADC/1023) / (1 - (ADC / 1023))
            NewClk(8);                              //change clk for smooth printing
            Disp2String("OHMMETER Resistance = ");  //print text to screen
            Disp2Dec(valueToDisplay);               //print value to screen as integer
            XmitUART2(Omega, 1);                    //print unit
            XmitUART2('\r', 1);                     //return cursor
            NewClk(32);                             //change clk back for power efficiency
        }
    }
    
    return;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IEC1bits.CNIE = 0;  //disable all interrupts
    delay_ms(40);       //debounce PBs for 40ms
    IEC1bits.CNIE = 1;  //enable all interrupts 
    
    if(PORTAbits.RA2 == 0)  //if PB1 is pressed
    {
        state = 1;          //go to state 1: voltmeter mode
    }
    else if(PORTAbits.RA4 == 0) //if PB2 is pressed
    {
        state = 2;          //go to state 2: ohmmeter mode
    }
    else if(PORTBbits.RB4 == 0) //if PB3 is pressed
    {
        state = 3;          //go to state 3: Idle
    }
    
    IFS1bits.CNIF = 0; // clear IF flag
    
    return;
}