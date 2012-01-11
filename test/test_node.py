#!/usr/bin/env python

import darc, time, publisher_component

n_ = darc.Node.create()
n_.setNodeID(120)

n = darc.Node_(n_)

#c = darc.Registry.instantiateComponent("MyPublisherComponent", n)

#n.accept("udp://127.0.0.1:5120")
#n.connect(125, "udp://127.0.0.1:5125")

#c.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
