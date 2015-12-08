#!/usr/bin/env python

import sys

sys.path.append('../../..')
import oosmos

oosmos.cLinux.Compile('servertest', 'servertest.c server.c sock.c listener.c')
oosmos.cLinux.Compile('clienttest', 'clienttest.c client.c sock.c')
