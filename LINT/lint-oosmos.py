#!/usr/bin/env python

import os

def Lint(FileName: str, LintFiles: str):
    os.system('lint-nt -passes(2) -b common.lnt %s %s %s/%s' % (LintFiles, OPTIONS, DIR, FileName))

OPTIONS = '-doosmos_ENABLE_POINTER_GUARDS -doosmos_ORTHO -doosmos_DEBUG'
DIR     = '../Source'

Lint('oosmos.c', '')

