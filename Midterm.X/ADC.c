#include "ADC.h"
#include "xc.h"
#include "ChangeClk.h"

void set_output_pin(uint8_t state){
    if(state == 1){
        AD1CHSbits.CH0SA = 0b0101;  //use VRef+ for AN5
    }
    else if(state ==2){
        AD1CHSbits.CH0SA = 0b1011;  //use VRef+ for AN11
    }
    else {
        return;
    }
    return;
}
uint16_t do_ADC()
{
    uint16_t ADCValue;          //variable to store the input
    
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
    
    /* Sampling and Converting */
    AD1CON1bits.SAMP = 1;       //begin sampling
    while(AD1CON1bits.DONE == 0)//wait until sampling and converting is done
    {}
    
    ADCValue = ADC1BUF0;        //get ADC output from ADC buffer
    AD1CON1bits.SAMP = 0;       //stop sampling
    AD1CON1bits.ADON = 0;       //turn off ADC
    
    //PrintADCToScreen(ADCValue); // print ADC value and bar graph to terminal
    
    return ADCValue;
}
