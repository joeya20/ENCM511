
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"
#include "ChangeClk.h"
#include "string.h"

extern uint8_t PBFlag;

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
    
    /* COnfigure CN IO interrupt */
    CNEN1bits.CN0IE = 1;    //enable CN interrupt for CN0/PB1
    CNEN1bits.CN1IE = 1;    //enable CN interrupt for CN1/PB2
    CNEN2bits.CN30IE = 1;   //enable CN interrupt for CN30/PB3
    
    IPC4bits.CNIP = 0b111;  //set CN interrupt priority
    IFS1bits.CNIF = 0;      // clear CN interrupt flag
    
    IEC1bits.CNIE = 1;      //enable CN interrupt
    
    PrintToScreen("\rNothing Pressed        ");
    return;
}
void PrintToScreen(char* MessageToPrint)
{
    NewClk(8);  //setting clk to 8 MHz so printing delay doesn't affect timer
    
    uint8_t i = 0;
    while( i < strlen(MessageToPrint)) 
    {
        XmitUART2(MessageToPrint[i], 1);    //printing string to terminal one char at a time
        i = i + 1;
    }
    
    NewClk(32); //changing clk back to 32KHz for better efficiency
    return;
}
void IOCheck(void)
{
    IEC1bits.CNIE = 0;  //turn off CN interrupt
    
    while(PORTBbits.RB4 == 0 || PORTAbits.RA4 == 0 || PORTAbits.RA2 == 0)   //if any button is pushed
    {
        if(PORTBbits.RB4 && PORTAbits.RA4 && PORTAbits.RA2 == 0)        //if only PB1 is pushed
        {
            PrintToScreen("\rPB1 is pressed         "); //print msg to screen
            LATBbits.LATB8 = ~LATBbits.LATB8; //toggle LED
            Delay_ms((uint16_t) 500); //delay approx 0.5s
        }
        else if(PORTBbits.RB4 && PORTAbits.RA4 == 0 && PORTAbits.RA2)    //if only PB2 is pushed
        {
            PrintToScreen("\rPB2 is pressed         ");   //print msg to screen
            LATBbits.LATB8 = ~LATBbits.LATB8;           //toggle LED
            Delay_ms((uint16_t) 2000);                  //delay approx 2s
        }
        else if(PORTBbits.RB4 == 0 && PORTAbits.RA4 && PORTAbits.RA2)    //if only PB3 is pushed
        {
            PrintToScreen("\rPB3 is pressed         ");   //print msg to screen
            LATBbits.LATB8 = ~LATBbits.LATB8;           //toggle LED
            Delay_ms((uint16_t) 3000);                  //delay approx 3s
        }
        else if(PORTBbits.RB4 && PORTAbits.RA4  == 0 && PORTAbits.RA2 == 0)     //if PB1 and PB2 are pushed
        {
            LATBbits.LATB8 = 1;                         //turn on LED
            PrintToScreen("\rPB1 and PB2 are pressed");   //print msg to screen
        }
        else if(PORTBbits.RB4  == 0 && PORTAbits.RA4 && PORTAbits.RA2 == 0)     //if PB1 and PB3 are pushed
        {
            LATBbits.LATB8 = 1;                         //turn on LED
            PrintToScreen("\rPB1 and PB3 are pressed");   //print msg to screen
        }
        else if(PORTBbits.RB4  == 0 && PORTAbits.RA4  == 0 && PORTAbits.RA2)    //if PB2 and PB3 are pushed
        {
            LATBbits.LATB8 = 1;                         //turn on LED
            PrintToScreen("\rPB2 and PB3 are pressed");   //print msg to screen
        }
        else                                                                    //if all PBs are pushed
        {
            LATBbits.LATB8 = 1;                         //turn on LED   
            PrintToScreen("\rAll PBs pressed        ");   //print msg to terminal
        }
    }
    
    LATBbits.LATB8 = 0;                             //LED off
    PrintToScreen("\rNothing pressed        ");   //print msg to screen
    
    PBFlag = 0;         //clear global flag
    IEC1bits.CNIE = 1;  //turn on CN interrupt
    
    return;
}
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    if(IFS1bits.CNIF == 1)
    {
        Delay_ms(30); //debounce button for 30 ms - if any button is pressed then IO bit should still be 0 after 30 ms
        
        if( PORTBbits.RB4 == 0 || PORTAbits.RA4 == 0 || PORTAbits.RA2 == 0) //if the button is still pressed after debounce period
        {
            PBFlag = 1;     //set global flag
        }
    }
    
    IFS1bits.CNIF = 0; // clear IF flag
    Nop();
}