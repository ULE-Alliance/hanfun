
## Overview

This document describes the current API for Bithium's HAN-FUN implementation.

The HAN-FUN specification aims at providing a common framework so that multiple
sensors and actuators can operate with each other.

The specification was created with Home Area Networks (HAN) in mind for the
purpose of home automation.

However, do to the flexibility the HAN-FUN specification can be  applied to other fields
where sensors and actuators need to work together to provide some functionality.

### General Concepts.

First of all the current implementation is in C++.

This means we have access to the concepts of classes and methods.

Each HAN-FUN interface role is implemented as a class. If the interface role sends
messages to the network then the class contains a method that will create the corresponding
message and send it to the network.

If an interface role receives messages from the network then callback methods were
created, the application need to override in order to deal with those events.

Building the interface classes the profile classes group interfaces together to
provide the functionality specified.

A unit classes create the bridge between the profiles and the devices that use them.

The device classes are responsible for the sending and receiving messages to/from the
network and delivering them to the correct unit.
