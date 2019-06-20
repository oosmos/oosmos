oosmos_dir = r'..\..\..\oosmos'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

os.system(r'node c:\dev\oosmos_dev\gen_dev\js\gen.js tests.json')

oosmos.cWindows.Compile(oosmos_dir, ['pidtest.c'],    '-c -Doosmos_ORTHO -Dpid_DEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['accumtest.c'],  '-c -Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['threadtest.c'], '-c -Doosmos_ORTHO -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['threadyieldtest.c'], '-c -Doosmos_ORTHO -DthreadtestDEBUG')
oosmos.cWindows.Compile(oosmos_dir, ['switchtest.c'], '-c')
oosmos.cWindows.Compile(oosmos_dir, ['matrixtest.c'], '-c')

oosmos.cWindows.Compile(oosmos_dir, ['Sockets/client.c'], '-c')
oosmos.cWindows.Compile(oosmos_dir, ['Sockets/server.c'], '-c')
oosmos.cWindows.Compile(oosmos_dir, ['Sockets/listener.c'], '-c')
#os.unlink(r'threadtest.obj')
