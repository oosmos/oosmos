oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

pid_c     = oosmos_dir+r'\Classes\pid.c'
pidtest_c = oosmos_dir+r'\Classes\Tests\pidtest.c'
oosmos_c  = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['main.c',pid_c,pidtest_c,oosmos_c], '-Doosmos_ORTHO -DthreadtestDEBUG')
