/*
 * File:   main.c
 * Author: JoeyA
 *
 * Created on September 24, 2020, 9:33 AM
 */
#include "xc.h"

int main(void) {
    //ucid: 30067378
    
    /* setting 4 inputs */
    TRISAbits.TRISA0 = 1;   //sets RA0 as input
    TRISAbits.TRISA1 = 1;   //sets RA1 as input
    TRISAbits.TRISA4 = 1;   //sets RA4 as input
    TRISAbits.TRISA5 = 1;   //sets RA5 as input

    /* setting 8 outputs */
    TRISBbits.TRISB0 = 0;   //sets RB0 as output
    TRISBbits.TRISB1 = 0;   //sets RB1 as output
    TRISBbits.TRISB2 = 0;   //sets RB2 as output
    TRISBbits.TRISB4 = 0;   //sets RB4 as output
    TRISBbits.TRISB7 = 0;   //sets RB7 as output
    TRISBbits.TRISB8 = 0;   //sets RB8 as output
    TRISBbits.TRISB9 = 0;   //sets RB9 as output
    TRISBbits.TRISB12 = 0;  //sets RB12 as output
        
    /* start of super loop */
    while(1)
    {
        if( (PORTA == 0x0001) || (PORTA == 0x0012) ) // if input = 0001: RA0 = 1, RA1 = 0, RA4 = 0, RA5 = 0 or input = 0110: RA0 = 0, RA1 = 1, RA4 = 1, RA5 = 0
        {
            LATB = 0x0007; //turns RB0, RB0 and RB2 LEDs on
        }
        /* ignoring input = 0010 and 0011 because output is zero for these inputs and will be handled by the else branch */
        else if ( PORTA == 0x0010 ) // if input = 0100: RA0 = 0, RA1 = 0, RA4 = 1, RA5 = 0
        {
            LATB = 0x0197; //turns RB0, RB1, RB2, RB4, RB7 and RB8 LEDs on
        }
        else if ( (PORTA == 0x0011) || (PORTA == 0x0013) ) // if input = 0101: RA0 = 1, RA1 = 0, RA4 = 1, RA5 = 0 or input = 0111: RA0 = 1, RA1 = 1, RA4 = 1, RA5 = 0
        {
            LATB = 0x0397; //turns RB0, RB1, RB2, RB4, RB7, RB8 and RB9 LEDs on
        }
        else if ( PORTA == 0x0020 ) // if input = 1000: RA0 = 0, RA1 = 0, RA4 = 0, RA5 = 1
        {
            LATB = 0x1397; //turns all LEDs on
        }
        else //if user input = 0000, 0010, 0011 and 1001 - 1111
        {
            LATB = 0x0000; //turns all LEDs off
        }
    }
    /*end of super loop */
    return 0;
}
