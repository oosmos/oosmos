/*
// OOSMOS - The Object-Oriented State Machine Operating System
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

/*
// It is unclear which #include file holds these prototypes.  We declare
// them here for now.
*/
extern "C" bool system_deep_sleep_set_option(int option);
extern "C" void system_deep_sleep(int);

static void SetupToggle(int Pin, int OnTimeMS, int OffTimeMS)
{
  pin * pPin = pinNew(Pin, pinOut, pinActiveHigh);
  toggleNew(pPin, OnTimeMS, OffTimeMS);
}

extern void setup() 
{  
  SetupToggle(BUILTIN_LED, 2000, 100);
}

extern void loop() 
{
  oosmos_RunStateMachines();
  
  system_deep_sleep_set_option(4);
  system_deep_sleep(10*1000);  /* in uS... */
}
