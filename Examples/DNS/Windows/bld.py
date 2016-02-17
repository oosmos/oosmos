oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

dns_c     = oosmos_dir+r'\Classes\dns.c'
sock_c    = oosmos_dir+r'\Classes\sock.c'
oosmos_c  = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['dnstest.c',dns_c,sock_c,oosmos_c], 'Ws2_32.lib Iphlpapi.lib')
