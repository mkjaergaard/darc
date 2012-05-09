#!/usr/bin/env python

import darc

darc.Log.logToFile("out.log")

n = darc.Node.create()

#n.accept("udp://127.0.0.1:5120-5125")
#n.connect("udp://127.0.0.1:5120-5125")

n.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
