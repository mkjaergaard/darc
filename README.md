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

Architecture Overview
---------------------
### Peer Layer:
A DARC control system is designed to run as a fully decentralized peer2peer system. (no rosmaster, no central parameter server etc.)
The network communication is abstracted by the peer layer.
A peer normally maps into a single executable.
Peers connect to eachother, either manually (or at some point based on zeroconf), and form a routable network.
Each peer is given a unique 16-byte identifier (boost::uuid).

The interface to the peer layer is basically a send_to(dest_peer_id, some_data) function and a few callbacks in case of data received, new peers connected etc.

### Services Layer:
The peer layer is utilized by a number of DARC services running on each peer e.g:
* Distributed Nameserver Service
* Publisher/Subscribe Service
* Procedure Service

A service instance communicates with other instances of the same service on other peers.

**Nameserver Service**
The nameserver is used to identify ressources (e.g. a topic, a parameter or a procedure) by a name.
And be notified when other peers provides or needs a resource.
Names are not remapped (like in ROS), but instead resourced with different names are 'matched' by adding aliases, normally in a parent namespace.
Normally there is no concept of a root namespace, only the 'my current namespace' and 'my parent namespace'.
This means namespaces can extend in either direction to allow for multi robot systems.
The details requires a longer explaination, and the implementaion is not really working yet, so dont worry about the nameserver right now.

**Publisher/Subscriber Service**
Message broker supporting the publisher/subscribe pattern.
Build on boost::asio for asynchronous callbacks handled by different thread.
The current version utilizes zero-copy transport of data when the publisher and susbcriber is attached to the same peer.
When publishers and subscribers are on seperate peers the data is serialized and sent throught the peer layer.

### Component Layer
The component layer wraps the peer and services layers in a more user friendly interface.

**Components**

User functionality is implemented in a class extending the darc::component class.

Example of the talker_component.cpp
```cpp
#include <iris/static_scope.hpp>
#include <darc/component.hpp>
#include <darc/periodic_timer.hpp>
#include <darc/publisher.h>

class talker_component : public darc::component, public iris::static_scope<iris::Info>
{
  int count_;
  darc::periodic_timer timer_;
  darc::publisher<std::string> chatter_pub_;

  void timer_callback()
  {
    boost::shared_ptr<std::string> msg = boost::make_shared<std::string>();

    *msg = std::string("Hello World ").append(boost::to_string(++count_));

    slog<iris::Info>("Publishing",
  	     "Msg", iris::arg<std::string>(*msg));

    chatter_pub_.publish(msg);
  }

public:
  talker_component() :
    count_(0),
    timer_(this, &talker_component::timer_callback, boost::posix_time::seconds(1)),
    chatter_pub_(this, "chatter")
  {
  }

};

DARC_REGISTER_COMPONENT(talker_component)
```

Notice:
* There is no spin loop. The component contains only event callbacks.
* The macro DARC_REGISTER_COMPONENT registers the component in the darc::registry which makes it possible to instatiate it.

The corresponding lister_component.cpp

```cpp
#include <iris/static_scope.hpp>
#include <darc/component.hpp>
#include <darc/subscriber.h>

class listener_component : public darc::component, public iris::static_scope<iris::Info>
{
  darc::subscriber<std::string> sub_;

  void chatter_callback(const boost::shared_ptr<const std::string> msg)
  {
    slog<iris::Info>("Received",
  	     "msg", iris::arg<std::string>(*msg));
  }

public:
  listener_component() :
    sub_(this, "chatter", boost::bind(&listener_component::chatter_callback, this, _1))
  {
  }

};

DARC_REGISTER_COMPONENT(listener_component)
```

**Component Manager**
To actually run components an instance of darc::component_manager must be created.
The component_manager wraps the peer and services layers.
The use case usually in an executables main() function is to:
* Instantiate the darc::component_manager
* Open connections to other component_managers (peers)
* Load components from .so files
* Instantiate and run components

The component_manager has been wrapped in python using boost::python so it is possible to start components throgh python.

Example of a script that loads and starts the listener_component (listener_peer.py):

```python
import darc

# Create component_manager                                                                                                                                                                                                                                                     
mngr = darc.component_manager()

# Load the Listener component                                                                                                                                                                                                                                                  
darc.component_loader.load_component("liblistener_component.so")

# Instantiate the Listener component                                                                                                                                                                                                                                           
c = darc.registry.instantiate_component("listener_component", mngr)

# Run the Listener component                                                                                                                                                                                                                                                   
c.run()

# Manually add connections to other peers in the 5000-5009 port range                                                                                                                                                                                                          
mngr.connect("zmq+tcp://127.0.0.1:5000")
mngr.connect("zmq+tcp://127.0.0.1:5001")
mngr.connect("zmq+tcp://127.0.0.1:5002")
mngr.connect("zmq+tcp://127.0.0.1:5003")
mngr.connect("zmq+tcp://127.0.0.1:5004")
mngr.connect("zmq+tcp://127.0.0.1:5005")
mngr.connect("zmq+tcp://127.0.0.1:5006")
mngr.connect("zmq+tcp://127.0.0.1:5007")
mngr.connect("zmq+tcp://127.0.0.1:5008")
mngr.accept("zmq+tcp://127.0.0.1:5009")

# Run the component_manager in the python thread                                                                                                                                                                                                                               
mngr.run_current_thread()
```


Brief overview of packages:
---------------------------
### darc_common:
Contains basic stuff such as the implementaion of peers that can be connected through network and form a routable system.
On top of the peers runs a number of services that can utilize the routable network.
Also contains a generic way to use your own serializer method, e.g. (ROS, boost, protobuf etc...).

### darc_ns:
Contains a service for decentralized name lookups.

### darc_pubsub:
A service for publisher/subscribe. Intra peer communication will use zero-copy shared pointers. Data between peers will be serialized.
It is transparent to the publishers & subscribers whether they communicate with publishers & subscribers on other peers or on the same peer.

### darc_procedure:
Initial impl of for procedures for both short request (similar to ros services) and longer running preemptible tasks (similar to ros actions).
Again fast zero-copy for intra peer stuff, and transparent serialization between peers

### darc_component:
The other packages are basically raw services dcesigned to work similar as a boost io_service.
The component package provides a component model for the user and wraps the pub/sub etc. in a more used friendly way and provides threading control, state control etc.
Components are intended to be compiled into a dynamic library that can be deployed (loaded dynamically) on peer.

## darc
Documentation

### darc_examples:
Contain examples of talker and listener components.
