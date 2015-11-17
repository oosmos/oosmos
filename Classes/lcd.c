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

#include <stdarg.h>
#include <stdio.h>
#include "oosmos.h"
#include "lcd.h"

#ifndef lcdMAX
#define lcdMAX 1
#endif

struct lcdTag
{
  pin * m_pRS;
  pin * m_pRW;
  pin * m_pE;
  
  pin * m_pData4;
  pin * m_pData5;
  pin * m_pData6;
  pin * m_pData7;
};

typedef enum
{
  modeWriteCommand = 1,
  modeWriteData,
  modeReadStatus,
  modeReadData,
} eMode;

typedef enum
{
  bits4 = 1,
  bits8,
} eBits;

typedef enum
{
  font5x8 = 1,
  font5x10,
} eFont;

typedef enum
{
  lines1 = 1,
  lines2,
} eLines;

typedef enum
{
  displayOn = 1,
  displayOff,
} eDisplay;

typedef enum
{
  cursorOff = 1,
  cursorOn,
} eCursor;

typedef enum
{
  blinkingOff = 1,
  blinkingOn,
} eBlinking;

typedef enum
{
  entryIncrement = 1,
  entryDecrement,
} eEntry;

typedef enum
{
  shiftOn = 1,
  shiftOff,
} eShift;

static void SetMode(lcd * pLCD, const eMode Mode)
{
  switch (Mode) {
    case modeWriteCommand:
      pinOff(pLCD->m_pRS);     
      if (pLCD->m_pRW != NULL) pinOff(pLCD->m_pRW);
      break;
    case modeWriteData:
      pinOn(pLCD->m_pRS);    
      if (pLCD->m_pRW != NULL) pinOff(pLCD->m_pRW);
      break;
    case modeReadStatus:
      pinOff(pLCD->m_pRS);     
      if (pLCD->m_pRW != NULL) pinOn(pLCD->m_pRW);
      break;
    case modeReadData:
      pinOn(pLCD->m_pRS);   
      if (pLCD->m_pRW != NULL) pinOn(pLCD->m_pRW);
      break;
  }
}

static void Write4(lcd * pLCD, const uint8_t Value) 
{ 
  (Value & 1 ? pinOn : pinOff)(pLCD->m_pData4);
  (Value & 2 ? pinOn : pinOff)(pLCD->m_pData5);
  (Value & 4 ? pinOn : pinOff)(pLCD->m_pData6);
  (Value & 8 ? pinOn : pinOff)(pLCD->m_pData7);

  pinOn(pLCD->m_pE);
  oosmos_DelayUS(1);
  pinOff(pLCD->m_pE);
}

static void WriteCommand4(lcd * pLCD, const uint8_t Command) 
{
  SetMode(pLCD, modeWriteCommand);
  Write4(pLCD, Command);
}

static void WriteCommand(lcd * pLCD, const uint8_t Command) 
{
  SetMode(pLCD, modeWriteCommand);
  
  Write4(pLCD, Command>>4);
  Write4(pLCD, Command);
}

static void WriteData(lcd * pLCD, const uint8_t Data) 
{
  SetMode(pLCD, modeWriteData);
  
  Write4(pLCD, Data>>4);
  Write4(pLCD, Data);
}

static void CommandFunctionSetInit(lcd * pLCD, const eBits Bits)
{
  uint8_t Command = 0x20;
  
  Command |= (Bits == bits8) << 4;
  
  WriteCommand4(pLCD, Command >> 4);
  oosmos_DelayUS(37);
}

static void CommandFunctionSet(lcd * pLCD, const eBits Bits, const eLines Lines, const eFont Font)
{
  uint8_t Command = 0x20;
  
  Command |= (Bits == bits8)    << 4;
  Command |= (Lines == lines2)  << 3; 
  Command |= (Font == font5x10) << 2;
  
  WriteCommand(pLCD, Command);
  oosmos_DelayUS(37);
}

static void CommandDisplaySet(lcd * pLCD, const eDisplay Display, const eCursor Underline, const eBlinking Blinking)
{
  uint8_t Command = 0x08;
  
  Command |= (Display == displayOn)   << 2;
  Command |= (Underline == cursorOn)  << 1;
  Command |= (Blinking == blinkingOn) << 0;
  
  WriteCommand(pLCD, Command);
  oosmos_DelayUS(37);
}

static void CommandClearDisplay(lcd * pLCD)
{
  const uint8_t Command = 0x01;
  WriteCommand(pLCD, Command);
}

static void CommandEntryModeSet(lcd * pLCD, const eEntry Entry, const eShift Shift)
{
  uint8_t Command = 0x04;
  
  Command |= (Entry == entryIncrement) << 1;
  Command |= (Shift == shiftOn)        << 0;
  
  WriteCommand(pLCD, Command);
  oosmos_DelayUS(37);
}

extern void lcdCursorHome(lcd * pLCD)
{
  const uint8_t Command = 0x02;
  
  WriteCommand(pLCD, Command);
  oosmos_DelayUS(1520);
}

extern void lcdClear(lcd * pLCD)
{
  CommandClearDisplay(pLCD);
  oosmos_DelayUS(1520);
}

extern void lcdPrint(lcd * pLCD, const char * pFormat, ...)
{
  char Buffer[80];
  va_list ArgList;

  va_start(ArgList, pFormat);
  vsprintf(Buffer, pFormat, ArgList);
  va_end(ArgList);

  const char * pString;

  for (pString = Buffer; *pString != '\0'; pString++)
    WriteData(pLCD, *pString);
}

extern void lcdSetCursor(lcd * pLCD, const int RowIndex, const int ColumnIndex)
{
  uint8_t Command = 0x80;
  
  const static int RowOffset[] = { 0x00, 0x40, 0x14, 0x54 };
  
  Command |= RowOffset[RowIndex] + ColumnIndex;
  
  WriteCommand(pLCD, Command);
  oosmos_DelayUS(37);
}

extern lcd * lcdNew(pin * pRS, pin * pRW, pin * pE,
                    pin * pData4, pin * pData5, pin * pData6, pin * pData7)
{
  oosmos_Allocate(pLCD, lcd, lcdMAX, NULL);
    
  pLCD->m_pRS     = pRS;
  pLCD->m_pRW     = pRW;
  pLCD->m_pE      = pE; 
  
  pLCD->m_pData4  = pData4;
  pLCD->m_pData5  = pData5;
  pLCD->m_pData6  = pData6;
  pLCD->m_pData7  = pData7;

  oosmos_DelayMS(50);
  
  CommandFunctionSetInit(pLCD, bits8);
  oosmos_DelayUS(4100);
  
  /*
  // The data sheet does not say one needs to do this twice.  However, 
  // the Arduino does it twice, and it definitely makes the display 
  // more stable when reloading new code without a cold reset.  
  */
  CommandFunctionSetInit(pLCD, bits8);
  oosmos_DelayUS(4100);

  CommandFunctionSetInit(pLCD, bits8);
  oosmos_DelayUS(100);

  CommandFunctionSetInit(pLCD, bits4);  
  CommandFunctionSet(pLCD, bits4, lines2, font5x8);
  CommandDisplaySet(pLCD, displayOn, cursorOff, blinkingOff);
  CommandClearDisplay(pLCD);
  CommandEntryModeSet(pLCD, entryIncrement, shiftOff);
  
  return pLCD;
}
