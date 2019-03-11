#!/usr/bin/env python

oosmos_dir  = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

classes_dir = oosmos_dir + '/Classes'
source_dir  = oosmos_dir + '/Source'

dns_c     = classes_dir+'/dns.c'
sock_c    = classes_dir+'/sock.c'
oosmos_c  = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['../Windows/main.c','../Windows/httptest.c',dns_c,sock_c,oosmos_c], '-I../Windows')
