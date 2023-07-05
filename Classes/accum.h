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

#ifndef ACCUM_H
#define ACCUM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct accumTag accum;

#if defined(__cplusplus)
extern "C" {
#endif

extern accum *  accumNew(void);
extern void     accumStart(accum * pAccum);
extern void     accumStop(accum * pAccum);
extern void     accumReset(accum * pAccum);
extern void     accumSetUS(accum * pAccum, uint64_t TallyUS);
extern uint64_t accumGetUS(accum * pAccum);
extern bool     accumHasReachedUS(accum * pAccum, uint64_t US);
extern bool     accumHasReachedMS(accum * pAccum, uint64_t MS);
extern bool     accumHasReachedSeconds(accum * pAccum, uint64_t Seconds);
extern bool     accumHasReachedMinutes(accum * pAccum, uint64_t Minutes);
extern bool     accumHasReachedHours(accum * pAccum, uint64_t Hours);
extern bool     accumHasReachedDays(accum * pAccum, uint64_t Days);

#if defined(__cplusplus)
}
#endif

#endif
