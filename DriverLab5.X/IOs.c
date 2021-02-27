
#include "IOs.h"
#include "TimeDelay.h"
#include "UART2.h"
#include "ChangeClk.h"
#include "string.h"
#include "ADC.h"

extern uint8_t PBFlag;

void IOinit(void)
{
    AD1PCFG = 0xFFDF; // AN5 - Analog; Others - Digital
}

//void IOCheck(void)
//{
//}