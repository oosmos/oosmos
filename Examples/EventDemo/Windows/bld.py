import sys

sys.path.append('..\..\..')
import oosmos

oosmos.cWindows.Compile('main.c control.c key.c motor.c pump.c', '-Doosmos_ORTHO')
