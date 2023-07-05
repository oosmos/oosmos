//
// OOSMOS btn Class
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

#ifndef btn_h
#define btn_h

#include "oosmos.h"
#include "pin.h"

typedef struct btnTag btn;

#if defined(__cplusplus)
extern "C" {
#endif

extern btn* btnNew(pin* pPin);

extern void btnSubscribeReleasedEvent(btn* pButton, oosmos_sQueue* pQueue, int ReleasedEventCode, void* pContext);
extern void btnSubscribePressedEvent(btn* pButton, oosmos_sQueue* pQueue, int PressedEventCode, void* pContext);

#if defined(__cplusplus)
}
#endif

#endif
