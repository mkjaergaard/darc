#!/usr/bin/env python

import darc, time, publisher_component

n = darc.Node.create()
c = darc.ComponentRegister.instantiateComponent("MyPublisherComponent", n)
#c.run()

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
