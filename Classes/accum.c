//
// OOSMOS accum Class
//
// Copyright (C) 2014-2020  OOSMOS, LLC
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

#ifndef accumMAX
#define accumMAX 3
#endif

//===================================

#include "accum.h"
#include "oosmos.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(accum_DEBUG)
  #include <stdio.h>
#endif

//
// For reference:
// ==============
//              1 Day is
//             24 Hours is
//          1,440 Minutes is
//         86,400 Seconds is
//     86,400,000 Milliseconds is
// 86,400,000,000 Microseconds
//

struct accumTag {
  uint64_t             m_TallyUS;
  oosmos_sActiveObject m_ActiveObject;
  uint32_t             m_Previous;
  bool                 m_Started;
};

static void Update(void * pObject)
{
  oosmos_POINTER_GUARD(pObject);

  accum * pAccum = (accum *) pObject;

  if (pAccum->m_Started) {
    const uint32_t Previous = pAccum->m_Previous;
    uint64_t       Now      = oosmos_GetFreeRunningUS();

    //
    // Handle the wrap around.
    //
    if (Now < Previous) {
      Now += 0x100000000;
    }

    pAccum->m_TallyUS += (Now - Previous);
    pAccum->m_Previous = (uint32_t) Now;
  }
}

extern accum * accumNew(void)
{
  oosmos_Allocate(pAccum, accum, accumMAX, NULL);
  pAccum->m_TallyUS = 0;
  pAccum->m_Started = false;
  oosmos_ActiveObjectInit(pAccum, m_ActiveObject, Update);

  return pAccum;
}

extern void accumReset(accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

  pAccum->m_TallyUS = 0;
  pAccum->m_Previous = oosmos_GetFreeRunningUS();

  #if defined(accum_DEBUG)
    (void) printf("Accum reset, Tally: %u\n", (uint32_t) pAccum->m_TallyUS);
  #endif
}

extern void accumSetUS(accum * pAccum, uint64_t TallyUS)
{
  oosmos_POINTER_GUARD(pAccum);

  pAccum->m_TallyUS = TallyUS;
}

extern uint64_t accumGetUS(accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

  Update(pAccum);

  return pAccum->m_TallyUS;
}

//
// Can call start multiple times after it's started with no harm.
//
extern void accumStart(accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

  if (pAccum->m_Started) {
    return;
  }

  pAccum->m_Previous = oosmos_GetFreeRunningUS();

  #if defined(accum_DEBUG)
    if (!pAccum->m_Started) {
      (void) printf("Accum Start -- Tally: %I64u, Seconds: %I64u\n", pAccum->m_TallyUS, oosmos_US2Seconds_Truncated(pAccum->m_TallyUS));
    }
  #endif

  pAccum->m_Started = true;
}

extern void accumStop(accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

  Update(pAccum);

  #if defined(accum_DEBUG)
    if (pAccum->m_Started) {
      (void) printf("Accum Stop  -- Tally: %I64u, Seconds: %I64u\n",  pAccum->m_TallyUS, oosmos_US2Seconds_Truncated(pAccum->m_TallyUS));
    }
  #endif

  pAccum->m_Started = false;
}

extern bool accumHasReachedUS(accum * pAccum, uint64_t US)
{
  oosmos_POINTER_GUARD(pAccum);

  if (pAccum->m_Started) {
    Update(pAccum);

    #if defined(accum_DEBUG)
      printf("US: %12I64u\n", pAccum->m_TallyUS);
    #endif
  }

  return pAccum->m_TallyUS >= US;
}

extern bool accumHasReachedMS(accum * pAccum, uint64_t MS)
{
  return accumHasReachedUS(pAccum, oosmos_MS2US(MS));
}

extern bool accumHasReachedSeconds(accum * pAccum, uint64_t Seconds)
{
  return accumHasReachedUS(pAccum, oosmos_Seconds2US(Seconds));
}

extern bool accumHasReachedMinutes(accum * pAccum, uint64_t Minutes)
{
  return accumHasReachedUS(pAccum, oosmos_Minutes2US(Minutes));
}

extern bool accumHasReachedHours(accum * pAccum, uint64_t Hours)
{
  return accumHasReachedUS(pAccum, oosmos_Hours2US(Hours));
}

extern bool accumHasReachedDays(accum * pAccum, uint64_t Days)
{
  return accumHasReachedUS(pAccum, oosmos_Days2US(Days));
}
