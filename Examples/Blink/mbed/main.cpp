//
// OOSMOS - mbed Blink example main program.
//
// Copyright (C) 2014-2016  OOSMOS, LLC
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
//

#include "oosmos.h"
#include "pin.h"
#include "toggle.h"

extern int main() 
{  
  pin * pLED1 = pinNew(LED1, pinOut, pinActiveHigh);
  pin * pLED2 = pinNew(LED2, pinOut, pinActiveHigh);
  pin * pLED3 = pinNew(LED3, pinOut, pinActiveHigh);
  pin * pLED4 = pinNew(LED4, pinOut, pinActiveHigh);
  
  toggleNew(pLED1, 5000, 2000);
  toggleNew(pLED2,  100,  100); 
  toggleNew(pLED3,  500,  500); 
  toggleNew(pLED4,   50, 1500); 
    
  while (true) {
    oosmos_RunStateMachines();
  }
}

