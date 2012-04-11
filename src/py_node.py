#!/usr/bin/env python

import darc
import sys

darc.Log.logToFile("out.log")

n = darc.Node.create()
n.accept("udp://127.0.0.1:5120-5130")
n.connect("udp://127.0.0.1:5120-5130")

for comp in sys.argv[1:]:
    darc.ComponentLoader.loadComponent("lib"+comp+".so")
    c = n.instantiateComponent(comp)
    c.run()

n.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
