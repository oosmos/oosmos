#!/usr/bin/env python

import sys

sys.path.append('../../..')
import oosmos

oosmos.cLinux.Compile('Completion', 'Completion.c')
oosmos.cLinux.Compile('EnterExit',  'EnterExit.c')
oosmos.cLinux.Compile('Motion',     'Motion.c')
