//
// Copyright (C) 2014-2020  OOSMOS, LLC
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

#include "oosmos.h"
#include "pin.h"
#include "encoder.h"
#include "prt.h"
#include "encodertest.h"

// Required by prt...
uint32_t prtArduinoBaudRate = 115200;

static void SetupEncoderTest(int PinA, int PinB)
{
  pin * pPinA = pinNew(PinA, pinIn, pinActiveLow);
  pin * pPinB = pinNew(PinB, pinIn, pinActiveLow);

  encodertestNew(pPinA, pPinB);
}

extern void setup()
{
  SetupEncoderTest(7, 8);
}

extern void loop()
{
  oosmos_RunStateMachines();
}
