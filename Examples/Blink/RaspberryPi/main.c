/*
// OOSMOS - Blink main program for Raspberry Pi
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
#include "toggle.h"
#include "pin.h"

extern int main(void)
{
  pin * pRED    = pinNew(0, pinOut, pinActiveHigh);
  pin * pYELLOW = pinNew(2, pinOut, pinActiveHigh);
  pin * pGREEN  = pinNew(3, pinOut, pinActiveHigh);

  toggleNew(pRED,   2000, 2000);
  toggleNew(pYELLOW, 100,  100);
  toggleNew(pGREEN,   50, 1500);

  while (true) {
    oosmos_RunStateMachines();
    oosmos_DelayMS(50);
  }
}
