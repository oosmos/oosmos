/*
// OOSMOS prt Class
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

#define MaxBuffer 100

#include <stdarg.h>
#include <stdio.h>
#include "oosmos.h" 
#include "prt.h" 

static void Init()
{
  static bool First = true;

  if (!First)
    return;
    
  First = false;

  #if defined(ARDUINO)
  {
    Serial.begin(prtArduinoBaudRate);
  }
  #elif defined(__PIC32MX)
    DBINIT();
  #endif
}

extern bool prtFormatted(const char * pFormat, ...)
{
  static char Buffer[MaxBuffer];

  va_list ArgList;
  va_start(ArgList, pFormat);
  vsnprintf(Buffer, MaxBuffer, pFormat, ArgList);
  va_end(ArgList);
  
  Init();
  
  #if defined(ARDUINO)
    Serial.print(Buffer);
  #elif defined(__PIC32MX)
    DBPRINTF("%s", Buffer);
  #else
    printf("%s", Buffer);
  #endif  

  return true;
}
