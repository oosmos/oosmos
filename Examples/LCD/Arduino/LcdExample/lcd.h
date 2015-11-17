/*
// OOSMOS lcd Class
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

#ifndef lcd_h
#define lcd_h

#include "pin.h"

typedef struct lcdTag lcd;

extern lcd * lcdNew(pin * pRS, pin * pRW, pin * pE,
                    pin * pData4, pin * pData5, pin * pData6, pin * pData7);
                    
extern void lcdPrint(lcd * pLCD, const char * pFormat, ...);                    
extern void lcdCursorHome(lcd * pLCD);
extern void lcdClear(lcd * pLCD);
extern void lcdSetCursor(lcd * pLCD, int Column, int Row);

#endif
