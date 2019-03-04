# The KIWI.KI Embedded Hiring Assessment

This is a project for KIWI.KI to assess potential embedded candidates, it will
contain a short task for the candidates to complete that will display their
embedded coding skill/style.

At KIWI we have multiple embedded devices running on bare metal. This project
is based on some real work we have had to do in our gateway and sensor devices,
with some simplified internals to make the example easier to understand. You
can assume that you have the C99 standard library available, except for some
parts that are not available with our bare metal devices:

 * No `<errno.h>` or `<math.h>`
 * No file handles, and no functions that operate on them
 * No functions that operate on `stdin`, `stderr` or `stdout`
 * No dynamic memory; `malloc`, `free`, etc. are not available to use

## The Task

### Intro

Our IoT infrastructure is a pretty standard star-of-stars topology, we have our
central backend that communicates with a number of "gateways" over the internet,
these gateways each have a number of "sensors" that they can reach over a custom
868 MHz protocol (known as `wireless` in the code).

The gateway device is purely used for providing internet connectivity for a
number of sensors, it has very little functionality of its own.

The sensor device is connected to a door and is capable of opening this door
(commonly via the buzzer system on an electrified apartment door). It also
communicates with the KIWI Ki wirelessly to authenticate users and open the
door, but that doesn't directly affect this task.

Both of these devices use the same base hardware system, and their firmwares
share a common base.

For the purposes of this task we will abstract out the two communication links,
ignoring any encryption/authentication that is needed in the real system and
assuming that each sensor is guaranteed to talk to just one gateway. The gateway
has a module that will handle the communication with the backend over the
internet in `includes/gateway/modem.h` and a module that will handle
communicating with a specific sensor over 868 MHz in
`includes/gateway/wireless.h`. The sensor has a module that will handle
communicating with its gateway in `includes/sensor/wireless.h`.

The firmware on these devices are architected as a series of cooperative polled
functions. The code you're implementing is one of these functions, so must be
careful to not block the main loop.

If the backend needs to refer to a specific sensor the only data it knows is
the device identifier available from `get_device_id()` in
`includes/common/device.h`. You can assume that when the backend communicates
with a gateway it is identified as part of the underlying internet protocol
(e.g. via the gateway's IP address).

### Details

We would like you to implement a new data protocol for a limited subset of our
commands. You have free reign to format the data both over the internet and the
868 MHz protocol as you like, but note:

 * We don't guarantee the backend engineers will implement your specification
   correctly.  The gateway will have to handle invalid data being sent to it
   while they work on a fix.
 * Neither the internet protocol, nor the custom 868 MHz protocol guarantees
   delivery, the backend expects this and will resend messages if it doesn't
   get some form of application layer ACK within a timeout.
 * The gateways and sensors both receive firmware updates from time to time and
   any future commands added to this protocol must not break it during the time
   when their firmware versions are out of sync.
 * Preferably it will be possible to add a new command to the sensor firmware
   without having to update the gateway that the sensor communicates through.
 * The internet protocol has a limit of 128 payload bytes per data packet.
 * The 868 MHz protocol has a limit of 32 payload bytes per packet, but does
   include its own addressing based on the device identifier.

The commands to be implemented are:

 * Ping Gateway/Sensor
   * The backend wishes to see if the gateway or a specified sensor is alive,
     it expects some form of a PONG response from it.
 * Restart Gateway/Sensor
   * The backend wishes to restart the gateway or a specified sensor. In this
     one case the device does not need to send back an ACK as the restart will
     be verified out of band.
 * Add New Ki Token to Sensor
   * The backend wishes to add a new authentication token for a Ki to a
     specified sensor, this token is a 128 bit binary value, see
     `includes/sensor/ki_store.h` for functions related to adding this.
 * Remove Ki Token from Sensor
   * The backend wishes to remove an authentication token for a Ki from a
     specified sensor, this token is a 128 bit binary value, see
     `includes/sensor/ki_store.h` for functions related to removing this.
 * Open Door Connected to Sensor
   * The backend wishes to unlock the door connected to a specified sensor, see
     `includes/sensor/door.h` for functions related to this.

The code should go in `src/gateway.c` and `src/sensor.c`, the internet protocol
should also be specified in a plaintext/markdown `PROTOCOL` file well enough
that the backend engineers could implement the protocol. Don't spend too long
on designing the protocol, as long as you have basic justifications for the
design of it we care more about your implementation/specification of it.

### Compiling

There is a provided `Makefile` that will compile using both `gcc` and `clang`
with relatively strict settings. Preferably this should pass without any errors
(and bonus points if `make Weverything` doesn't complain about too much, but
there's too many nitpicks to actually use that in practice).

### Merge Requests

Our embedded team has a work process that takes a few hints from Agile
development. A major one of these is creating merge requests for any change and
performing code review on them. If you know how to use `git` we would like you
to complete this task as a series of commits with well written messages as if
it were intended to be a single merge request.
