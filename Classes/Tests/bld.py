oosmos_dir = r'..\..\..\oosmos'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

os.system(r'..\..\Gen\gen.exe tests.json')

oosmos.cWindows.Compile(oosmos_dir, ['threadtest.c'], '-c -Doosmos_ORTHO -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['threadyieldtest.c'], '-c -Doosmos_ORTHO -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['switchtest.c'], '-c -DthreadtestDEBUG')

oosmos.cWindows.Compile(oosmos_dir, ['Sockets/client.c'],   '-c -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['Sockets/server.c'],   '-c -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['Sockets/listener.c'], '-c -DthreadtestDEBUG')
#os.unlink(r'threadtest.obj')
