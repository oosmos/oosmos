#!/usr/bin/env python

import sys

sys.path.append('../../..')
import oosmos

oosmos.cLinux.Compile('Final',            'Final.c')
oosmos.cLinux.Compile('DelayMS',          'DelayMS.c')
oosmos.cLinux.Compile('DelaySeconds',     'DelaySeconds.c')
oosmos.cLinux.Compile('DelayUS',          'DelayUS.c')
oosmos.cLinux.Compile('TimeoutInMS',      'TimeoutInMS.c')
oosmos.cLinux.Compile('TimeoutInSeconds', 'TimeoutInSeconds.c')
oosmos.cLinux.Compile('TimeoutInUS',      'TimeoutInUS.c')
oosmos.cLinux.Compile('StateTimeout',     'StateTimeout.c')
