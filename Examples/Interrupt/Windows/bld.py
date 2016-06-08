oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['Interrupt.c',oosmos_c])
