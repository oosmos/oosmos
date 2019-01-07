#include "control.h"
#include "oosmos.h"
#include "pin.h"
#include "toggle.h"

typedef struct controlTag control;

static control * controlNew(void)
{
  pin * pPin = pinNew(LD2_GPIO_Port, LD2_Pin, pinOut, pinActiveHigh);
  toggleNew(pPin, 100, 900);
  return NULL;
}

extern void controlSetup(void)
{
  controlNew();
}

extern void controlLoop(void)
{
  oosmos_RunStateMachines();
}
