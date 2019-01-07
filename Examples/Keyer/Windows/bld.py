import sys
import os

oosmos_dir = r'..\..\..'

sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'
pin_c    = oosmos_dir+r'\Classes\pin.c'
btn_c    = oosmos_dir+r'\Classes\btn.c'
keyer_c  = oosmos_dir+r'\Classes\keyer.c'

#os.system(r'node c:\dev\oosmos_dev\GenDev\js\gen.js main.json')

oosmos.cWindows.Compile(oosmos_dir, ['main.c', oosmos_c, pin_c, btn_c, keyer_c])
