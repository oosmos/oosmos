import sys

oosmos_dir = r'..\..\..'

sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

oosmos.cWindows.Compile(oosmos_dir, ['Final.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['DelayMS.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['DelaySeconds.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['TimeoutInSeconds.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['TimeoutInMS.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['TimeoutInUS.c', oosmos_c])
oosmos.cWindows.Compile(oosmos_dir, ['StateTimeout.c', oosmos_c])
