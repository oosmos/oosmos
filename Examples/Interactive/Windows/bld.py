import sys
import os

oosmos_dir = r'..\..\..'

sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'
pin_c    = oosmos_dir+r'\Classes\pin.c'
btn_c    = oosmos_dir+r'\Classes\btn.c'

os.system(r'node c:\dev\oosmos_dev\GenDev\js\gen.js interactive.json')

oosmos.cWindows.Compile(oosmos_dir, ['interactive.c', oosmos_c, pin_c, btn_c])
