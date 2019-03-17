oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

accumtest_c = oosmos_dir+r'\Classes\Tests\accumtest.c'
oosmos_c    = oosmos_dir+r'\Source\oosmos.c'
accum_c     = oosmos_dir+r'\Classes\accum.c'

oosmos.cWindows.Compile(oosmos_dir, ['main.c',accum_c,accumtest_c,oosmos_c])
