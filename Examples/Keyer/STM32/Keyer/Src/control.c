#include "control.h"
#include "oosmos.h"
#include "pin.h"
#include "keyer.h"
#include "toggle.h"

typedef struct controlTag control;

static control * controlNew(void)
{
  pin * pDitPin     = pinNew(Dit_GPIO_Port,    Dit_Pin,    pinIn,  pinActiveLow);
  pin * pDahPin     = pinNew(Dah_GPIO_Port,    Dah_Pin,    pinIn,  pinActiveLow);
  pin * pSpeakerPin = pinNew(Buzzer_GPIO_Port, Buzzer_Pin, pinOut, pinActiveHigh);

  pin * pLED        = pinNew(LD2_GPIO_Port,    LD2_Pin,    pinOut, pinActiveHigh);

  const int WPM = 15;

  keyerNew(pDahPin, pDitPin, pSpeakerPin, WPM);
  toggleNew(pLED, 100, 500);
  return NULL;
}

extern void controlSetup(void)
{
  controlNew();
}

extern void controlLoop(void)
{
  oosmos_RunStateMachines();
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}
