import sys

sys.path.append('../../..')
import oosmos

oosmos.cWindows.Compile('servertest.c server.c sock.c listener.c', 'Ws2_32.lib')
oosmos.cWindows.Compile('clienttest.c client.c sock.c',            'Ws2_32.lib')
