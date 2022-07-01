#!/usr/bin/python3
import sys
from zeros import zeros
g=open("./_zeros.hpp", "w")
g.write("int zeros[1000] = {")
for x in range (0, 1000):
    if x in zeros:
        g.write("1,")
    else:
        g.write("0,")
g.write("};")
g.close()
