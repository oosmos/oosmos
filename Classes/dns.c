//
// OOSMOS dns Class
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "sock.h"
#include "dns.h"

#ifdef _WIN32
  #include <winsock2.h>
  #include <inaddr.h>     // for IN_ADDR, s_addr
  #include <minwindef.h>  // for MAKEWORD, WORD
  #include <winerror.h>   // for WSAEWOULDBLOCK, WSAECONNABORTED, WSAECONNREFUSED

  typedef SOCKET dns_tSocket;

  typedef int socklen_t;
  #define IOCTL ioctlsocket
  #define CLOSE closesocket


  #define dnsEWOULDBLOCK  WSAEWOULDBLOCK
  #define dnsECONNRESET   WSAECONNRESET
  #define dnsECONNREFUSED WSAECONNREFUSED
  #define dnsEINPROGRESS  WSAEINPROGRESS
  #define dnsECONNABORTED WSAECONNABORTED
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <errno.h>
  #include <arpa/inet.h>
  #include <sys/ioctl.h>

  typedef int dns_tSocket;

  #define IOCTL ioctl
  #define CLOSE close

  #define dnsEWOULDBLOCK  EWOULDBLOCK
  #define dnsECONNRESET   ECONNRESET
  #define dnsECONNREFUSED ECONNREFUSED
  #define dnsEINPROGRESS  EINPROGRESS
  #define dnsECONNABORTED ECONNABORTED
#endif

struct dnsTag
{
  struct {
    bool bFirst;
  } dnsQuery;

  dns_tSocket Socket;
};

typedef struct
{
  uint8_t TransactionID[2];

  uint8_t Flags[2];

  uint8_t Questions[2];
  uint8_t Answers[2];
  uint8_t AuthorityRRs[2];
  uint8_t AdditionalRRs[2];
} tHeader;

typedef struct
{
  // Variable length 'Name' precedes these fields
  uint8_t Type[2];
  uint8_t Class[2];
} tQuestionTail;

typedef struct
{
  uint8_t Name[2];
  uint8_t Type[2];
  uint8_t Class[2];
  uint8_t TTL[4];
  uint8_t Length[2];
  uint8_t Address[4];
} tAnswer;

static uint32_t NameServerIP;
static uint16_t TransactionID = 1;

static void Init(void)
{
  static bool Initialized = false;

  if (!Initialized) {
    #ifdef _WIN32
      WSADATA wsaData;
      WORD wVersionRequested = MAKEWORD(1, 1);
      WSAStartup(wVersionRequested, &wsaData);
    #endif

    #if 1
      // DNS the easy way... Use the Google name server.
      NameServerIP = sockDotToIP_HostByteOrder("8.8.8.8");
    #else
      // DNS the harder way... Interrogate platform-dependent elements.
      #ifdef _WIN32
        FIXED_INFO   Dummy;
        ULONG        Size = sizeof(Dummy);
        FIXED_INFO * pFixedInfo = &Dummy;

        GetNetworkParams(pFixedInfo, &Size);

        pFixedInfo = malloc(Size);
        GetNetworkParams(pFixedInfo, &Size);
        NameServerIP = sockDotToIP_HostByteOrder(pFixedInfo->DnsServerList.IpAddress.String);
        free(pFixedInfo);
      #else
        char IP[80];

        FILE * pFile = fopen("/etc/resolv.conf", "r");
          fscanf(pFile, "nameserver %s\n", IP);
        fclose(pFile);
        NameServerIP = sockDotToIP_HostByteOrder(IP);
      #endif
    #endif

    Initialized = true;
  }
}

static int dnsGetLastError(void)
{
#ifdef _WIN32
  return WSAGetLastError();
#else
  return errno;
#endif
}

//
// Convert 'cnn.com\x0' to '\x3cnn\x3com\x0'.
//
static size_t DomainToDnsDomain(const char * pDomain, uint8_t * pDnsDomain)
{
  size_t Bytes = 0;

  do {
    char * pDot   = strchr(pDomain, '.');
    size_t Length = pDot == NULL ? strlen(pDomain) : (size_t)(pDot-pDomain);

    *pDnsDomain++ = (uint8_t) Length;
    Bytes += Length;

    while (Length-- > 0) {
      *pDnsDomain++ = *pDomain++;
    }

    Bytes += 1;
  } while (*pDomain++ != 0);

  *pDnsDomain = 0;
  return Bytes+1;
}

extern bool dnsQuery(dns * pDns, const char * pHost, uint32_t * pIP, int MaxIPs)
{
  char Buffer[200];

  struct sockaddr_in SockAddr;
  socklen_t SockAddrSize = sizeof(SockAddr);
  int Bytes;

  if (pDns->dnsQuery.bFirst) {
    uint16_t ID;
    uint16_t Flags; // Standard Query
    uint16_t Questions;
    uint16_t Answers;
    uint16_t AuthorityRRs;
    uint16_t AdditionalRRs;
    uint8_t * pQuestionName;
    size_t NameSize;
    tQuestionTail * pQuestionTail;
    uint16_t Type;
    uint16_t Class;
    size_t QuerySize;

    const uint16_t DnsPort = 53;

    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family      = AF_INET;
    SockAddr.sin_port        = htons(DnsPort);
    SockAddr.sin_addr.s_addr = htonl(NameServerIP);

    tHeader * pHeader = (tHeader * ) Buffer;

    ID = TransactionID++;
    pHeader->TransactionID[0] = (uint8_t) (ID >> 8);
    pHeader->TransactionID[1] = (uint8_t) (ID);

    Flags = 0x0100; // Standard Query
    pHeader->Flags[0] = (uint8_t) (Flags >> 8);
    pHeader->Flags[1] = (uint8_t) (Flags);

    Questions = 1;
    pHeader->Questions[0] = (uint8_t) (Questions >> 8);
    pHeader->Questions[1] = (uint8_t) (Questions);

    Answers = 0;
    pHeader->Answers[0] = (uint8_t) (Answers >> 8);
    pHeader->Answers[1] = (uint8_t) (Answers);

    AuthorityRRs = 0;
    pHeader->AuthorityRRs[0] = (uint8_t) (AuthorityRRs >> 8);
    pHeader->AuthorityRRs[1] = (uint8_t) (AuthorityRRs);

    AdditionalRRs = 0;
    pHeader->AdditionalRRs[0] = (uint8_t) (AdditionalRRs >> 8);
    pHeader->AdditionalRRs[1] = (uint8_t) (AdditionalRRs);

    pQuestionName = (uint8_t *) (pHeader + 1);
    NameSize = DomainToDnsDomain(pHost, pQuestionName);

    pQuestionTail = (tQuestionTail * ) (pQuestionName + NameSize);
    Type = 1;
    pQuestionTail->Type[0] = (uint8_t) (Type >> 8);
    pQuestionTail->Type[1] = (uint8_t) (Type);

    Class = 1;
    pQuestionTail->Class[0] = (uint8_t) (Class >> 8);
    pQuestionTail->Class[1] = (uint8_t) (Class);

    QuerySize = sizeof(tHeader) + NameSize + sizeof(tQuestionTail);

    if (sendto(pDns->Socket, Buffer, QuerySize, 0, (struct sockaddr *) &SockAddr, SockAddrSize) == -1) {
      exit(1);
    }

    pDns->dnsQuery.bFirst = false;
    return false;
  }

  Bytes = recvfrom(pDns->Socket, Buffer, sizeof(Buffer), 0,
                                        (struct sockaddr *) &SockAddr, &SockAddrSize);
  if (Bytes == -1) {
    if (dnsGetLastError() == dnsEWOULDBLOCK) {
      return false;
    }

    exit(1);
  }

  {
    const tHeader * pHeader = (tHeader *) Buffer;

    const uint16_t Answers = (uint8_t) (pHeader->Answers[0] << 8) |
                             (uint8_t) (pHeader->Answers[1]);

    const uint8_t * pQuery       = (const uint8_t *) (pHeader + 1);
    const size_t    QueryNameLen = strlen((const char *) pQuery);

    const tAnswer * pAnswer = (tAnswer *) (pQuery + (QueryNameLen + 1) + sizeof(tQuestionTail));

    int Count;

    memset(pIP, 0, sizeof(*pIP) * MaxIPs);

    for (Count = 1; Count <= Answers; Count++) {
      *pIP = pAnswer->Address[0] << 24 |
             pAnswer->Address[1] << 16 |
             pAnswer->Address[2] << 8  |
             pAnswer->Address[3];

      if (Count == MaxIPs)
        break;

      pAnswer++;
      pIP++;
    }
  }

  pDns->dnsQuery.bFirst = true;
  return true;
}

extern dns * dnsNew()
{
  dns * pDns = malloc(sizeof(dns));

  pDns->dnsQuery.bFirst = true;

  Init();

  if ((pDns->Socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    exit(1);
  }

  //
  // Make the socket non-blocking.
  //
  {
    unsigned long IsNonBlocking = 1;
    IOCTL(pDns->Socket, FIONBIO, &IsNonBlocking);
  }

  return pDns;
}

extern void dnsDelete(dns * pDns)
{
  CLOSE(pDns->Socket);
  free(pDns);
}
