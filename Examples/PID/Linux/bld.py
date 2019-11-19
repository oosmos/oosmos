#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c    = oosmos_dir+'/Source/oosmos.c'
pid_c       = oosmos_dir+'/Classes/pid.c'
pidtest_c   = oosmos_dir+'/Classes/Tests/pidtest.c'

oosmos.cLinux.Compile(oosmos_dir, 'main', ['../Windows/main.c',pidtest_c,pid_c,oosmos_c], '-Dpid_DEBUG')
