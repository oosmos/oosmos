oosmos_dir = r'..\..\..\oosmos'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

os.system(r'..\..\Gen\gen.exe tests.json')
oosmos.cWindows.Compile(oosmos_dir, ['threadtest.c'], '-c -Doosmos_ORTHO -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['threadyieldtest.c'], '-c -Doosmos_ORTHO -DthreadtestDEBUG')
#os.unlink(r'threadtest.obj')
