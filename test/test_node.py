#!/usr/bin/env python

import darc, time, publisher_component

n = darc.Node.create()
c = darc.Registry.instantiateComponent("MyPublisherComponent", n)

n.setNodeID(120)
#n.accept("udp://127.0.0.1:5120")
#n.connect(125, "udp://127.0.0.1:5125")

#c.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
