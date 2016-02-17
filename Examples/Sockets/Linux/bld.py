#!/usr/bin/env python

oosmos_dir = r'../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

sock_c     = oosmos_dir+r'/Classes/sock.c'
oosmos_c   = oosmos_dir+r'/Source/oosmos.c'
client_c   = oosmos_dir+r'/Classes/Tests/Sockets/client.c'
server_c   = oosmos_dir+r'/Classes/Tests/Sockets/server.c'
listener_c = oosmos_dir+r'/Classes/Tests/Sockets/listener.c'
clienttest_c = oosmos_dir+r'/Classes/Tests/Sockets/clienttest.c'
servertest_c = oosmos_dir+r'/Classes/Tests/Sockets/servertest.c'

oosmos.cLinux.Compile(oosmos_dir, 'clienttest', [clienttest_c,client_c,sock_c,oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'servertest', [servertest_c,server_c,sock_c,listener_c,oosmos_c])
