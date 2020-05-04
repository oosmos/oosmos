import os

os.system(r'..\Gen\gen.exe classes.json')
os.system(r'cl -W4 -wd4204 -wd4065 -wd4100 -wd4127 -c -I..\Source -D_CRT_SECURE_NO_WARNINGS pid.c sock.c dns.c matrix.c keyer.c toggle.c toggle_state.c btn.c btnph.c encoder.c accum.c keypad.c pin.c reg.c prt.c tok.c')
#os.system(r'CLang-cl -W4 -wd4204 -wd4065 -wd4100 -wd4127 -c -D_CRT_SECURE_NO_WARNINGS pid.c sock.c dns.c matrix.c keyer.c toggle.c toggle_state.c btn.c btnph.c encoder.c accum.c keypad.c pin.c reg.c prt.c tok.c -I..\source')
