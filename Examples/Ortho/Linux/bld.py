#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'Completion', ['../Windows/Completion.c', oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'EnterExit',  ['../Windows/EnterExit.c',  oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'Motion',     ['../Windows/Motion.c',     oosmos_c])
