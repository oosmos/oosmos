#!/usr/bin/env python

import sys

sys.path.append('../../..')
import oosmos

oosmos.cLinux.Compile('blink', 'main.c pin.c prt.c toggle.c -l wiringPi -Doosmos_RASPBERRY_PI')
