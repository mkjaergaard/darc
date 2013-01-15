darc
====

Summary
-------
DARC 0.2 is the second round prototype of a ROS-like transportation layer.
It is compatible with ROS-messages and use the catkin build system, thus integrated in the ROS ecosystem.

The 0.1-prototype branch contains the the first prototype written during my internship at WG in 2011/2012.
https://github.com/mkjaergaard/darc/tree/0.1-prototype

Version 0.2 has been redesigned in several ways such as:
* The implementaion have been split into several sub-projects to motivate a seperated architecture
* Nodes have been reaplaced by Peers, to better symbolize the decentralized p2p structure
* A decentralized name service
* General architecture cleanup
