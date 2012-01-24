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
Compared to ROS, DARC is more strict concerningthe Component/Node concepts. This allows for more control, and designs easier to explain.

**Component**

A control system in DARC is built up by a series of components.
A component defines the boundary of the unit of functionality that can be loaded, deployed and start/stopped.
The components communicate with each other using the available DARC communication primitives (publishers/subscribers/procedures etc).

**Node**

A node in DARC is a point of communication that links the components together.
Each component is associated and loaded into a single node, and the nodes are connected to eachother creating a routing graph.

**Threading**

A node is running in a single process. All the components that are loaded into the respective node is therefore within the same process and can communicate with eachother using shared memory (boost shared pointes).

Howto
-----

Source
------

* `DARC library <https://github.com/mkjaergaard/darc>`_
* `DARC examples <https://github.com/mkjaergaard/darc_examples>`_
* `DARC/ROS bridge <https://github.com/mkjaergaard/darc_ros>`_
