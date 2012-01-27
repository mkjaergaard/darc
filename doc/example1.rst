.. _example1:

Creating Components in C++
======

A component is created by deriving from darc::Component, and must implement a certain contructor signature.
Since the system is event based there is no main loop or similar run method in the component.
Periodic events can be triggered by creating a periodic timer.

The ``DARC_REGISTER_COMPONENT`` registers the respective c++ class as a darc component. It  makes it possible by the system to instantiate and run the component based on the name.

publisher_component.cpp
-----------------------
The following example shows a component publishing a message every 1 second:

.. literalinclude:: code/publisher_component.cpp
   :language: c++
   :linenos:

subscriber_component.cpp
-----------------------
The following example shows a component subscribing to the message:

.. literalinclude:: code/subscriber_component.cpp
   :language: c++
   :linenos:

Special Cases:
--------------
The above examples show the standard way of creating a component, and the one you would normally use.
In special cases the user might need to have a different constructor signature, run extra threads, accept arguments from the command line etc.
The system is designed so that the default behavior can be overridden to handle the special cases, while keeping the stadard way as simple as possible.
