oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['AnalogMap.c',oosmos_c])
