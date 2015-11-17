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

#ifndef spi_h
#define spi_h

#include "oosmos.h"
#include "pin.h"

typedef struct spiTag spi;
typedef struct spi_sSlaveTag spi_sSlave;

extern spi * spiNew(pin * pCLK, pin * pMOSI, pin * pMISO);
extern spi_sSlave * spiNewSlave(spi * pSPI, pin * pChipSelect);

extern void spiSendByte(spi_sSlave * pDevice, uint8_t Byte);
extern void spiSendBytes(spi_sSlave * pDevice, const void * pBytes, size_t Bytes);

#endif
