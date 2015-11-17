/*
// OOSMOS Blink Example 
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
#include "toggle.h"

static void SetupToggle(int Pin, int OnTimeMS, int OffTimeMS)
{
  pin * pPin = pinNew(Pin, pinOut, pinActiveHigh);
  toggleNew(pPin, OnTimeMS, OffTimeMS);
}

extern void setup() 
{  
  SetupToggle(13, 2000, 2000);
  SetupToggle(12, 100, 100);
  SetupToggle(11, 50, 1500);
}

extern void loop() 
{
  oosmos_RunStateMachines();
}
