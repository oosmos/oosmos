import subprocess

subprocess.run(r'..\Gen\gen.exe classes.json', check=True)

file_list_strings = """
pid.c
sock.c
dns.c
matrix.c
keyer.c
toggle.c
toggle_state.c
btn.c
btnph.c
encoder.c
accum.c
keypad.c
pin.c
reg.c
prt.c
tok.c
"""

file_list = [file.strip() for file in file_list_strings.strip().splitlines() if file.strip()]

# Prepare the compile command
compile_command = [
    'cl',
    '-W4',
    '-wd4204',
    '-wd4065',
    '-wd4100',
    '-wd4127',
    '-c',
    r'-I..\Source',
    '-D_CRT_SECURE_NO_WARNINGS',
    '-Doosmos_PIN_KEY_WINDOWS'
] + file_list

subprocess.run(compile_command, check=True)
