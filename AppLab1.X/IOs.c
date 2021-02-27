
#include <p24F16KA101.h>

#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"
#include "ChangeClk.h"
#include "string.h"
#include "math.h"

uint8_t Min = 0;
uint8_t Sec = 0;
uint8_t AlarmOn = 0;
uint8_t CountdownOn = 0;

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
    
    RefreshTimer();
    
    return;
}

void PrintStringToScreen(char* MessageToPrint)
{
    NewClk(8);  //setting clk to 8 MHz so printing delay doesn't affect timer
    int i = 0;
    
    while(i < strlen(MessageToPrint))
    {
        XmitUART2(MessageToPrint[i], 1);
        i = i + 1;
    }
    
    NewClk(32); //changing clk back to 32KHz for better efficiency
    return;
}

void PrintNumToScreen(uint8_t num)
{
    uint8_t rem;  //remainder in div by 10
    uint16_t quot; 
    uint8_t ctr = 0;  //counter
    
    while(ctr<2)
    {
        quot = num/(pow(10,(1-ctr)));
        rem = quot%10;
        XmitUART2(rem + 0x30 , 1);
        ctr = ctr + 1;
    }
    
    return;
}

void RefreshTimer(void)
{
    NewClk(8);
    XmitUART2(' ', 5);
    PrintNumToScreen(Min);
    Disp2String("m : ");
    PrintNumToScreen(Sec);
    Disp2String("s        \r");
    
    NewClk(32);
    
    return;
}

void CountdownComplete(void)
{
    PrintStringToScreen("     00m : 00s --ALARM\r");
    LATBbits.LATB8 = 1;
    CountdownOn = 0;
    CNEN1bits.CN0IE = 1;    //enable CN interrupt for CN0/PB1
    CNEN1bits.CN1IE = 1;    //enable CN interrupt for CN1/PB2
    return;
}

void IOCheck(void)
{
    IEC1bits.CNIE = 0;  //disable CN interrupts to avoid debounces
    delay_ms(500);      // 500 ms delay to filter out debounces 
    IEC1bits.CNIE = 1;  //Enable CN interrupts to detect pb release
    
    while((PORTAbits.RA2 == 0) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 1) && (CountdownOn == 0))//while only PB1 is pressed
    {
        if(Min < 59)
        {
            Min = Min + 1;
            RefreshTimer();
            delay_ms(500);
        }
    }
    while((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 0) && (PORTBbits.RB4 == 1) && (CountdownOn == 0))//while only PB2 is pressed
    {
        if(Sec < 59)
        {
            Sec = Sec + 1;
            RefreshTimer();
            delay_ms(500);
        }
    }
    if((PORTAbits.RA2 == 1) && (PORTAbits.RA4 == 1) && (PORTBbits.RB4 == 0) && (CountdownOn == 0))   //while PB3 is pressed and Countdown is OFF
    {
        CountdownOn = 1;
    }
    
    if( CountdownOn == 1 )
    {
        StartCountdown();
    }
    
    return;
}

void StartCountdown(void)
{
    CNEN1bits.CN0IE = 0;    //enable CN interrupt for CN0/PB1
    CNEN1bits.CN1IE = 0;    //enable CN interrupt for CN1/PB2
    CNEN2bits.CN30IE = 0;   //enable CN interrupt for CN30/PB3
    while((Min > 0) || (Sec > 0))
    {
        while(Sec > 0)
        {
            Sec = Sec - 1;
            RefreshTimer();
            LATBbits.LATB8 = !LATBbits.LATB8;
            delay_ms(1000);
        }
        if( Min > 0 )
        {
            Min = Min - 1;
            Sec = 59;
            RefreshTimer();
            LATBbits.LATB8 = !LATBbits.LATB8;
            delay_ms(1000);
        }
    }
    CountdownComplete();
    
    return;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0; // clear IF flag
    T2CONbits.TON = 0; // Disable timer
    IEC0bits.T2IE = 0; //Disable timer interrupt
    IOCheck();
    Nop();
    return;
}
