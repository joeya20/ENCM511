// MPLAB header libraries
#include <xc.h>
#include <p24fxxxx.h>
#include <p24F16KA101.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>

// User header files
#include "IOs.h"
#include "ChangeClk.h"
#include "TimeDelay.h"
#include "UART2.h"

//Preprocessor directives - Configuration bits for MCU start up
#pragma config IESO = OFF    // 2 Speed Startup disabled 
#pragma config FNOSC = FRC  // Start up CLK = 8 MHz 
#pragma config FCKSM = CSECMD // Clock switching is enabled, clock monitor off  
#pragma config SOSCSEL = SOSCLP // Secondary oscillator for Low Power Operation 
#pragma config POSCFREQ = MS  //Primary Oscillator/External clk freq betwn  
#pragma config OSCIOFNC = ON  //CLKO output disabled on pin 8, use as IO.  
#pragma config POSCMOD = NONE  // Primary oscillator mode is disabled


// MACROS for Idle, Sleep modes
#define Nop() {__asm__ volatile ("nop");}
#define ClrWdt() {__asm__ volatile ("clrwdt");}
#define Sleep() {__asm__ volatile ("pwrsav #0");}   //Sleep() - put MCU in sleep mode - CPU and some peripherals off
#define Idle() {__asm__ volatile ("pwrsav #1");}    //Idle() - put MCU in idle mode - only CPU off
#define dsen() {__asm__ volatile ("BSET DSCON, #15");} //

//Global Variables
uint8_t state = 0;
const uint16_t CLOCK = 500;       //system clk
const uint8_t PRESCALER = 64;    // TMR2 updates every prescler*2/fclk
const uint16_t MULTIPLIER = 1;     // Multiplier to get frequency in kHz

//MAIN
int main(void) {
     
    //Change Clock       
    NewClk(CLOCK);                  // 8 for 8 MHz; 500 for 500 kHz; 32 for 32 kHz     
    //Clock output on REFO/RB15 ? PULSE GEN Testing purposes only      
    TRISBbits.TRISB15 = 0;          // Set RB15 as output for REFO      
    REFOCONbits.ROSSLP = 1;         // Ref oscillator is disabled in sleep      
    REFOCONbits.ROSEL = 0;          // Output base clk showing clock switching      
    REFOCONbits.RODIV = 0b0111;     // decides the pulse freq
    T2CONbits.TCKPS = 0b10;     //set prescaler 
    
    REFOCONbits.ROEN = 0;           // Ref oscillator is disabled for now because it is not in use (More efficient to be off when not)

    IOinit();   //initialize IOs
    
    while(1)
    {
        if(state == 0) 
        {
            Idle();         //if global flag is off
        }
        else 
        {
            IOCheck();      //if CN ISR is triggered and Global flag is set then check IO
        }
    }
    return 0;
}
