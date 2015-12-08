#!/usr/bin/env python

import os

def Remove(Filename):
  os.system('rm -f %s' % Filename)

Remove('DelayMS')
Remove('DelaySeconds')
Remove('DelayUS')
Remove('Final')
Remove('TimeoutInMS')
Remove('TimeoutInSeconds')
Remove('TimeoutInUS')
Remove('StateTimeout')
