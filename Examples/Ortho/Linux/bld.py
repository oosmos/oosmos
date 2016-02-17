#!/usr/bin/env python

oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'Completion', ['Completion.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'EnterExit',  ['EnterExit.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'Motion',     ['Motion.c',oosmos_c])
