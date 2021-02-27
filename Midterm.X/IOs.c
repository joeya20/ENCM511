#include <p24F16KA101.h>

#include "IOs.h"
#include "ADC.h"
#include "UART2.h"
#include "ChangeClk.h"
#include "string.h"
#include "math.h"

extern uint8_t state;           //Mode: voltmeter, ohm meter or pulse meter
uint8_t counter1 = 0;           //counter till rising edge of pulse
uint8_t counter2 = 0;           //counter till falling edge of pulse
uint8_t countMode = 0;          //counter mode to calculate frequency

extern const uint16_t CLOCK;       //system clk
extern const uint8_t PRESCALER;     //TMR2 updates every prescler*2/fclk
extern const uint16_t MULTIPLIER;  //Multiplier to get frequency in kHz
const float VDD = 3;              //Maximum Voltage in the system
float voltage;

void IOinit(void)
{
    AD1PCFG = 0xF7DF; // AN5, AN11 Analog, rest Digital 
    
    /* outputs */
    TRISBbits.TRISB8 = 0; // Set pin RB8 as output
    /* inputs */
    TRISBbits.TRISB4 = 1;   // Set pin RB4 as input
    TRISAbits.TRISA2 = 1;   // Set pin RA2 as input
    TRISAbits.TRISA4 = 1;   // Set pin RA4 as input
    TRISBbits.TRISB12 = 1;  // Ser pin RB12 as input

    CNPU1bits.CN0PUE = 1;   //enables pull up resistor for RA4
    CNPU1bits.CN1PUE = 1;   //enables pull up resistor for RB4
    CNPU2bits.CN30PUE = 1;  //enables pull up resistor for RA2
    
    /* Configure CN IO interrupt */
    CNEN1bits.CN0IE = 1;    //enable CN interrupt for CN0/PB1
    CNEN1bits.CN1IE = 1;    //enable CN interrupt for CN1/PB2
    CNEN2bits.CN30IE = 1;   //enable CN interrupt for CN30/PB3
    CNEN1bits.CN14IE = 1;           //enable CN interrupt for CN14
    
    IPC4bits.CNIP = 0b111;  //set CN interrupt priority
    IFS1bits.CNIF = 0;      // clear CN interrupt flag
    
    IEC1bits.CNIE = 1;      //enable CN interrupt
    
    return;
}

void PrintString(char* MessageToPrint)
{
    int i = 0;              //set i = 0 for while loop
    
    while(i < strlen(MessageToPrint))
    {
        XmitUART2(MessageToPrint[i], 1);    //iterate through each character and append
        i = i + 1;                          //increment
    }   

    return;
}

void PrintFloat(float num)
{   
    char str[10];               //temporary char array

    sprintf(str, "%.3f", num); // Converts -15.567 stored in f into an array of characters
    Disp2String(str); // Displays -15.567 on terminal
    
    return;
}

void PrintVoltage(float voltage) {
    NewClk(8);                          //change clk for smooth printing
        
    PrintString("\rVOLTMETER Voltage =      ");     //print text
    PrintFloat(voltage);                            //print voltage
    PrintString(" V                          ");              //print unit
    
    NewClk(CLOCK);                                  //revert clk
}

void PrintResistance(float resistance) {
    NewClk(8);                                      //change clk for smooth printing
    
    PrintString("\rOHMMETER Resistance =     ");    //print text to screen
    PrintFloat(resistance);                         //print value to screen as integer
    XmitUART2(' ', 1);                              //print space
    XmitUART2(234, 1);                              //print unit
    PrintString("                           ");              //print space
    
    NewClk(CLOCK);                             //change clk back for power efficiency
}


void PrintPulse(double frequency, float amplitude) {
    NewClk(8);                                      //change clk for smooth printing
    
    PrintString("\rPULSEMETER Freq =     ");    //print text to screen
    PrintFloat(frequency);                      //print value to screen as integer
    PrintString(" kHz     Amplitude = ");            //print text
    PrintFloat(amplitude);                      //print amplitude
    PrintString(" V    ");                      //print units
    
    NewClk(CLOCK);                             //change clk back for power efficiency
}

void IOCheck(void)
{

    while(state == 1 || state == 2 || state == 3) {
        
        if(state == 1) {
            set_output_pin(state);                  //configure which pin to read from
            
            float output = do_ADC() / 1023.0;  //get (ADC/max ADC) ratio
            
            voltage = output * VDD;          //Vdd (max voltage) is 3
            
            PrintVoltage(voltage);          //print voltmeter
        }   
        else if(state == 2) {
            set_output_pin(state);                  //configure which pin to read from
            
            float output = do_ADC(state) / 1023.0;  //get (ADC/max ADC) ratio
            
            float resistance = (output*1000)/(1-output);  //calculate resistance
            
            PrintResistance(resistance);            //print ohm meter
        }
        if(state == 3) {
            if(PORTBbits.RB12) {        //if input to RB12 is HIGH
                voltage = VDD;          //store Amplitude
            }
            uint8_t count = counter1 + counter2;                        //calculate total TMR2
            if(count) {                                                 //if total TMR2 not 0 and voltage not 0
                double frequency = ((CLOCK*MULTIPLIER)/(2.0*PRESCALER*count));                //calculate frequency
                PrintPulse(frequency, voltage);                         //print pulse meter
            }
        }
    }
    return;
}

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    if (state == 3) {
        if(countMode == 0) {
            counter1 = TMR2;            //set counter1 to TMR2 until rising edge of pulse
            TMR2 = 0;                   //reset TMR2
        }
        else if (countMode == 1) {
            counter2 = TMR2;            //set counter1 to TMR2 until falling edge of pulse
            TMR2 = 0;                   //reset TMR2
        }
        countMode = !countMode;
    }
    
    if(!PORTAbits.RA2 || !PORTAbits.RA4 || !PORTBbits.RB4) {
        
        if(!PORTAbits.RA2) {
            REFOCONbits.ROEN = 0; // Ref oscillator is disabled
            T2CONbits.TON = 0;      //stop timer
            state = 1;              //set voltmeter state
        }
        else if(!PORTAbits.RA4) {
            REFOCONbits.ROEN = 0; // Ref oscillator is enabled
            T2CONbits.TON = 0;      // stop timer
            state = 2;              //set ohm meter state
        }
        else if(!PORTBbits.RB4){
            state = 3;                      //set pulsemeter state
            TMR2config();                   //configure Timer 2
            T2CONbits.TON = 1;              //start timer
            
            TMR2 = 0;
            
            REFOCONbits.ROEN = 1; // Ref oscillator is enabled
        }
    }
    
    IFS1bits.CNIF = 0; // clear IF flag
    
    return;
}