import sys

sys.path.append('..\..\..')
import oosmos

oosmos.cWindows.Compile('Final.c')
oosmos.cWindows.Compile('DelayMS.c')
oosmos.cWindows.Compile('DelaySeconds.c')
oosmos.cWindows.Compile('TimeoutInSeconds.c')
oosmos.cWindows.Compile('TimeoutInMS.c')
oosmos.cWindows.Compile('TimeoutInUS.c')
oosmos.cWindows.Compile('StateTimeout.c')
