import sys
import os

oosmos_dir = r'..\..\..'

sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'
pin_c    = oosmos_dir+r'\Classes\pin.c'
toggle_c = oosmos_dir+r'\Classes\toggle.c'

oosmos.cWindows.Compile(oosmos_dir, ['main.c', oosmos_c, pin_c, toggle_c])
