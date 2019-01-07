oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c          = oosmos_dir+r'\Source\oosmos.c'
prt_c             = oosmos_dir+r'\Classes\prt.c'
threadyieldtest_c = oosmos_dir+r'\Classes\Tests\threadyieldtest.c'

oosmos.cWindows.Compile(oosmos_dir, ['main.c',threadyieldtest_c,prt_c,oosmos_c])
