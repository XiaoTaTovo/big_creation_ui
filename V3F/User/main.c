#include "debug.h"
int main(void)
{
    SystemInit();
    SystemAndCoreClockUpdate();
    Delay_Init();
    NVIC_WakeUp_V5F(Core_V5F_StartAddr);
    while(1) { Delay_Ms(1000); }
}
