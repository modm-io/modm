# XPCC rosserial examples

[rosserial] is a protocol for wrapping standard ROS serialized messages and multiplexing multiple topics and services over a character device such as a serial port or network socket.

XPCC includes support for rosserial. See examples for an introduction.

On the host the rosserial Python bridge must be running, e.g.

    rosrun rosserial_python serial_node.py _port:=/dev/ttyACM0 _baud:=1000000

## Simple example

The simplest example is the `sub_pub` example. It is made for a Nucleo board with an RGB Led (or three LEDs) connected to A0, A1, and A2 of the Arduino connector.

Compile the example and flash it to the board.

     cd sub_pub
     scons
     scons program

The green LED on the board should toggle once per second. XPCC uses rosserial to advertise a publisher (`/button`) and three subscribers (`/led/red`, `/led/green`, `/led/blue`).

On a Linux with ROS installed get a new ROS workspace:

     mkdir -p xpcc_ws/src
     cd xpcc_ws/src
     catkin_init_workspace
     git clone https://github.com/strongly-typed/xpcc_rosserial_demo.git
     cd ..
     catkin_make
     source devel/setup.bash

And start the supplied launch file:

     roslaunch xpcc_rosserial_demo xpcc_led_demo.launch

The `led_demo.py`-node subscribes to the `/button` topic from the Nucleo board and publishes new LED states to `/led/[red, green, blue]` topics.

The LEDs should toggle between red, green and blue with each button press. The expected console output should look like:

    [INFO] [1509051381.245712]: ROS Serial Python Node
    [INFO] [1509051381.266949]: Connecting to /dev/ttyACM0 at 1000000 baud
    [INFO] [1509051381.297971]: /led_demo: XPCC Rosserial Led Demo: Press the button to cycle red, green and blue LED.
    [INFO] [1509051383.393886]: Note: publish buffer size is 512 bytes
    [INFO] [1509051383.396080]: Setup publisher on /button [std_msgs/Bool]
    [INFO] [1509051383.420355]: Note: subscribe buffer size is 512 bytes
    [INFO] [1509051383.423184]: Setup subscriber on /led/red [std_msgs/Bool]
    [INFO] [1509051383.448187]: Setup subscriber on /led/green [std_msgs/Bool]
    [INFO] [1509051383.492618]: Setup subscriber on /led/blue [std_msgs/Bool]
    [INFO] [1509051398.504384]: /led_demo: Button True
    [INFO] [1509051398.507496]: /led_demo: Setting green
    [INFO] [1509051398.683607]: /led_demo: Button False
    [INFO] [1509051399.109279]: /led_demo: Button True
    [INFO] [1509051399.112368]: /led_demo: Setting blue

This demonstrates the capability of xpcc to work with ROS as a data sink and data source.

[rosserial]: http://wiki.ros.org/rosserial
