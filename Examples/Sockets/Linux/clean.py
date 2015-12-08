#!/usr/bin/env python

import os

def Remove(Filename):
  os.system('rm -f %s' % Filename)

Remove('servertest')
Remove('clienttest')
