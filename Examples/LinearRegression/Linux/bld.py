#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

reg_c     = oosmos_dir+'/Classes/reg.c'
regtest_c = oosmos_dir+'/Classes/Tests/regtest.c'
prt_c     = oosmos_dir+'/Classes/prt.c'
oosmos_c  = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['main.c',regtest_c,reg_c,prt_c,oosmos_c])
