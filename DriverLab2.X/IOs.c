#include "xc.h"
#include "IOs.h"

void IOinit(void)
{
    AD1PCFG = 0xFFFF; // Turn all analog pins as digital  
    /* outputs */
    TRISBbits.TRISB8 = 0; // Set pin RB8 as output
    /* inputs */
    TRISBbits.TRISB4 = 1;   // Set pin RB4 as input
    TRISAbits.TRISA2 = 1;   // Set pin RA2 as input
    TRISAbits.TRISA4 = 1;   // Set pin RA4 as input
    CNPU1bits.CN0PUE = 1;   //enables pull up resistor for RA4
    CNPU1bits.CN1PUE = 1;   //enables pull up resistor for RB4
    CNPU2bits.CN30PUE = 1;  //enables pull up resistor for RA2
    
    return;
}

void delay_s(uint8_t delayDuration)
{
    uint32_t counter = 0;   
    uint32_t delayCycles = delayDuration * 70000;
    
    while(counter < delayCycles) 
    {
        if( PORTAbits.RA2 == 0 ||   // if button is still pressed, increment counter to continue delay
            PORTAbits.RA4 == 0 ||
            PORTBbits.RB4 == 0 )
        {
            counter++;
        }
        else                        // button was released, end counter
        {
            break;
        }
    }
    
    return;
}

void IOcheck(void)
{
    while(PORTAbits.RA2 == 0)   // while PB1 is pushed
    {   
        if( PORTAbits.RA4 == 0 || PORTBbits.RB4 == 0 )  //if any other PB is pushed
        {
            LATBbits.LATB8 = 1; //turn on LED
        }
        else
        {
            LATBbits.LATB8 = ~LATBbits.LATB8;   //toggle LED
            delay_s(1); //delay approx 1s
        }
    }
    while(PORTAbits.RA4 == 0)   // while PB2 is pushed
    {   
        if( PORTAbits.RA2 == 0 || PORTBbits.RB4 == 0 )  //if any other PB is pushed
        {
            LATBbits.LATB8 = 1; //turn on LED
        }
        else
        {
            LATBbits.LATB8 = ~LATBbits.LATB8;   //toggle LED
            delay_s(2); //delay approx 2s
        }
    }
    while(PORTBbits.RB4 == 0)   // while PB3 is pushed
    {   
        if( PORTAbits.RA4 == 0 || PORTAbits.RA2 == 0 )  //if any other PB is pushed
        {
            LATBbits.LATB8 = 1; //turn on LED
        }
        else
        {
            LATBbits.LATB8 = ~LATBbits.LATB8;   //toggle LED
            delay_s(3); //delay approx 3s
        }
    }
    
    LATBbits.LATB8 = 0;
    
    return;
}