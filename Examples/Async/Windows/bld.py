oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

prt_c       = oosmos_dir+r'\Classes\prt.c'
asynctest_c = oosmos_dir+r'\Classes\Tests\asynctest.c'
oosmos_c    = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['main.c',asynctest_c,prt_c,oosmos_c], '-Doosmos_ORTHO')
