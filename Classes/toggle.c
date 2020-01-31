//
// OOSMOS toggle Class
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

#ifndef toggleMAX
#define toggleMAX 5
#endif

//===================================

#include "oosmos.h"
#include "toggle.h"
#include "pin.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct toggleTag
{
  oosmos_sObjectThread m_ObjectThread;

  pin      * m_pPin;
  uint32_t   m_TimeOnMS;
  uint32_t   m_TimeOffMS;
};

static void ToggleThread(const toggle * pToggle, oosmos_sState * pState)
{
  oosmos_POINTER_GUARD(pToggle);
  oosmos_POINTER_GUARD(pState);

  oosmos_ThreadBegin();
    for (;;) {
      pinOn(pToggle->m_pPin);
      oosmos_ThreadDelayMS(pToggle->m_TimeOnMS);

      pinOff(pToggle->m_pPin);
      oosmos_ThreadDelayMS(pToggle->m_TimeOffMS);
    }
  oosmos_ThreadEnd();
}

extern toggle * toggleNew(pin * pPin, uint32_t TimeOnMS, uint32_t TimeOffMS)
{
  oosmos_Allocate(pToggle, toggle, toggleMAX, NULL);

  oosmos_ObjectThreadInit(pToggle, m_ObjectThread, ToggleThread, true);

  pToggle->m_pPin      = pPin;
  pToggle->m_TimeOnMS  = TimeOnMS;
  pToggle->m_TimeOffMS = TimeOffMS;

  return pToggle;
}
