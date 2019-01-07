#include "sleep.h"
#include "stm32l1xx_hal.h"

extern void sleepNow(void)
{   
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
