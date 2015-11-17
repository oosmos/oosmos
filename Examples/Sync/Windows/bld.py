import sys

sys.path.append('../../..')
import oosmos

oosmos.cWindows.Compile('main.c synctest.c prt.c', '-Doosmos_ORTHO')
