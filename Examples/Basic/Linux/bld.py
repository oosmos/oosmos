#!/usr/bin/env python

oosmos_dir  = "../../.."

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

oosmos_c  = os.path.normpath(oosmos_dir+"/Source/oosmos.c")

oosmos.cLinux.Compile(oosmos_dir, 'Final',            ['../Windows/Final.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'DelayMS',          ['../Windows/DelayMS.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'DelaySeconds',     ['../Windows/DelaySeconds.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'DelayUS',          ['../Windows/DelayUS.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'TimeoutInMS',      ['../Windows/TimeoutInMS.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'TimeoutInSeconds', ['../Windows/TimeoutInSeconds.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'TimeoutInUS',      ['../Windows/TimeoutInUS.c',oosmos_c])
oosmos.cLinux.Compile(oosmos_dir, 'StateTimeout',     ['../Windows/StateTimeout.c',oosmos_c])
