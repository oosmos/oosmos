/*
// OOSMOS sw Class
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

#ifndef sw_h
#define sw_h

#include "oosmos.h"
#include "pin.h"

typedef struct swTag sw;

extern sw * swNew(pin * pPin);
extern sw * swNewDetached(pin * pPin);

extern void swSubscribeOpenEvent(sw * pSwitch, oosmos_sQueue * pQueue, int OpenEventCode, void * pContext);
extern void swSubscribeCloseEvent(sw * pSwitch, oosmos_sQueue * pQueue, int CloseEventCode, void * pContext);

extern bool swIsOpen(const sw * pSwitch);
extern bool swIsClosed(const sw * pSwitch);
extern void swRunStateMachine(void * pSwitch);

#endif
