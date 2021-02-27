
#include "ADC.h"
#include "xc.h"
#include "IOs.h"
#include "UART2.h"
#include "ChangeClk.h"

uint16_t do_ADC(uint8_t inputSelect)
{
    uint16_t ADCValue;
    
    /* Initializing ADC */
    AD1CON1bits.ADON = 1;       //turn on ADC
    AD1CON1bits.FORM = 0b00;    //output unsigned integer
    AD1CON1bits.SSRC = 0b111;   //auto-convert after sampling
    AD1CON1bits.ASAM = 0;       //start sampling when SAMP bit is set
    
    AD1CON2bits.VCFG = 0b000;   //use pic reference voltage
    AD1CON2bits.CSCNA = 0;      //Don't scan inputs
    AD1CON2bits.BUFM = 0;       //output one 16 bit word
    AD1CON2bits.ALTS = 0;       //use MUXA settings
    
    AD1CON3bits.ADRC = 0;       //use system clock
    AD1CON3bits.SAMC = 0b11111; //slow signal so slowest sampling time is okay and uses least amount of power
    
    AD1CHSbits.CH0NA = 0;       //use VRef-
    
    if(inputSelect == 1)        //set the input to sample and convert
    {
        AD1CHSbits.CH0SA = 0b0101;  //use AN5 to get voltage
    }
    else if (inputSelect == 2)
    {
        AD1CHSbits.CH0SA = 0b1011;  //use AN11 to get resistance
    }
    
    /* Sampling and Converting */
    AD1CON1bits.SAMP = 1;       //begin sampling
    while(AD1CON1bits.DONE == 0)//wait until sampling and converting is done
    {}
    
    ADCValue = ADC1BUF0;        //get ADC output from ADC buffer
    AD1CON1bits.SAMP = 0;       //stop sampling
    AD1CON1bits.ADON = 0;       //turn off ADC
    
    return ADCValue;
}