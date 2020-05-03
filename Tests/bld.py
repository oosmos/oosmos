#!/usr/bin/env python

import os

oosmos_dir = r'..'

import sys
sys.path.append(oosmos_dir)
import oosmos
import platform

if platform.system() == 'Windows':
  oosmos_c  = oosmos_dir+r'\Source\oosmos.c'
  pin_c     = oosmos_dir+r'\Classes\pin.c'
  btn_c     = oosmos_dir+r'\Classes\btn.c'
  btnph_c   = oosmos_dir+r'\Classes\btnph.c'

  os.system(oosmos_dir + r'\Gen\gen.exe tests.json')

  oosmos.cWindows.Compile(oosmos_dir, ['ThreadDelayUS.c',           oosmos_c])
  oosmos.cWindows.Compile(oosmos_dir, ['ThreadDelaySeconds.c',      oosmos_c])
  oosmos.cWindows.Compile(oosmos_dir, ['Buttons.c',                 oosmos_c, pin_c, btn_c, btnph_c], '-DbtnMaxButtons=4 -DbtnphMaxButtons=4')
  oosmos.cWindows.Compile(oosmos_dir, ['ObjectThreads.c',           oosmos_c])
  oosmos.cWindows.Compile(oosmos_dir, ['History.c',                 oosmos_c, pin_c, btn_c], '-DbtnMaxButtons=4')
  oosmos.cWindows.Compile(oosmos_dir, ['TimeConversions.c',         oosmos_c], '-Doosmos_ORTHO')
  oosmos.cWindows.Compile(oosmos_dir, ['FreeRunningUS.c',           oosmos_c])
  oosmos.cWindows.Compile(oosmos_dir, ['ThreadComplete.c',          oosmos_c])
  oosmos.cWindows.Compile(oosmos_dir, ['Action.c',                  oosmos_c])
  oosmos.cWindows.Compile(oosmos_dir, ['ThreadEvents.c',            oosmos_c], '-Doosmos_ORTHO')
  oosmos.cWindows.Compile(oosmos_dir, ['Poll.c',                    oosmos_c], '-Doosmos_ORTHO')
else:
  oosmos_c  = oosmos_dir+r'/Source/oosmos.c'
  oosmos.cLinux.Compile(oosmos_dir, 'ThreadDelayUS',      ['ThreadDelayUS.c',      oosmos_c])
  oosmos.cLinux.Compile(oosmos_dir, 'ThreadDelaySeconds', ['ThreadDelaySeconds.c', oosmos_c])
  oosmos.cLinux.Compile(oosmos_dir, 'ObjectThreads',      ['ObjectThreads.c',      oosmos_c])
  oosmos.cLinux.Compile(oosmos_dir, 'TimeConversions',    ['TimeConversions.c',    oosmos_c], '-Doosmos_ORTHO')
