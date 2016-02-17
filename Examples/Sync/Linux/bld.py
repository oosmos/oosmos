#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

tests_dir = oosmos_dir+'/Classes/Tests'

classes_dir = oosmos_dir+'/Classes'
prt_c       = classes_dir+'/prt.c'
oosmos_c    = oosmos_dir+'/Source/oosmos.c'
synctest_c  = oosmos_dir+'/Classes/Tests/synctest.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['main.c',synctest_c,prt_c,oosmos_c])
