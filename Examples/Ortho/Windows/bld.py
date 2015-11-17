import sys

sys.path.append('..\..\..')
import oosmos

oosmos.cWindows.Compile('Motion.c',     '-Doosmos_ORTHO')
oosmos.cWindows.Compile('Completion.c', '-Doosmos_ORTHO')
oosmos.cWindows.Compile('EnterExit.c',  '-Doosmos_ORTHO')
