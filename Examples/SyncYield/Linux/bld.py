#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

prt_c           = oosmos_dir+'/Classes/prt.c'
syncyieldtest_c = oosmos_dir+'/Classes/Tests/syncyieldtest.c'
oosmos_c        = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['main.c',syncyieldtest_c,prt_c,oosmos_c])
