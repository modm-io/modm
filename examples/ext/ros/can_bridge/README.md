# CAN Bridge between modm and ROS

## Package summary

Conversion node for messages from modm CAN to a ROS Topic and vice versa.
Strongly inspired by [`socketcan_bridge`](http://wiki.ros.org/socketcan_bridge) from ROS. From a ROS point of view this is a complete replacement of a USB CAN adaptor (like Peak PCAN-USB) connected to a Linux computer running `socketcan_bridge`. It allows you to receive all CAN messages from the bus in ROS and send arbitrary messages to the CAN bus from ROS.

## Overview

The packages provides functionality to expose CAN frames from modm to a ROS Topic. Internally it uses modm to receive and send messages to and from the CAN bus controller of a microcontroller.

### Hardware setup

A modm compatible microcontroller with CAN controller is required. In most cases an external CAN transceiver must be connected between the CAN bus and the CAN controller of the microcontroller. Example setup is

* STM32F103 Blue Pill Board
* SN65HVD252 CAN Transceiver
  * `CAN_TX` connected to pin `B9` of microcontroller
  * `CAN_RX` connected to pin `B8` of microcontroller
* USB UART bridge
  * `TX` connected to pin `A3`
  * `RX` connected to pin `A2`

Nucleo boards with integrated USB UART bridge also work. LPC11C24 might be added later.

On-board LED toggles once per second and for every message received or transmitted.

## Nodes

The functionality is offered in the form of a single node running on the microcontroller. The microcontroller uses [rosserial_python](http://wiki.ros.org/rosserial_python) to forward and receive messages to and from the ROS core on the host computer.

### Subscribed Topics

`sent_messages` [can_msgs/Frame](http://docs.ros.org/api/can_msgs/html/msg/Frame.html)

* Messages received here will be sent to the CAN bus.

### Published Topics

`received_messages` [can_msgs/Frame](http://docs.ros.org/api/can_msgs/html/msg/Frame.html)

* Frames received on the CAN bus are published in this topic.

## Examples

Build and program the binary to the microcontroller:

    lbuild build && scons program

To run the node with a different port and baud rate, for example on `/dev/ttyACM0`, you must specify the `~port` and `~baud` parameters on the command line.

    rosrun rosserial_python serial_node.py _port:=/dev/ttyACM0 _baud:=250000

Baud rate is the baud rate of the serial link and is set to 250 kBps. It is not the baud rate of the CAN bus, which is also hardcoded to 250 kBps.
