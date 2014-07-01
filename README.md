# HAN-FUN

This project aims at providing an implementation of the HAN-FUN application specification.

A HAN-FUN network is a star network, it consists of a concentrator (base) and several nodes that
connect to the concentrator to send/receive messages to/from the network.

## Project Setup

This project is implemented in C++ and depends only on the standard C++ library for usage, however
the unit tests and example applications depend on third-party libraries.

The build system used is [CMake](http://www.cmake.org/).

* Dependencies

  * gcc (>= 4.5)
  * cmake (>= 2.8.11)

* Building:

      $ mkdir build/
      $ cd build/
      $ cmake ..
      $ make

## Testing

Currently the build of the unit tests is only supported when compiling using the code repository.

To build the unit tests present in the source code, issue the following commands
(on the root of the project):

    $ git submodule init
    $ git submodule update
    $ mkdir build
    $ cd build
    $ cmake -DHAN-FUN_BUILD_TESTS=ON ..
    $ make

The unit tests will be run at the end of the complication.

To re-run the tests, issue:

    $ make check

## Examples

The folder _apps/_ contains two example applications, one for the concentrator (base) side and one
for the node side.

The applications communicate over TCP/IP.

The support for this transport layer is based on the [libuv](https://github.com/joyent/libuv).

This is downloaded and build automatically when building the example applications is requested.

To build the example applications, issue the following commands (on the root of the project):

    $ mkdir build
    $ cd build
    $ cmake -DHF_BUILD_APPS=ON ..
    $ make

This will build the example applications for the base (_build/apps/base_) and for
the node (_build/apps/node_).

The *base* application demonstrates the registration of nodes in a HAN-FUN network and the binding
of nodes that have complementary interfaces.

The *node* application demonstrates, a simple switch or a simple light application.

For example to register a _simple switch_ and _simple light_ to the _base_ and bind the _switch_ to
the _light_ so that when the _switch toggles_ it toggles the _light_.

 * Open three terminals and start the applications;

       $ apps/base    (Base)
       $ apps/node 1  (Simple Switch)
       $ apps/node 2  (Simple Light)

 * Change the first node application to a simple switch;

       > ss

 * Change the second node application to a simple light;

       > sl

 * Start registration on the base and register the simple switch;

       > r 1 1 (on the base terminal)
       > r (on the switch terminal)
       > a (check device address, it should be 1)

 * Start registration on the base and register the simple light;

       > r 1 2 (on the base terminal)
       > r (on the light terminal)
       > a (check device address, it should be 2)

 * Bind the _switch_ to the _light_;

       > b 1 2 (on the base terminal)

  * Send a toggle command from the _switch_ to the _light_ :

       > toggle (on the switch terminal)

The last command should make the node application configured as a _light_ print that a
toggle command was received.

Please see the help in the applications for further features.

## Author

 * Filipe Alves <<filipe.alves@bithium.com>>

## License

Copyright &copy; &nbsp; 2014 Bithium S.A. All rights reserved.

 * http://www.bithium.com
