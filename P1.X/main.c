/*
 * File:   main.c
 * Author: JoeyA
 *
 * Created on September 15, 2020, 5:15 PM
 */

#include "xc.h"

uint8_t a;
uint16_t b;
uint8_t ctr;

int main(void) {
    
    while(1)
    {
        while(ctr < 10)
        {
            a += 50;
            b += 10000;
            ctr++;
        }
        ctr = 0;
    }
    
    return 0;
}
