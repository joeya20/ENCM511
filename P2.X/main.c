/*
 * File:   main.c
 * Author: JoeyA
 *
 * Created on September 16, 2020, 7:17 PM
 */
#include "xc.h"

int main(void) {
    
    TRISAbits.TRISA0 = 1;   //sets RA0-GPIO2 as input
    TRISAbits.TRISA1 = 1;   //sets RA1-GPIO3 as input
    TRISBbits.TRISB0 = 0;   //sets RB0-GPIO4 as output
    TRISBbits.TRISB1 = 0;   //sets RB1-GPIO5 as output
    
    /* Super loop */
    while(1)
    {
//        if ((PORTAbits.RA0 == 0) && (PORTAbits.RA1 == 1))
//        {
//            LATBbits.LATB0 = 1; //sets RB0 high
//            LATBbits.LATB1 = 0; //sets RB1 low
//        }
//        else if ((PORTAbits.RA0 == 1) && (PORTAbits.RA1 == 0))
//        {
//            LATBbits.LATB0 = 0; //sets RB0 low
//            LATBbits.LATB1 = 1; //sets RB1 high
//        }
//        else
//        {
//            LATBbits.LATB0 = 0; //sets RB0 low
//            LATBbits.LATB1 = 0; //sets RB1 low
//        }
        if (PORTAbits.RA0 != PORTAbits.RA1)
        {
            LATBbits.LATB0 = !PORTAbits.RA0;
            LATBbits.LATB1 = !PORTAbits.RA1;
        }
        else
        {
            LATBbits.LATB0 = 0;
            LATBbits.LATB1 = 0;
        }
    }
    return 0;
}
