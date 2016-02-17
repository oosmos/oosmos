#!/usr/bin/env python


oosmos_dir = '../../..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+'/Source/oosmos.c'

oosmos.cLinux.Compile(oosmos_dir, 'AnalogMap', ['../Windows/AnalogMap.c', oosmos_c])
