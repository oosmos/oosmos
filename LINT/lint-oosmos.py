#!/usr/bin/env python

import os

def Lint(FileName, LintFiles):
    os.system('lint-nt -passes(2) -b common.lnt %s %s %s/%s' % (LintFiles, OPTIONS, DIR, FileName))

OPTIONS = ''
DIR     = '../Source'

Lint('oosmos.c', '')

OPTIONS = '-DoosmosORTHO -Doosmos_DEBUG'

Lint('oosmos.c', '')
