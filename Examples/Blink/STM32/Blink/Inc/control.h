#ifndef CONTROL_H
#define CONTROL_H

#include "oosmos.h"
#include <stdint.h>

typedef struct
{
  oosmos_sEvent Event;
  uint32_t      ErrorNumber;
} control_sErrorEvent;

extern void controlSetup(void);
extern void controlLoop(void);

#endif
