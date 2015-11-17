/*
// OOSMOS - PIC32 switch example main program.
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
#include "switchtest.h"

#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_1

static void CreateSwitchTest(const IoPortId Port, const int PinNumber)
{
  pin * pPin = pinNew_Debounce(Port, PinNumber, pinIn, pinActiveLow, 50);
  switchtestNew(pPin);
}

extern int main(void)
{
  oosmos_ClockSpeedInMHz(80);

  CreateSwitchTest(IOPORT_C, BIT_1);
  CreateSwitchTest(IOPORT_C, BIT_2);

  while (true) {
    oosmos_RunStateMachines();
  }
}
