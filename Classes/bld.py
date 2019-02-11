import os

os.system(r'..\Gen\gen.exe classes.json')
os.system(r'cl -W4 -c -D_CRT_SECURE_NO_WARNINGS sock.c dns.c matrix_state.c matrix.c keyer.c toggle.c toggle_state.c accum.c keypad.c pin.c reg.c prt.c tok.c -I..\source')
