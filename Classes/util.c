//
// util class - Part of OOSMOS
//
// Copyright (C) 2014-2018  OOSMOS, LLC
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License ("GPLv2").
//
// This software may be used without the GPLv2 restrictions by entering
// into a commercial license agreement with OOSMOS, LLC.
// See <https://oosmos.com/licensing/>.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

extern void utilHexDump(const void * pBuffer, const size_t TotalBytes)
{
  const uint8_t * pByte    = pBuffer;
  const char    * pCurrent = pBuffer;

  uint8_t    Dump[16 * 3 + 1];
  uint8_t    Ascii[16 + 1];

  uint8_t  * pDump = Dump;
  uint8_t  * pAscii = Ascii;

  size_t     Bytes;

  for (Bytes = 0; Bytes <= TotalBytes; pByte++, Bytes++) {
    if ((Bytes % 16) == 0 || Bytes == TotalBytes) {
      if (Bytes != 0) {
        printf("%p: %s|%.16s|\n", pCurrent, Dump, Ascii);
        pCurrent += 16;
      }

      if (Bytes == TotalBytes) {
        return;
      }

      pDump  = Dump;
      pAscii = Ascii;

      memset(Dump, ' ', sizeof(Dump)-1);
      Dump[sizeof(Dump)-1] = 0;

      memset(Ascii, ' ', sizeof(Ascii)-1);
      Ascii[sizeof(Ascii)-1] = 0;
    }

    //
    // This is a very fast, straight-through, binary-to-hex ASCII conversion.
    //
    const uint8_t Byte = *pByte;
    *pDump++ = (uint8_t) "0123456789ABCDEF"[(Byte >>  4) & 0xF];
    *pDump++ = (uint8_t) "0123456789ABCDEF"[(Byte      ) & 0xF];
    *pDump++ = ' ';

    *pAscii++ = isprint(Byte) ? Byte : '.';
  }
}
