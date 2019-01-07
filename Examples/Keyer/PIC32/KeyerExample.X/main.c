//
// OOSMOS - KeyerExample PIC32 main program
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "oosmos.h"
#include "pin.h"
#include "keyer.h"
#include <stdbool.h>

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  pin * pDahPin     = pinNew(IOPORT_C, BIT_1, pinIn,  pinActiveLow);
  pin * pDitPin     = pinNew(IOPORT_C, BIT_2, pinIn,  pinActiveLow);
  pin * pSpeakerPin = pinNew(IOPORT_C, BIT_3, pinOut, pinActiveHigh);

  const int WPM = 15;

  keyerNew(pDahPin, pDitPin, pSpeakerPin, WPM);

  for (;;) {
    oosmos_RunStateMachines();
  }
}
