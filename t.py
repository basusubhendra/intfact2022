#!/usr/bin/python3
import sys
f = open("./e.txt","r")
g = open("./_e.txt","w")
f.read(2)
g.write("20")
while True:
    c = str(f.read(1))
    if not c or len(c) == 0 or c == "":
        break
    g.write(str(c))
g.close()
f.close()
