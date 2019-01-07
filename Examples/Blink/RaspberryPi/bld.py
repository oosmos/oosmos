#!/usr/bin/env python

oosmos_dir  = '../../..'

wiringPi_dir = 'wiringPi'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

oosmos_c  = os.path.normpath(oosmos_dir+"/Source/oosmos.c")
pin_c     = oosmos_dir+r'/Classes/pin.c'
toggle_c  = oosmos_dir+r'/Classes/toggle.c'

wiring_options = '-I%s -L%s, -l%s' % (wiringPi_dir, wiringPi_dir, wiringPi_dir)
 
oosmos.cLinux.Compile(oosmos_dir, 'blink', ['main.c', oosmos_c, pin_c, toggle_c], '-Doosmos_RASPBERRY_PI '+wiring_options)
