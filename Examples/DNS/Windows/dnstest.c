//
// OOSMOS dnstest 
//
// Copyright (C) 2014-2016  OOSMOS, LLC
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
//

#include <stdio.h>
#include "oosmos.h"
#include "dns.h"

#define MAX_TESTS   15
#define MAX_ANSWERS 3

typedef struct dnstestTag dnstest;

struct dnstestTag {
  const char * pDomain;
  dns        * pDNS;
  uint32_t     IP[MAX_ANSWERS];
  bool         Done;

  oosmos_sActiveObject ActiveObject;
};

static int ActiveInstances = 0;

static void Run(void * pObject)
{
  dnstest * pDnsTest = (dnstest *) pObject;

  if (pDnsTest->Done)
    return;

  printf("Checking %s...\n", pDnsTest->pDomain);
  
  if (dnsQuery(pDnsTest->pDNS, pDnsTest->pDomain, pDnsTest->IP, MAX_ANSWERS)) {
    for (int Answer = 0; Answer < MAX_ANSWERS; Answer++) {
      const uint32_t IP = pDnsTest->IP[Answer];

      if (IP == 0) 
        break;

      printf("%s[%d]: %d.%d.%d.%d\n", pDnsTest->pDomain, Answer, IP>>24 & 0xff, IP>>16 & 0xff, IP>>8 & 0xff, IP & 0xff);
    }

    ActiveInstances -= 1;
    pDnsTest->Done = true;
  }
}

static dnstest * dnstestNew(const char * pDomain)
{
  oosmos_Allocate(pDnsTest, dnstest, MAX_TESTS, NULL);

  pDnsTest->pDNS    = dnsNew();
  pDnsTest->pDomain = pDomain;
  pDnsTest->Done    = false;

  oosmos_RegisterActiveObject(pDnsTest, Run, &pDnsTest->ActiveObject);

  ActiveInstances += 1;

  return pDnsTest;
}

extern int main(void)
{
  dnstestNew("cnn.com");
  dnstestNew("example.com");
  dnstestNew("ibm.com");
  dnstestNew("ycombinator.com");
  dnstestNew("apple.com");
  dnstestNew("google-public-dns-a.google.com.com");

  do  {
    oosmos_RunStateMachines();
    oosmos_DelayMS(5);
  } while (ActiveInstances > 0);

  return 0;
}
