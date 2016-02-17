
oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos
import os

key_c    = oosmos_dir+r'\Classes\Windows\key.c'
oosmos_c = oosmos_dir+r'\Source\oosmos.c'

INCLUDE = os.path.normpath(oosmos_dir+r'\Classes\Windows')

oosmos.cWindows.Compile(oosmos_dir, ['main.c','control.c',key_c,'motor.c','pump.c',oosmos_c], '-Doosmos_ORTHO -I'+INCLUDE)
