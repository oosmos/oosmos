//
// OOSMOS StateTimeout Example
//
// Copyright (C) 2014-2019  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://www.oosmos.com/licensing/>.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int main(void)
{
  printf(". (period) - dit\n");
  printf("/ (slash)  - dah\n");
  printf("\n");

  pin * pDitPin     = pinNew('.', pinActiveHigh);
  pin * pDahPin     = pinNew('/', pinActiveHigh);
  pin * pSpeakerPin = pinNew(' ', pinActiveHigh);

  keyerNew(pDahPin, pDitPin, pSpeakerPin, 20);

  for (;;) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(1);
  }
}
