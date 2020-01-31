//
// OOSMOS pid Class
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

#ifndef PID_H
#define PID_H

typedef struct pidTag pid;

extern pid * pidNew(float Kp, float Ki, float Kd, float SetPoint);

extern float pidAdjustOutput(pid * pPID, float Input);

extern void pidSet_SetPoint(pid * pPID, float SetPoint);
extern void pidSet_Kp(pid * pPID, float Kp);
extern void pidSet_Ki(pid * pPID, float Ki);
extern void pidSet_Kd(pid * pPID, float Kd);

#endif
