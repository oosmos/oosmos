oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['Motion.c', oosmos_c],     '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Completion.c', oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['EnterExit.c', oosmos_c],  '-Doosmos_ORTHO')
