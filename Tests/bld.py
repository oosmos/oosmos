import os

oosmos_dir = r'..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

use_node = (len(sys.argv) == 2) and (sys.argv[1] == '--node')

if use_node:
  print('*** Using node directly')
  os.system(r'node ..\Gen\gen.min.js tests.json')
else:
  os.system(r'..\Gen\gen.exe tests.json')

oosmos.cWindows.Compile(oosmos_dir, ['ThreadComplete.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['Action.c',         oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['ThreadEvents.c',   oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Poll.c',           oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Conversions.c',    oosmos_c], '-Doosmos_ORTHO')
