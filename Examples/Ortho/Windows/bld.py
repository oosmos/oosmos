import os

oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

os.system(oosmos_dir + r'\Gen\gen.exe ortho.json')

oosmos.cWindows.Compile(oosmos_dir, ['OrthoThreadEvents.c', oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['OrthoThreadStates.c', oosmos_c], '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Motion.c', oosmos_c],            '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['Completion.c', oosmos_c],        '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['EnterExit.c', oosmos_c],         '-Doosmos_ORTHO')
oosmos.cWindows.Compile(oosmos_dir, ['OrthoThreads.c', oosmos_c],      '-Doosmos_ORTHO')
