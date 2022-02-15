#
#  oosmos.py - A set of common python functions.
#
import sys
import shutil
import subprocess
import os
import glob

from collections.abc import Callable

def Path(P: str) -> str:
	return os.path.normpath(P)

def Join(Array: list[str]) -> str:
	return ' '.join(Array)

def WalkDir(Dir: str, pFunc: Callable[[str, str], str], UserArg: str) -> None:
	for RootDir, DirList, FileList in os.walk(Dir):
		for File in FileList:
			FullFilePath = os.path.join(RootDir, File)
			pFunc(os.path.normpath(FullFilePath), UserArg)

		for Dir in DirList:
			WalkDir(Dir, pFunc, UserArg)


def Clean(Dir: str, ExtensionsArg: str) -> None:
	Extensions = ExtensionsArg.split()

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
	def Clean() -> None:
		WildRemove('*.exp')
		WildRemove('*.lib')
		WildRemove('*.exe')
		WildRemove('*.pdb')
		WildRemove('*.obj')
		WildRemove('*.pch')
		WildRemove('*.ilk')
		WildRemove('*.suo')
		WildRemove('*.tds')
		WildRemove('*.bak')

	@staticmethod
	def Compile(oosmos_dir: str, FileArray: list[str], Options: str = '') -> None:
		print('Compiling...')

		Files = ' '.join(FileArray)
		Line = r'cl -I. -I%s\Source -I%s\Classes -I%s\Classes\Tests -nologo -Zi -W4 -wd4204 -wd4065 -wd4100 -wd4127 -D_CRT_SECURE_NO_WARNINGS '%(oosmos_dir,oosmos_dir,oosmos_dir)+Files+' -Doosmos_DEBUG '+Options
		#Line = r'CLang-cl -I. -I%s\Source -I%s\Classes -I%s\Classes\Tests -nologo -Zi -W4 -wd4204 -wd4065 -wd4100 -wd4127 -D_CRT_SECURE_NO_WARNINGS '%(oosmos_dir,oosmos_dir,oosmos_dir)+Files+' -Doosmos_DEBUG '+Options
		#Line = r'CLang-tidy ' + Files + ' -checks=*,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-readability-magic-numbers,-cppcoreguidelines-avoid-magic-numbers,-google-readability-avoid-underscore-in-googletest-name -- -I. -I%s\Source -I%s\Classes -I%s\Classes\Tests -c -D_CRT_SECURE_NO_WARNINGS '%(oosmos_dir,oosmos_dir,oosmos_dir)+' -Doosmos_DEBUG '+Options
		# To enable asan, add /fsanitize=address /MT clang_rt.asan-i386.lib immediately after cl. Requires VS 2017.
		print(Line)

		try :
			p = subprocess.Popen(Line, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd = os.getcwd())
		except:
			print("\n*** Unable to compile. Is Visual Studio installed?")
			sys.exit(16)

		if p.stdout is not None:
			for Line in p.stdout:
				Line = Line.rstrip()

				if Line.startswith((b'reg', b'Generating Code...', b'Compiling...')):
					continue

				if Line.endswith((b'.c', b'.cpp')):
					continue

				print(Line.decode('utf-8'))

class cLinux:
	@staticmethod
	def Compile(oosmos_dir: str, Target: str, FileArray: list[str], Options: str = '') -> None:
		Files = ' '.join(FileArray)
		print(f'Compiling {Target}...')

		classes_dir = os.path.normpath(oosmos_dir+'/Classes')
		Line = "gcc -I%s/Source -I%s -I%s/Tests -I. -std=c99 -Wall -Wno-overflow -Wno-unused-parameter -pedantic -Werror -Wshadow -flto -o %s -D_POSIX_C_SOURCE=199309 -D__linux__ -Doosmos_DEBUG -Doosmos_ORTHO %s " % (oosmos_dir, classes_dir, classes_dir, Target, Files) + Options
		os.system(Line)

def WildRemove(FilenamePattern: str) -> None:
	FileList = glob.glob(FilenamePattern)

	for FileName in FileList:
		os.remove(FileName)

def MakeReadWrite(Filename: str) -> None:
	os.chmod(Filename, 0o777)

def MakeReadOnly(Filename: str) -> None:
	os.chmod(Filename, 0o444)

def CopyFileReadOnly(FromFile: str, ToFile: str) -> None:
	if os.path.exists(ToFile):
		MakeReadWrite(ToFile)

	shutil.copyfile(FromFile, ToFile)
	MakeReadOnly(ToFile)

if __name__ == '__main__':
	print("'oosmos.py' is a module of reusable scripting elements and is not a standalone script.")
