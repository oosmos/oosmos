import sys

sys.path.append('..\..\..')
import oosmos

oosmos.cWindows.Compile('regtest.c main.c reg.c prt.c')
