#!/usr/bin/python
''' Overview:

    There are a lot of redundant file in these examples.  We could have attempted to
    create a system of include directories and libraries, common elements and makefiles,
    as is traditionally done, but much of that would render these examples highly
    platform dependent.  To maximize portablility, we simply keep common files in
    one place and then copy them into the project directories where they are needed
    and render them read-only.

    This means that you can navigate to any of the examples and all the files that it
    depends on are in that directory.

    Source
      oosmos.h
      oosmos.c

    Classes
      Holds common classes that are reused by many examples.

    Examples
      Contains all the examples.
'''

import oosmos

#
# Copy files from a directory, to another directory, possibly changing extensions,
# and making the target file read only.
#
def Copy(From, FromExtensions, To, ToExtensions, Files, bCopyOosmos = True):
  FromExtensions = FromExtensions.split()
  ToExtensions   = ToExtensions.split()

  if bCopyOosmos:
    for Index, FromExtension in enumerate(FromExtensions):
      ToExtension = ToExtensions[Index]

      FromFile = './Source/oosmos.%s' % (FromExtension)
      ToFile   = './%s/oosmos.%s'     % (To, ToExtension)
      oosmos.CopyFileReadOnly(FromFile, ToFile)

  if Files == '':
    return

  Files = Files.split()

  for File in Files:
    for Index, FromExtension in enumerate(FromExtensions):
      ToExtension = ToExtensions[Index]

      FromFile = './%s/%s.%s' % (From, File, FromExtension)
      ToFile   = './%s/%s.%s' % (To, File, ToExtension)
      oosmos.CopyFileReadOnly(FromFile, ToFile)

def Main():
  print('Cleaning Examples...')
  oosmos.Clean('Examples', 'exe obj ilk pdb')

  print('Populating from Classes Into Examples...')

  #
  # Blink
  #
  Copy('Classes', 'h c', 'Examples/Blink/Arduino/BlinkExample',       'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/ESP8266/BlinkExample',       'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/ChipKit/BlinkExample',       'h cpp', 'pin toggle')
  oosmos.CopyFileReadOnly('Examples/Blink/Arduino/BlinkExample/BlinkExample.ino', 'Examples/Blink/ChipKit/BlinkExample/BlinkExample.ino')
  Copy('Classes', 'h c', 'Examples/Blink/mbed',                       'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/MSP430/BlinkExample',        'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/LightBlueBean/BlinkExample', 'h cpp', 'pin toggle')

  Copy('Classes', 'h', 'Examples/Blink/STM32/Blink/Inc', 'h', 'pin toggle')
  Copy('Classes', 'c', 'Examples/Blink/STM32/Blink/Src', 'c', 'pin toggle')

  Copy('Classes', 'h c', 'Examples/Blink/RaspberryPi',                'h c',   'pin prt toggle')


  #
  # Encoder
  #
  Copy('Classes',       'h c', 'Examples/Encoder/Arduino/EncoderExample', 'h cpp', 'pin prt encoder')
  Copy('Classes/Tests', 'h c', 'Examples/Encoder/Arduino/EncoderExample', 'h cpp', 'encodertest', not IncludeOosmos)

  #
  # Keyer
  #
  Copy('Classes', 'h c', 'Examples/Keyer/Arduino/KeyerExample',     'h cpp', 'keyer pin prt')

  Copy('Classes', 'h', 'Examples/Keyer/STM32/Keyer/Inc',            'h', 'keyer pin toggle')
  Copy('Classes', 'c', 'Examples/Keyer/STM32/Keyer/Src',            'c', 'keyer pin toggle')

  Copy('Classes', 'h c', 'Examples/Keyer/MSP430/KeyerExample',      'h cpp', 'keyer pin prt')

  Copy('Classes', 'h c', 'Examples/Keyer/ChipKit/KeyerExample',     'h cpp', 'keyer pin')
  oosmos.CopyFileReadOnly('Examples/Keyer/Arduino/KeyerExample/KeyerExample.ino', 'Examples/Keyer/ChipKit/KeyerExample/KeyerExample.ino')


  #
  # LCD
  #
  Copy('Classes',       'h c', 'Examples/LCD/Arduino/LcdExample', 'h cpp', 'pin lcd')
  Copy('Classes/Tests', 'h c', 'Examples/LCD/Arduino/LcdExample', 'h cpp', 'lcdtest', not IncludeOosmos)

  #
  # Matrix
  #
  Copy('Classes', 'h c', 'Examples/Matrix/Arduino/MatrixExample',         'h cpp', 'pin sw matrix prt')
  Copy('Classes/Tests', 'h c', 'Examples/Matrix/Arduino/MatrixExample',   'h cpp', 'matrixtest')

  #
  # mcp4131
  #
  Copy('Classes',       'h c', 'Examples/mcp4131/Arduino/mcp4131Example', 'h cpp', 'spi mcp4131 pin')
  Copy('Classes/Tests', 'h c', 'Examples/mcp4131/Arduino/mcp4131Example', 'h cpp', 'mcp4131test', not IncludeOosmos)

  #
  # Switch
  #
  Copy('Classes', 'h c', 'Examples/Switch/Arduino/SwitchExample',       'h cpp', 'pin sw prt')
  Copy('Classes/Tests', 'h c', 'Examples/Switch/Arduino/SwitchExample', 'h cpp', 'switchtest', not IncludeOosmos)

  #
  # Thread
  #
  Copy('Classes',       'h c', 'Examples/Thread/Arduino/ThreadExample',  'h cpp', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/Thread/Arduino/ThreadExample',  'h cpp', 'threadtest')

IncludeOosmos = True

Main()
