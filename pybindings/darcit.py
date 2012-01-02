#!/usr/bin/env python


import darc, time

thing = darc.Thing()

thing.start()
time.sleep(3)
thing.stop()
print "sleeping... thing should stop"
thing.wait()
print "Done waiting, threads joined."

from IPython.Shell import IPShellEmbed
ipshell = IPShellEmbed()
ipshell()
