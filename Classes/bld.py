import os

os.system(r'..\Gen\gen.exe classes.json')
os.system(r'cl -W3 -c matrix_state.c keyer.c toggle.c toggle_state.c accum.c keypad.c  -I..\source')
