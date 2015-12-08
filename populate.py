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
import os
import glob
import platform
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
  
  #Copy('src_obfuscateed', 'h c', 'oosmos', 'h c', 'oosmos', not IncludeOosmos)
  #Copy('src',             'h c', 'oosmos', 'h c', 'oosmos', not IncludeOosmos)

  #
  # AnalogMap
  #
  Copy('',       'h c', 'Examples/AnalogMap/Windows',            'h c', '')

  Copy('',       'h c', 'Examples/AnalogMap/Linux',              'h c', '')
  oosmos.CopyFileReadOnly('Examples/AnalogMap/Windows/AnalogMap.c', 'Examples/AnalogMap/Linux/AnalogMap.c')

  #
  # Blink
  #
  Copy('Classes', 'h c', 'Examples/Blink/Arduino/BlinkExample',       'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/ESP8266/BlinkExample',       'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/ChipKit/BlinkExample',       'h cpp', 'pin toggle')
  oosmos.CopyFileReadOnly('Examples/Blink/Arduino/BlinkExample/BlinkExample.ino', 'Examples/Blink/ChipKit/BlinkExample/BlinkExample.ino')
  Copy('Classes', 'h c', 'Examples/Blink/Galileo/BlinkExample',       'h cpp', 'pin toggle')
  oosmos.CopyFileReadOnly('Examples/Blink/Arduino/BlinkExample/BlinkExample.ino', 'Examples/Blink/Galileo/BlinkExample/BlinkExample.ino')
  Copy('Classes', 'h c', 'Examples/Blink/mbed',                       'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/MSP430/BlinkExample',        'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/LightBlueBean/BlinkExample', 'h cpp', 'pin toggle')
  Copy('Classes', 'h c', 'Examples/Blink/PIC32/BlinkExample.X',       'h c',   'pin toggle')

  Copy('Classes', 'h c', 'Examples/Blink/RaspberryPi',                'h c',   'pin prt toggle')


  #
  # Encoder
  #
  Copy('Classes',       'h c', 'Examples/Encoder/Arduino/EncoderExample', 'h cpp', 'pin prt encoder')
  Copy('Classes/Tests', 'h c', 'Examples/Encoder/Arduino/EncoderExample', 'h cpp', 'encodertest', not IncludeOosmos)

  #
  # EventDemo
  #
  Copy('',                'h c', 'Examples/EventDemo/Windows',        'h c', '')
  Copy('Classes/Windows', 'h c', 'Examples/EventDemo/Windows',        'h c', 'key')

  #
  # Interrupt
  #
  Copy('',       'h c', 'Examples/Interrupt/Windows',              'h c', '')

  #
  # Keyer
  #
  Copy('Classes', 'h c', 'Examples/Keyer/Arduino/KeyerExample',     'h cpp', 'keyer pin prt')

  Copy('Classes', 'h c', 'Examples/Keyer/MSP430/KeyerExample',      'h cpp', 'keyer pin prt')

  Copy('Classes', 'h c', 'Examples/Keyer/ChipKit/KeyerExample',     'h cpp', 'keyer pin')
  oosmos.CopyFileReadOnly('Examples/Keyer/Arduino/KeyerExample/KeyerExample.ino', 'Examples/Keyer/ChipKit/KeyerExample/KeyerExample.ino')

  Copy('Classes', 'h c', 'Examples/Keyer/Galileo/KeyerExample',     'h cpp', 'keyer pin')
  oosmos.CopyFileReadOnly('Examples/Keyer/Arduino/KeyerExample/KeyerExample.ino', 'Examples/Keyer/Galileo/KeyerExample/KeyerExample.ino')

  Copy('Classes', 'h c', 'Examples/Keyer/PIC32/KeyerExample.X',     'h c', 'keyer pin prt')

  #
  # LCD
  # 
  Copy('Classes',       'h c', 'Examples/LCD/Arduino/LcdExample', 'h cpp', 'pin lcd')
  Copy('Classes/Tests', 'h c', 'Examples/LCD/Arduino/LcdExample', 'h cpp', 'lcdtest', not IncludeOosmos)

  Copy('Classes',       'h c', 'Examples/LCD/PIC32/LcdExample.X', 'h c', 'pin lcd')
  Copy('Classes/Tests', 'h c', 'Examples/LCD/PIC32/LcdExample.X', 'h c', 'lcdtest', not IncludeOosmos)

  #
  # LinearRegression
  #
  Copy('Classes',       'h c', 'Examples/LinearRegression/Windows',   'h c', 'reg prt')
  Copy('Classes/Tests', 'h c', 'Examples/LinearRegression/Windows',   'h c', 'regtest', not IncludeOosmos)

  Copy('Classes',       'h c', 'Examples/LinearRegression/Linux',     'h c', 'reg prt')
  Copy('Classes/Tests', 'h c', 'Examples/LinearRegression/Linux',     'h c', 'regtest', not IncludeOosmos)


  #
  # Matrix
  #
  Copy('Classes', 'h c', 'Examples/Matrix/Arduino/MatrixExample',         'h cpp', 'pin sw matrix prt')
  Copy('Classes/Tests', 'h c', 'Examples/Matrix/Arduino/MatrixExample',   'h cpp', 'matrixtest')

  Copy('Classes', 'h c', 'Examples/Matrix/PIC32/MatrixExample.X',         'h c', 'pin sw matrix prt')
  Copy('Classes/Tests', 'h c', 'Examples/Matrix/PIC32/MatrixExample.X',   'h c', 'matrixtest')

  #
  # mcp4131
  #
  Copy('Classes',       'h c', 'Examples/mcp4131/Arduino/mcp4131Example', 'h cpp', 'spi mcp4131 pin')
  Copy('Classes/Tests', 'h c', 'Examples/mcp4131/Arduino/mcp4131Example', 'h cpp', 'mcp4131test', not IncludeOosmos)

  Copy('Classes',       'h c', 'Examples/mcp4131/PIC32/mcp4131.X',        'h c',   'spi mcp4131 pin')
  Copy('Classes/Tests', 'h c', 'Examples/mcp4131/PIC32/mcp4131.X',        'h c',   'mcp4131test', not IncludeOosmos)

  #
  # Ortho
  #
  Copy('Classes', 'h c', 'Examples/Ortho/Windows',       'h c', '')

  Copy('Classes', 'h c', 'Examples/Ortho/Linux',         'h c', '')
  oosmos.CopyFileReadOnly('Examples/Ortho/Windows/Completion.c', 'Examples/Ortho/Linux/Completion.c')
  oosmos.CopyFileReadOnly('Examples/Ortho/Windows/EnterExit.c',  'Examples/Ortho/Linux/EnterExit.c')
  oosmos.CopyFileReadOnly('Examples/Ortho/Windows/Motion.c',     'Examples/Ortho/Linux/Motion.c')

  #
  # Basic
  #
  Copy('Classes', 'h c', 'Examples/Basic/Windows',       'h c', '')

  Copy('Classes', 'h c', 'Examples/Basic/Linux',         'h c', '')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/Final.c',            'Examples/Basic/Linux/Final.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/DelayUS.c',          'Examples/Basic/Linux/DelayUS.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/DelayMS.c',          'Examples/Basic/Linux/DelayMS.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/DelaySeconds.c',     'Examples/Basic/Linux/DelaySeconds.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/TimeoutInUS.c',      'Examples/Basic/Linux/TimeoutInUS.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/TimeoutInMS.c',      'Examples/Basic/Linux/TimeoutInMS.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/TimeoutInSeconds.c', 'Examples/Basic/Linux/TimeoutInSeconds.c')
  oosmos.CopyFileReadOnly('Examples/Basic/Windows/StateTimeout.c',     'Examples/Basic/Linux/StateTimeout.c')

  #
  # Sockets
  #
  Copy('Classes',               'h c', 'Examples/Sockets/Windows',  'h c', 'sock')
  Copy('Classes/Tests/Sockets', 'h c', 'Examples/Sockets/Windows',  'h c', 'client server listener', not IncludeOosmos)

  Copy('Classes',               'h c', 'Examples/Sockets/Linux',  'h c', 'sock')
  Copy('Classes/Tests/Sockets', 'h c', 'Examples/Sockets/Linux',  'h c', 'client server listener', not IncludeOosmos)
  oosmos.CopyFileReadOnly('Examples/Sockets/Windows/clienttest.c', 'Examples/Sockets/Linux/clienttest.c')
  oosmos.CopyFileReadOnly('Examples/Sockets/Windows/servertest.c', 'Examples/Sockets/Linux/servertest.c')

  #
  # Switch
  #
  Copy('Classes', 'h c', 'Examples/Switch/Arduino/SwitchExample',       'h cpp', 'pin sw prt')
  Copy('Classes/Tests', 'h c', 'Examples/Switch/Arduino/SwitchExample', 'h cpp', 'switchtest', not IncludeOosmos)

  Copy('Classes', 'h c', 'Examples/Switch/PIC32/SwitchExample.X',       'h c', 'pin sw prt')
  Copy('Classes/Tests', 'h c', 'Examples/Switch/PIC32/SwitchExample.X', 'h c', 'switchtest', not IncludeOosmos)

  #
  # Sync
  #
  Copy('Classes',       'h c', 'Examples/Sync/Windows',              'h c', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/Sync/Windows',              'h c', 'synctest')

  Copy('Classes',       'h c', 'Examples/Sync/Arduino/SyncExample',  'h cpp', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/Sync/Arduino/SyncExample',  'h cpp', 'synctest')

  Copy('Classes',       'h c', 'Examples/Sync/PIC32/SyncExample.X',  'h c', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/Sync/PIC32/SyncExample.X',  'h c', 'synctest')

  Copy('Classes',       'h c', 'Examples/Sync/Linux',              'h c', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/Sync/Linux',              'h c', 'synctest')

  #
  # SyncYield
  #
  Copy('Classes',       'h c', 'Examples/SyncYield/Windows',         'h c', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/SyncYield/Windows',         'h c', 'syncyieldtest')

  Copy('Classes',       'h c', 'Examples/SyncYield/Linux',           'h c', 'prt')
  Copy('Classes/Tests', 'h c', 'Examples/SyncYield/Linux',           'h c', 'syncyieldtest')
  oosmos.CopyFileReadOnly('Examples/SyncYield/Windows/main.c', 'Examples/SyncYield/Linux/main.c')

  #
  # UART
  #
  Copy('Classes/PIC32', 'h c', 'Examples/UART/PIC32/UartExample.X',              'h c', 'uart', IncludeOosmos)


  
IncludeOosmos = True

Main()
