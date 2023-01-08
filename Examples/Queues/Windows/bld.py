import sys

oosmos_dir = r'..\..\..'

sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'


oosmos.cWindows.Compile(oosmos_dir, ['main.c', oosmos_c])
