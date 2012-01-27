#!/usr/bin/env python

import darc, time, MyPublisherComponent

n = darc.Node.create()
n.setNodeID(120)
n.accept("udp://127.0.0.1:5120")
n.connect(125, "udp://127.0.0.1:5125")

c1 = n.instantiateComponent("MyPublisherComponent")

#c.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
