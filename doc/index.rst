DARC
======

Overview
--------------
DARC is a component based framework for building control systems.
It is written in C++ and has Python bindings.

DARC was created as a prototype for a new generation of the core library of `ROS <http://www.ros.org>`_ (a new version of `roscpp <http://ros.org/wiki/roscpp>`_ and `rospy <http://ros.org/wiki/rospy>`_), but it exists as a independent framework and can be used as such.
It uses the `ROS message files <http://www.ros.org/wiki/msg>`_ as data format, so it is often quite straightforward to update code written for ROS to use DARC instead.
The `Catkin <https://github.com/willowgarage/catkin>`_ build system introduced in ROS Fuerte allows ROS messages to be used seperately from ROS and thus allowed a system like DARC to be created.

Purpose
-------
DARC is designed to fix some of the limitations in ROS, resulting from inadequate design, and code difficult to maintain.
In addition, it strives to greatly improve the control you get over your system, and adds introspectrum and profiling options allowing you to get an overview of the performance of the running system (bottlenecks, etc).

Recently a lot of extra functionality have been built on top of ROS and the message passing system, such as:

* Actionlib
* Nodelets
* Pluginlib
* Dynamic Reconfigure
* Ros Serial
* Multimaster Systems

These sub-projects show some of the functionality and use-cases that people need, and ROS fundamentally lacks. But instead of fixing the underlying architecture (which of course would be a large effort), they have been overcome by creating workarounds, sacrificing performance, and by using a lot of code generators.

The fundamental design of DARC take all these use-cases into account. They are handled by design or on the system level, and thus none of the hacky workarounds mentioned above are needed in a DARC system.

Architecture
------------
Compared to ROS, DARC is more strict concerning the Component/Node concepts. This allows for more control, and designs easier to explain.

**Component**

A component is where the user implements his functionality.
One component defines the unit of functionality that can be loaded and deployed as whole.
The components communicate with each other using the available DARC communication primitives (publishers/subscribers/procedures etc).
A component is state controlled, it can be started, stopped, paused, and waiting for something.
It can be linked into a executable at compile time or loaded from a dynamic library at runtime.

**Node**

A node in DARC is a point of communication that links the components together.
Each component is associated and loaded into a single node, and the nodes are connected to eachother creating a routing graph.

**Boost ASIO**

The asyncronous architecture of the system is implemented using `Boost ASIO <www.boost.org/libs/asio>`_.
Data is exchanged using Boost Shared Pointers.
A component registers the callbacks using Boost Functions & Boost Bind, and is the owner of a Boost io_service.
The node posts events to the components by posting the callbacks in respective io_service.
Each component will have an associated thread servicing the respective io_service (it can also use several threads, bu that requires the callbacks to be designed as thread safe).

The node is also the owner of an io_service and a thread to service it, handling all the serialization, deserialization, and network communication.

The system is purely event based, and contains no spin loops in the threads.

**Primitives**

DARC primitives are used for the components to communicate with eachother, or to manage the functionality of each component.
The current version has:

* Pub/Sub
* Procedures
* Periodic Timers
* Parameters

A Procedure is the equivalent of a ROS action.
It is a asyncronous procedure call taking a ROS msg as argument, will return a ROS msg when it is completed, and in the meantime can send ROS msgs as status updates.
Since it is implemented on the system level (and not on top of publishers and subscribers), it is faster than ROS actions, and it doesnt require creating .action files.

**Network Topology**

The network is decentralized (No rosmaster required).
A node needs only to connect to at least one other node in the node network to be part of the running system.
The actual network topology is handled at the node level.
The user can specify how each node pair should communicate (UDP/TCP/Serial link/other), or he han ask it to discover the other nodes automatically (zeroconf). (Note: Current prototype only works for manually specified UDP links).
Each node pair can have several types of links open, e.g. UDP for exchanging small published messages, and TCP for excanging remote procedure calls depending on the QoS requirements (guaranteed/non-guaranteed delivery).

A node is running in a single process. All the components that are loaded into the respective node is therefore within the same process and can communicate with eachother using shared memory (boost shared pointes).

If two components in different nodes need to communicate, the nodes will serialize the message, send it over their transport link, deserialize it again, and dispatch it to all the target components in the respective node as a shared pointer.
A published message is thus serialized and transported only once, even if there are several remote subscribers.

.. graphviz::

   digraph network
   {
     subgraph cluster_0
     {
       style=filled;
       color=lightgrey;
       node [style=filled,color=white];
       label = "process #1";
       "Node1" -> "C1" [dir=both];
       "Node1" -> "C2" [dir=both];
       "Node1" -> "C3" [dir=both];
     }
     subgraph cluster_1
     {
       style=filled;
       color=lightgrey;
       node [style=filled,color=white];
       label = "process #2";
       "Node2" -> "C4" [dir=both];
       "Node2" -> "C5" [dir=both];
       "Node2" -> "C6" [dir=both];
     }
     "Node1" -> "Node2" [dir=both];
   }

Examples TBD
------------

Source
------

* `DARC library <https://github.com/mkjaergaard/darc>`_
* `DARC examples <https://github.com/mkjaergaard/darc_examples>`_
* `DARC/ROS bridge <https://github.com/mkjaergaard/darc_ros>`_
