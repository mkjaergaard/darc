darc
====

Summary
-------
DARC 0.2 is the second round prototype of a ROS-like transportation layer.
It is compatible with ROS-messages and use the catkin build system, thus integrated in the ROS ecosystem.

The 0.1-prototype branch contains the the first prototype written during my internship at WG in 2011/2012.
https://github.com/mkjaergaard/darc/tree/0.1-prototype
https://github.com/mkjaergaard/darc_examples/tree/0.1-prototype

Version 0.2 has been redesigned in several ways such as:
* The implementaion have been split into several sub-projects to motivate a higly seperated architecture
* Nodes have been reaplaced by Peers, to better symbolize their purpose in the p2p system
* A decentralized name service
* General architecture cleanup

Brief overview of packages:
darc_common:
Contains basic stuff such as the implementaion of peers that can be connected through network and form a routable system.
On top of the peers runs a number of services that can utilize the routable network.
Also contains a generic way to use your own serializer method, e.g. (ROS, boost, protobuf etc...).

darc_ns:
Contains a service for decentralized name lookups.

darc_pubsub:
A service for publisher/subscribe. Intra peer communication will use zero-copy shared pointers. Data between peers will be serialized.
It is transparent to the publishers & subscribers whether they communicate with publishers & subscribers on other peers or on the same peer.

darc_procedure:
Initial impl of for procedures for both short request (similar to ros services) and longer running preemptible tasks (similar to ros actions).
Again fast zero-copy for intra peer stuff, and transparent serialization between peers

darc_component:
The other packages are basically raw services dcesigned to work similar as a boost io_service.
The component package provides a component model for the user and wraps the pub/sub etc. in a more used friendly way and provides threading control, state control etc.

Version 0.2 status:
* The python bindings and ipython console in 0.1 has not been ported to 0.2 yet. 0.2 is intended to be easier to extend with language bindings since the 'darc_component' package mainly contains wrappers that can be re-implemented to give a natural feeling for any new language.

