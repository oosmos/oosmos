/*
// OOSMOS - PIC32 Matrix example main program
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
#include "matrixtest.h"

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  pin * pRow1 = pinNew(IOPORT_E, BIT_7, pinOut, pinActiveLow);
  pin * pRow2 = pinNew(IOPORT_E, BIT_5, pinOut, pinActiveLow);
  pin * pRow3 = pinNew(IOPORT_E, BIT_3, pinOut, pinActiveLow);

  pin * pCol1 = pinNew(IOPORT_C, BIT_1, pinIn,  pinActiveLow);
  pin * pCol2 = pinNew(IOPORT_C, BIT_2, pinIn,  pinActiveLow);
  pin * pCol3 = pinNew(IOPORT_C, BIT_3, pinIn,  pinActiveLow);

  matrixtestNew(pRow1, pRow2, pRow3, pCol1, pCol2, pCol3);

  while (true) {
    oosmos_RunStateMachines();
  }
}
