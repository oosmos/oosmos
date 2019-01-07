//
// OOSMOS accum Class
//
// Copyright (C) 2014-2018  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
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
#include <stdint.h>

#ifdef accum_DEBUG
#include <stdio.h>
#endif

struct accumTag {
  uint64_t             m_TallyUS;
  oosmos_sActiveObject m_ActiveObject;
  uint32_t             m_Previous;
  bool                 m_Started;
};

static void Running(void * pObject)
{
  oosmos_POINTER_GUARD(pObject);

  accum * pAccum = (accum *) pObject;

  if (pAccum->m_Started) {
    const uint32_t Previous = pAccum->m_Previous;
    uint64_t       Now      = oosmos_GetFreeRunningMicroseconds();

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
  oosmos_RegisterActiveObject(pAccum, Running, &pAccum->m_ActiveObject);

  return pAccum;
}

extern void accumReset(accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

  pAccum->m_TallyUS = 0;
  pAccum->m_Previous = oosmos_GetFreeRunningMicroseconds();

  #ifdef accum_DEBUG
    (void) printf("Accum reset, Tally: %u\n", (uint32_t) pAccum->m_TallyUS);
  #endif
}

extern void accumSetUS(accum * pAccum, uint64_t TallyUS)
{
  oosmos_POINTER_GUARD(pAccum);

  pAccum->m_TallyUS = TallyUS;
}

extern uint64_t accumGetUS(const accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

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

  pAccum->m_Previous = oosmos_GetFreeRunningMicroseconds();

  #ifdef accum_DEBUG
    if (!pAccum->m_Started) {
      (void) printf("Accum Start -- Tally: %u, Seconds: %u\n", (uint32_t) pAccum->m_TallyUS, (uint32_t) ((pAccum->m_TallyUS / 1000) / 1000));
    }
  #endif

  pAccum->m_Started = true;
}

extern void accumStop(accum * pAccum)
{
  oosmos_POINTER_GUARD(pAccum);

  Running(pAccum);

  #ifdef accum_DEBUG
    if (pAccum->m_Started) {
      (void) printf("Accum Stop  -- Tally: %u, Seconds: %u\n", (uint32_t) pAccum->m_TallyUS, (uint32_t) ((pAccum->m_TallyUS / 1000) / 1000));
    }
  #endif

  pAccum->m_Started = false;
}

extern bool accumHasReachedUS(accum * pAccum, uint64_t US)
{
  oosmos_POINTER_GUARD(pAccum);

  if (pAccum->m_Started) {
    Running(pAccum);

    return pAccum->m_TallyUS >= US;
  }

  return false;
}

extern bool accumHasReachedMS(accum * pAccum, uint32_t MS)
{
  return accumHasReachedUS(pAccum, ((uint64_t) MS) * 1000);
}

extern bool accumHasReachedSeconds(accum * pAccum, uint32_t Seconds)
{
  return accumHasReachedUS(pAccum, (((uint64_t) Seconds) * 1000) * 1000);
}