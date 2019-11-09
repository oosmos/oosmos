#!/usr/bin/env python

import os

OOSMOS_DIR = '..'

def Lint(FileName, LintFiles = ''):
  os.system('lint-nt -b common.lnt %s %s %s/%s/%s' % (LintFiles, OPTIONS, OOSMOS_DIR, DIR, FileName))

DIR     = 'Tests'
OPTIONS = '-Doosmos_ORTHO -Doosmos_DEBUG'

Lint('TimeConversions.c',         'oosmos-user.lnt')
Lint('Action.c',                  'oosmos-user.lnt')
Lint('Poll.c',                    'oosmos-user.lnt')
Lint('FreeRunningMicroseconds.c', 'oosmos-user.lnt')
Lint('ThreadEvents.c',            'oosmos-user.lnt')
Lint('ThreadComplete.c',          'oosmos-user.lnt')

DIR = 'Examples/AnalogMap/Windows'

Lint('AnalogMap.c',       'oosmos-user.lnt')


DIR = 'Examples/Basic/Windows'

Lint('DelayMS.c',          'oosmos-user.lnt')
Lint('DelaySeconds.c',     'oosmos-user.lnt')
Lint('DelayUS.c',          'oosmos-user.lnt')
Lint('Final.c',            'oosmos-user.lnt')
Lint('StateTimeout.c',     'oosmos-user.lnt')
Lint('TimeoutInMS.c',      'oosmos-user.lnt')
Lint('TimeoutInSeconds.c', 'oosmos-user.lnt')
Lint('TimeoutInUS.c',      'oosmos-user.lnt')

DIR = 'Examples/DNS/Windows'

Lint('dnstest.c',          'oosmos-user.lnt')

DIR = 'Examples/Ortho/Windows'

Lint('EnterExit.c',  'oosmos-user.lnt')
Lint('Completion.c', 'oosmos-user.lnt')
Lint('Motion.c',     'oosmos-user.lnt')

DIR = 'Examples/EventDemo/Windows'

Lint('control.c', 'oosmos-user.lnt')
Lint('pump.c',    'oosmos-user.lnt')
Lint('motor.c',   'oosmos-user.lnt')
Lint('main.c',    'oosmos-user.lnt')

DIR = 'Examples/NestedStateThreads/Windows'
Lint('NestedStateThreads.c',    'oosmos-user.lnt')

DIR = 'Examples/Thread/Windows'

Lint('main.c',     'oosmos-user.lnt')

DIR = 'Examples/ThreadYield/Windows'

Lint('main.c',     'oosmos-user.lnt')

DIR = 'Examples/LinearRegression/Windows'

Lint('main.c',         'oosmos-user.lnt')

DIR = 'Examples/Interrupt/Windows'

Lint('interrupt.c',         'oosmos-user.lnt')

DIR = 'Classes'

Lint('adc.c',          'oosmos-user.lnt')
OPTIONS = '-Dpwm_DEBUG'
Lint('pwm.c',          'oosmos-user.lnt')
OPTIONS = '-Dpid_DEBUG'
Lint('pid.c',          'oosmos-user.lnt')
OPTIONS = '-Dpin_DEBUG'
Lint('pin.c',          'oosmos-user.lnt pin.lnt')
Lint('matrix.c',       'oosmos-user.lnt matrix.lnt')
Lint('reg.c',          'oosmos-user.lnt')
Lint('tok.c',          'oosmos-user.lnt')
Lint('keyer.c',        'oosmos-user.lnt')
Lint('keypad.c',       'oosmos-user.lnt')
Lint('encoder.c',      'oosmos-user.lnt')
Lint('btn.c',          'oosmos-user.lnt')
Lint('btnph.c',        'oosmos-user.lnt')
Lint('sw.c',           'oosmos-user.lnt')
Lint('toggle_state.c', 'oosmos-user.lnt')
Lint('matrix_state.c', 'oosmos-user.lnt')
Lint('toggle.c',       'oosmos-user.lnt')
Lint('toggle_state.c', 'oosmos-user.lnt')
Lint('util.c',         'oosmos-user.lnt')
Lint('prt.c',          'oosmos-user.lnt')

Lint('accum.c',        'oosmos-user.lnt')
OPTIONS = '-Daccum_DEBUG'
Lint('accum.c',        'oosmos-user.lnt')

DIR = 'Source'

Lint('oosmos.c')

OPTIONS = '-DoosmosORTHO -Doosmos_DEBUG'

Lint('oosmos.c')
