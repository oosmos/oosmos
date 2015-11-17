//
// Copyright (C) 2014-2015  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <http://www.oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "oosmos.h"
#include "pin.h"
#include "encoder.h"
#include "prt.h"
#include "encodertest.h"

/* Required by prt... */
unsigned long prtArduinoBaudRate = 115200;

static void SetupEncoderTest(int PinA, int PinB, int Max)
{
  pin * pPinA = pinNew_Debounce(PinA, pinIn, pinActiveLow, 2);
  pin * pPinB = pinNew(PinB, pinIn, pinActiveLow);

  encodertestNew(pPinA, pPinB, Max);
}

extern void setup() 
{
  SetupEncoderTest(9, 10, 20);
  SetupEncoderTest(11, 12, 40);
}

extern void loop() 
{
  oosmos_RunStateMachines();
}
