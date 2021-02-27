

#include "IOs.h"

void IO_init(void)
{   
    /* sets RB8 as output and RA4 as input */
    TRISBbits.TRISB8 = 0;
    TRISAbits.TRISA4 = 1;
    
    /* enables pull up resistor for RA4 */
    CNPU1bits.CN0PUE = 1;
    
    return;
}

void IO_check(void)
{
    while(PORTAbits.RA4 == 0)
        {
            LATBbits.LATB8 = 1;
        }            
        LATBbits.LATB8 = 0;
        
    return;
}