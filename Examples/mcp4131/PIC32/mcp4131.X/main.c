/*
// OOSMOS - mcp4131 Example 
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

#include "spi.h"
#include "pin.h"
#include "oosmos.h"
#include "mcp4131test.h"

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  //
  // Create an SPI bus.
  //
  pin * pCLK  = pinNew(IOPORT_B, BIT_12, pinOut, pinActiveHigh);  // Expansion pin J10-49
  pin * pMOSI = pinNew(IOPORT_B, BIT_13, pinOut, pinActiveLow);   // Expansion pin J10-50
  spi * pSPI  = spiNew(pCLK, pMOSI, NULL);

  //
  // Allocate chip select pin for an SPI slave.
  //
  pin * pCS1  = pinNew(IOPORT_B, BIT_10, pinOut, pinActiveLow);   // Expansion pin J10-47
  
  //
  // Allocate chip select pin for another SPI slave.
  //
  pin * pCS2  = pinNew(IOPORT_B, BIT_11, pinOut, pinActiveLow);   // Expansion pin J10-48

  //
  // Create tests for two mcp4131 devices on that SPI bus. The tests will create
  // the SPI slave objects on the SPI bus from the specified chip select pins.
  //
  mcp4131testNew(pSPI, pCS1, 2);
  mcp4131testNew(pSPI, pCS2, 8);

  //
  // Run.
  //
  while (true)
    oosmos_RunStateMachines();
}
