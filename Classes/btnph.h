//
// OOSMOS btnph Class -- Button Press & Hold.
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

#ifndef btnph_h
#define btnph_h

#include "oosmos.h"
#include "pin.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct btnphTag btnph;

extern btnph * btnphNew(pin * pPin, uint32_t HoldTimeMS);

extern void btnphSubscribeReleasedEvent(btnph * pButton, oosmos_sQueue * pQueue, int ReleasedEventCode, void * pContext);
extern void btnphSubscribeHeldEvent    (btnph * pButton, oosmos_sQueue * pQueue, int HeldEventCode,     void * pContext);
extern void btnphSubscribePressedEvent (btnph * pButton, oosmos_sQueue * pQueue, int PressedEventCode,  void * pContext);

#endif
