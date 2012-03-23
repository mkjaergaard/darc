#!/usr/bin/env python

import darc, MyPublisherComponent

darc.Log.logToFile("out.log")

n = darc.Node.create()
c = n.instantiateComponent("MyPublisherComponent")

n.accept("udp://127.0.0.1:5120-5125")
n.connect("udp://127.0.0.1:5120-5125")

c.run()
n.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
