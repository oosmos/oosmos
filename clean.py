#!/usr/bin/env python

import os
import subprocess
import platform
import oosmos

def HandleDir(FullFilePath, _):
  Path, FileName = os.path.split(FullFilePath)

  if FileName == 'clean.py':
    if platform.system() == 'Windows' and Path.endswith('/Windows'):
      print(r'Cleaning oosmos\%s...' % (Path, ))
      subprocess.call(['python', 'clean.py'], cwd = Path, shell=True)
    elif platform.system() == 'Linux' and Path.endswith('/Linux'):
      print('Cleaning oosmos/%s...' % (Path, ))
      subprocess.call('./clean.py', cwd = Path, shell=True)

oosmos.WalkDir('Examples', HandleDir, None)
