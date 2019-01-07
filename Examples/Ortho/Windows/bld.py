import os

oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

use_node = (len(sys.argv) == 2) and (sys.argv[1] == '--node')

if use_node:
  print('*** Using node directly')
  os.system(r'node ..\..\..\gen ortho.json')
else:
  os.system(r'..\..\..\gen\gen.exe ortho.json')

oosmos.cWindows.Compile(oosmos_dir, ['Motion.c', oosmos_c],     '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Completion.c', oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['EnterExit.c', oosmos_c],  '-Doosmos_ORTHO')
