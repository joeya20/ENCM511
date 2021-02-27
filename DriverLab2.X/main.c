/*
 * File:   main.c
 * Author: JoeyA
 *
 * Created on October 8, 2020, 3:48 PM
 */

#include "xc.h"
#include "IOs.h"

int main(void) {
    
    IOinit();
    /* start of super loop */
    while (1)
    {
        IOcheck();   //check state of PBs and turn on LED if needed
    }
    
    return 0;
}
