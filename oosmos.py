#
#  oosmos.py - A set of common python functions.
#
import sys
import shutil
import subprocess
import os
import glob

def WalkDir(Dir, pFunc, UserArg):
  for RootDir, DirList, FileList in os.walk(Dir):
    for File in FileList:
      FullFilePath = os.path.join(RootDir, File)
      pFunc(os.path.normpath(FullFilePath), UserArg)

    for Dir in DirList:
      WalkDir(Dir, pFunc, UserArg)


def Clean(Dir, Extensions):
  Extensions = Extensions.split()

  for SubDir, Dirs, Files in os.walk(Dir):
    if 'dist' in Dirs:
      shutil.rmtree(SubDir+'/dist')

    if 'build' in Dirs:
      shutil.rmtree(SubDir+'/build')

    if SubDir == 'dist' or SubDir == 'build':
      print('remove '+SubDir)
      continue

    for File in Files:
      for Extension in Extensions:
        bExtension = Extension.encode('ascii')
        bFile      = File.encode('ascii')

        if bFile.endswith((b'.'+bExtension,)):
          os.remove(SubDir+'/'+File)

class cWindows:
  @staticmethod
  def Clean():
    WildRemove('*.exe')
    WildRemove('*.pdb')
    WildRemove('*.obj')
    WildRemove('*.ilk')
    WildRemove('*.suo')
    WildRemove('*.tds')
    WildRemove('*.bak')

  @staticmethod
  def Compile(FileName, Options = ''):
    print('Compiling '+FileName+'...')

    try :
      p = subprocess.Popen('cl /nologo /Zi /W4 /wd4065 /wd4100 /wd4127 /D_CRT_SECURE_NO_WARNINGS '+FileName+' oosmos.c '+Options+' -Doosmos_DEBUG',
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd = os.getcwd())
    except:
      print("\n*** Unable to compile. Is Visual Studio installed?")
      sys.exit(16)
  
    for Line in p.stdout:
      Line = Line.rstrip()
  
      if Line.startswith((b'reg', b'Generating Code...', b'Compiling...')):
        continue
  
      if Line.endswith((b'.c', b'.cpp')):
        continue
  
      print(Line)

class cLinux:
  @staticmethod
  def Compile(Target, Files):
    print('Compiling %s...' % (Files))
    os.system("gcc -Wall -o %s -Doosmos_DEBUG -Doosmos_ORTHO %s oosmos.c " % (Target, Files))

def WildRemove(FilenamePattern):
  FileList = glob.glob(FilenamePattern)

  for FileName in FileList:
    os.remove(FileName)

def MakeReadWrite(Filename):
  os.chmod(Filename, 0o777)

def MakeReadOnly(Filename):
  os.chmod(Filename, 0o444)

def CopyFileReadOnly(FromFile, ToFile):
  if os.path.exists(ToFile):
    MakeReadWrite(ToFile)

  shutil.copyfile(FromFile, ToFile)
  MakeReadOnly(ToFile)

if __name__ == '__main__':
  print("'oosmos.py' is a module of reusable scripting elements and is not a standalone script.")
