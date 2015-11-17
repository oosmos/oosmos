/*
// OOSMOS spi Class
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

#include "spi.h"
#include "pin.h"
#include "oosmos.h"

struct spiTag
{
  pin * m_pCLK;
  pin * m_pMOSI;
  pin * m_pMISO;
};

struct spi_sSlaveTag
{
  spi * m_pSPI;
  pin * m_pCS;
};

#ifndef spiMAX
#define spiMAX 2
#endif

#ifndef spi_slaveMAX
#define spi_slaveMAX 2
#endif

static void SendByte(spi_sSlave * pSlave, uint8_t Byte)
{
  spi * pSPI = pSlave->m_pSPI;

  pin * pCLK  = pSPI->m_pCLK;
  pin * pMOSI = pSPI->m_pMOSI;

  int Count;

  for (Count = 1; Count <= 8; Count++) {
    ((Byte & 0x80) ? pinOff : pinOn)(pMOSI);

    pinOn(pCLK);
    pinOff(pCLK);

    Byte <<= 1;
  }

  pinOff(pMOSI);
}

extern void spiSendByte(spi_sSlave * pSlave, const uint8_t Byte)
{
  pinOn(pSlave->m_pCS);
  SendByte(pSlave, Byte);
  pinOff(pSlave->m_pCS);
}

extern void spiSendBytes(spi_sSlave * pSlave, const void * pData, const size_t Bytes)
{
  uint8_t * pBytes = (uint8_t *) pData;

  pinOn(pSlave->m_pCS);

  size_t ByteIndex;

  for (ByteIndex = 0; ByteIndex < Bytes; ByteIndex++)
    SendByte(pSlave, pBytes[ByteIndex]);

  pinOff(pSlave->m_pCS);
}

extern spi * spiNew(pin * pCLK, pin * pMOSI, pin * pMISO)
{
  oosmos_Allocate(pSPI, spi, spiMAX, NULL);

  pSPI -> m_pCLK  = pCLK;
  pSPI -> m_pMOSI = pMOSI;
  pSPI -> m_pMISO = pMISO;

  return pSPI;
}

extern spi_sSlave * spiNewSlave(spi * pSPI, pin * pCS)
{
  oosmos_Allocate(pSlave, spi_sSlave, spi_slaveMAX, NULL);

  pSlave->m_pSPI = pSPI;
  pSlave->m_pCS  = pCS;

  return pSlave;
}
