/*
// OOSMOS mcp4131 Class
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

#include "oosmos.h"
#include "mcp4131.h"
#include "spi.h"

#ifndef mcp4131MAX
#define mcp4131MAX 2
#endif

struct mcp4131Tag
{
  spi_sSlave * m_pSlave;
};

extern mcp4131 * mcp4131New(spi_sSlave * pSlave)
{
  oosmos_Allocate(pMCP4131, mcp4131, mcp4131MAX, NULL);

  pMCP4131->m_pSlave = pSlave;
  
  return pMCP4131;
}

extern void mcp4131SetResistance(mcp4131 * pMCP, const int Resistance)
{
  uint8_t Command[2];

  Command[0] = 0;
  Command[1] = (uint8_t) Resistance;

  spiSendBytes(pMCP->m_pSlave, Command, sizeof(Command));
}
