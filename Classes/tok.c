//
// tok class - Part of OOSMOS
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

#include "oosmos.h"
#include "tok.h"
#include <string.h>
#include <stdlib.h>

struct tokTag
{
  char * m_pUserString;
  char * m_pIndex;
};

extern const char * tokParse(tok * pTok, const char * pDelimiters)
{
  oosmos_POINTER_GUARD(pTok);
  oosmos_POINTER_GUARD(pDelimiters);

  char  * pStart;
  char  * pEnd;
  char  * pIndex = pTok->m_pIndex;

  if (*pIndex == '\0') {
    return NULL;
  }

  pStart = pIndex + strspn(pIndex, pDelimiters);

  if (*pStart == '\0') {
    return NULL;
  }

  pEnd = pStart + strcspn(pStart, pDelimiters);

  if (*pEnd == '\0') {
    pTok->m_pIndex = pEnd;
    return pStart;
  }

  *pEnd = '\0';
  pTok->m_pIndex = pEnd + 1;

  return pStart;
}

extern tok * tokNew(const char * pUserString)
{
  oosmos_POINTER_GUARD(pUserString);

  tok * pTok = (tok *) malloc(sizeof(tok));

  oosmos_POINTER_GUARD(pTok);

  pTok->m_pUserString = malloc(strlen(pUserString)+1);
  strcpy(pTok->m_pUserString, pUserString);

  pTok->m_pIndex = pTok->m_pUserString;
  return pTok;
}

extern void tokDelete(tok * pTok)
{
  free(pTok->m_pUserString);
  free(pTok);
}
