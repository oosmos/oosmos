//
// OOSMOS prt Class
//
// Copyright (C) 2014-2019  OOSMOS, LLC
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
//

#define MaxBuffer 100

#include "prt.h"
#include "oosmos.h"
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

#if defined(ARDUINO)
  extern uint32_t prtArduinoBaudRate = 115200;
#endif

static void Init()
{
  static bool First = true;

  if (!First) {
    return;
  }

  First = false;

  #if defined(ARDUINO)
    Serial.begin(prtArduinoBaudRate);
  #endif
}

extern void prtFormatted(const char * pFormat, ...)
{
  char Buffer[MaxBuffer];

  va_list ArgList; //lint -e438
  va_start(ArgList, pFormat);
    (void) vsnprintf(Buffer, MaxBuffer, pFormat, ArgList);
  va_end(ArgList);

  Init();

  #if defined(ARDUINO)
    Serial.print(Buffer);
  #else
    printf("%s", Buffer);
  #endif
}
