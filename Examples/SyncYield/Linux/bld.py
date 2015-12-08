#!/usr/bin/env python

import sys

sys.path.append('../../..')
import oosmos

oosmos.cLinux.Compile('main', 'main.c syncyieldtest.c prt.c')
