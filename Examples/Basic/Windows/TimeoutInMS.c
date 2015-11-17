/*
// OOSMOS TimeoutInMS Example
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

#include <stdio.h>
#include "oosmos.h"

static const int WaitTimeMS = 1000;

extern int main(void)
{
  /*
  // Allocate a Timeout object.
  */
  oosmos_sTimeout Timeout;

  /*
  // Set timeout.
  */
  oosmos_TimeoutInMS(&Timeout, WaitTimeMS);

  printf("Waiting for %d milliseconds...\n", WaitTimeMS);

  while (true) {
    /*
    // Check if the time has expired.
    */
    if (oosmos_TimeoutHasExpired(&Timeout))
      break;

    printf("Running...\n");

    /*
    // Be polite. Prevent 100% CPU usage on multi-tasked 
    // machines (e.g. Windows or Linux).
    */
    oosmos_DelayMS(75);
  }

  printf("SUCCESS\n");

  return 0;
}
