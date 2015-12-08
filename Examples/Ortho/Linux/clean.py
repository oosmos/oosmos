#!/usr/bin/env python

import os

def Remove(Filename):
  os.system('rm -f %s' % Filename)

Remove('Completion')
Remove('EnterExit')
Remove('Motion')
