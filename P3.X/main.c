/*
 * File:   main.c
 * Author: JoeyA
 *
 * Created on September 29, 2020, 6:33 PM
 */


#include "xc.h"
#include "IOs.h"

unsigned int globalVar;

int main(void) {
    IO_init();
    
    while (1)
    {
        IO_check();
    }
    
    return 0;
}
