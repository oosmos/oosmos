oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

reg_c     = oosmos_dir+r'\Classes\reg.c'
regtest_c = oosmos_dir+r'\Classes\Tests\regtest.c'
prt_c     = oosmos_dir+r'\Classes\prt.c'
oosmos_c  = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['main.c',regtest_c,reg_c,prt_c,oosmos_c])
