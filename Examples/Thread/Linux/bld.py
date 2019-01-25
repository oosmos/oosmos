#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

tests_dir = oosmos_dir+'/Classes/Tests'

classes_dir  = oosmos_dir+'/Classes'
prt_c        = classes_dir+'/prt.c'
oosmos_c     = oosmos_dir+'/Source/oosmos.c'
threadtest_c = oosmos_dir+'/Classes/Tests/threadtest.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['../Windows/main.c',threadtest_c,prt_c,oosmos_c])
