#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

prt_c             = oosmos_dir+'/Classes/prt.c'
threadyieldtest_c = oosmos_dir+'/Classes/Tests/threadyieldtest.c'
oosmos_c          = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['../Windows/main.c', threadyieldtest_c,prt_c,oosmos_c])
