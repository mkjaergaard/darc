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
* Nodes have been replaced by Peers, to better symbolize their purpose in the p2p system
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

A component is normally compiled into a dynamic library.
```cmake
cmake_minimum_required(VERSION 2.8)
project(darc_examples)

find_package(catkin)

find_package(catkin REQUIRED COMPONENTS darc_component)
include_directories(${catkin_INCLUDE_DIRS})

add_library(talker_component SHARED
  src/talker_component.cpp)
target_link_libraries(talker_component ${catkin_LIBRARIES})

add_library(listener_component SHARED
  src/listener_component.cpp)
target_link_libraries(listener_component ${catkin_LIBRARIES})
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

Running the examples:
---------------------
Download code:
```console
mkdir darc_ws
cd darc_ws
rosinstall . --catkin https://raw.github.com/mkjaergaard/darc/master/share/darc.rosinstall
```

Configure:
```console
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=/opt/ros/groovy
```
Note: The ROS path is required since DARC supports ros serialization.

Build:
```console
make -j8
make pybindings
```

Setup Paths:
```console
source ./devel/setup.sh
```

Run Component Manager:
```console
./devel/lib/darc_component/darc_component_manager
```

So nothing really happens since we dont have any running components:

```console
[INFO  13:45:41.596249] Darc ComponentManager Running
[INFO  13:45:41.600465] ZeroMQ accept [URL:tcp://127.0.0.1:5000]
[INFO  13:45:41.601049] ZeroMQ connect [URL:tcp://127.0.0.1:5001] [Out-ID:6d52d5dc]
[INFO  13:45:41.601673] ZeroMQ connect [URL:tcp://127.0.0.1:5002] [Out-ID:8283b4c0]
[INFO  13:45:41.601909] ZeroMQ connect [URL:tcp://127.0.0.1:5003] [Out-ID:8aa83778]
[INFO  13:45:41.602153] ZeroMQ connect [URL:tcp://127.0.0.1:5004] [Out-ID:881cfc43]
[INFO  13:45:41.602390] ZeroMQ connect [URL:tcp://127.0.0.1:5005] [Out-ID:17184999]
[INFO  13:45:41.602631] ZeroMQ connect [URL:tcp://127.0.0.1:5006] [Out-ID:8570997f]
[INFO  13:45:41.602863] ZeroMQ connect [URL:tcp://127.0.0.1:5007] [Out-ID:e333ce40]
[INFO  13:45:41.603098] ZeroMQ connect [URL:tcp://127.0.0.1:5008] [Out-ID:d154002b]
[INFO  13:45:41.603341] ZeroMQ connect [URL:tcp://127.0.0.1:5009] [Out-ID:32c5f209]
No component to load
[INFO  13:45:41.603478] Running component_manager
```
CTRL+C to stop

The example executable can load and run components specified with -l:
```console
./devel/lib/darc_component/darc_component_manager -l talker_component -l listener_component
```

Now we have the components running:

```console
[INFO  13:48:10.104477] Darc ComponentManager Running
[INFO  13:48:10.105511] ZeroMQ accept [URL:tcp://127.0.0.1:5000]
[INFO  13:48:10.106011] ZeroMQ connect [URL:tcp://127.0.0.1:5001] [Out-ID:34d9d32f]
[INFO  13:48:10.106495] ZeroMQ connect [URL:tcp://127.0.0.1:5002] [Out-ID:78706adb]
[INFO  13:48:10.106655] ZeroMQ connect [URL:tcp://127.0.0.1:5003] [Out-ID:7f521b65]
[INFO  13:48:10.106787] ZeroMQ connect [URL:tcp://127.0.0.1:5004] [Out-ID:fee31d03]
[INFO  13:48:10.106941] ZeroMQ connect [URL:tcp://127.0.0.1:5005] [Out-ID:91771adb]
[INFO  13:48:10.107081] ZeroMQ connect [URL:tcp://127.0.0.1:5006] [Out-ID:b6077a2e]
[INFO  13:48:10.107223] ZeroMQ connect [URL:tcp://127.0.0.1:5007] [Out-ID:3e0b2ea4]
[INFO  13:48:10.107364] ZeroMQ connect [URL:tcp://127.0.0.1:5008] [Out-ID:dcd0bde2]
[INFO  13:48:10.107506] ZeroMQ connect [URL:tcp://127.0.0.1:5009] [Out-ID:ccb4ce84]
[INFO  13:48:10.107589] Loading Component [name:talker_component]
Loading: libtalker_component.so
[INFO  13:48:10.110807] Registered Component [Name:talker_component]
[INFO  13:48:10.110885] Instantiating Component [Name:talker_component]
[INFO  13:48:10.111185] Loading Component [name:listener_component]
Loading: liblistener_component.so
[INFO  13:48:10.111206] Running Component [Name:talker_component]
[INFO  13:48:10.112782] Registered Component [Name:listener_component]
[INFO  13:48:10.112809] Instantiating Component [Name:listener_component]
[INFO  13:48:10.112899] Running component_manager
[INFO  13:48:10.112915] Running Component [Name:listener_component]
[INFO  13:48:11.111666] Publishing [Msg:Hello World 1]
[INFO  13:48:11.111816] Received [msg:Hello World 1]
[INFO  13:48:12.111639] Publishing [Msg:Hello World 2]
[INFO  13:48:12.111764] Received [msg:Hello World 2]
[INFO  13:48:13.111628] Publishing [Msg:Hello World 3]
[INFO  13:48:13.111766] Received [msg:Hello World 3]
[INFO  13:48:14.111638] Publishing [Msg:Hello World 4]
[INFO  13:48:14.111787] Received [msg:Hello World 4]
...
...
```

Lets try the python version in a different shell.
(Remember to source the setup.sh file again)

```console
python ../darc_examples/python/listener_peer.py
```

And we start receiving the messages:

```console
Loading: liblistener_component.so
[INFO  13:50:35.473240] Registered Component [Name:listener_component]
[INFO  13:50:35.473345] Instantiating Component [Name:listener_component]
[INFO  13:50:35.473646] Running Component [Name:listener_component]
[INFO  13:50:35.473936] ZeroMQ connect [URL:tcp://127.0.0.1:5000] [Out-ID:bf5cd3c4]
[INFO  13:50:35.474312] ZeroMQ connect [URL:tcp://127.0.0.1:5001] [Out-ID:9bb6f22e]
[INFO  13:50:35.474571] ZeroMQ connect [URL:tcp://127.0.0.1:5002] [Out-ID:b96a1d20]
[INFO  13:50:35.474827] ZeroMQ connect [URL:tcp://127.0.0.1:5003] [Out-ID:57ef2237]
[INFO  13:50:35.475101] ZeroMQ connect [URL:tcp://127.0.0.1:5004] [Out-ID:47922663]
[INFO  13:50:35.475364] ZeroMQ connect [URL:tcp://127.0.0.1:5005] [Out-ID:51271b7f]
[INFO  13:50:35.475609] ZeroMQ connect [URL:tcp://127.0.0.1:5006] [Out-ID:e23c114a]
[INFO  13:50:35.475925] ZeroMQ connect [URL:tcp://127.0.0.1:5007] [Out-ID:5a38cf47]
[INFO  13:50:35.476191] ZeroMQ connect [URL:tcp://127.0.0.1:5008] [Out-ID:258e3b53]
[INFO  13:50:35.476587] ZeroMQ accept [URL:tcp://127.0.0.1:5009]
[INFO  13:50:35.476704] Running component_manager
[INFO  13:50:59.235725] Peer Connected [peer_id:101639b7]
[INFO  13:51:03.144473] Received [msg:Hello World 24]
[INFO  13:51:04.144472] Received [msg:Hello World 25]
[INFO  13:51:05.144511] Received [msg:Hello World 26]
[INFO  13:51:06.144410] Received [msg:Hello World 27]
[INFO  13:51:07.144499] Received [msg:Hello World 28]
...
...
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

### darc
Documentation and wrapper for dependencies

### darc_examples:
Contain examples of talker and listener components.
