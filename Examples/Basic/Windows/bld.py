import sys
import os

oosmos_dir = r'..\..\..'

sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'
pin_c    = oosmos_dir+r'\Classes\pin.c'
btn_c    = oosmos_dir+r'\Classes\btn.c'

os.system(oosmos_dir + r'\Gen\gen.exe Basic.json')

oosmos.cWindows.Compile(oosmos_dir, ['AllEvents.c', oosmos_c, pin_c, btn_c], '-DbtnMaxButtons=4')
oosmos.cWindows.Compile(oosmos_dir, ['Final.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['DelayMS.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['DelaySeconds.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['TimeoutInSeconds.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['TimeoutInMS.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['TimeoutInUS.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['StateTimeout.c', oosmos_c])
