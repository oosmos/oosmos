import os

oosmos_dir = r'..\..\..'

import sys
sys.path.append(oosmos_dir)
import oosmos

oosmos_c = oosmos_dir+r'\Source\oosmos.c'

os.system(oosmos_dir + r'\Gen\gen.exe StateThreads.json') 

oosmos.cWindows.Compile(oosmos_dir, ['StateThreads.c',oosmos_c], '-Doosmos_ORTHO')
