% Cross Platform Component Communication
% Fabian Greif
% 2012-09-16

![](images/logo_xpcc.png "xpcc logo")

The XPCC project consists of two main parts:

- First part is the XPCC communication
  module, a packet orientated communication protocol, enabling transparent
  communication between components on different platforms (from PCs to small
  8-bit microcontrollers). It provides an interface to communicate via CAN,
  TCP/IP or TIPC.
- The second part is a universal but a bit robotic orientated C++ library.

Because standard C++ libraries like the STL are not available for small
microcontrollers we created a reduced version of it, adapted to the specific 
needs of microcontrollers. For example a set of generic containers is
described in the \ref container section.

This manual is divided in the following sections:

- [Why use xpcc?](why_use_xpcc.html)
- get_code
- installation
	- installation_linux
	- [Install Windows](installation_windows.html)
	- IDE
		- [Configure Eclipse (DE)](install_ide_eclipse.html)
		- [Configure PN2 (DE)](install_ide_pn2.html)
		- [Configure AVR Studio 2 (DE)](install_ide_avrstudio4.html)
-  build_system
	- sconstruct_template
	- sconstruct
	- sconstruct_advanced
	- scons_tools
	- configuration_files
- getting_started
	- project_sconstruct
	- project_makefile
- developer
	- design_decisions
	- coding_conventions
	- unittests
	- license
- system_design


The people behind this library
------------------------------

![](images/rca_2010.jpg "Team 2010")

During the last years we, [the Roboterclub Aachen e.V.][rca], have developed a
software library for communication among components that are distributed on
PCs and Microcontrollers. The primary aim has been the usage of this system
in autonomous robots for the [EUROBOT competition][eurobot].

This library is the result of this development. The PC and the AVR software
have merged together into a single library to simplifiy and unifiy the software
development.

This library was successfully used in our robot [Insitor][rca_insitor] with which we won the
[vize world champion][rca_eurobot_2010] title in 2010.


How can I contribute?
---------------------

Use the library and give us some feedback. That's the easiest way for you and
the best way for us to see if something is unclear or missing.

If you need a specific feature just drop us a [mail][]
and we will look into it. But don't expect a quick response, we are always
busy building robots ;-)

If you want to change the library be sure to read the 
[developer section][develop.html], to familiarize yourself with the coding
convention, design decisions etc.
The Source Code is free avilable, so feel free to adapt it to your needs. The
only thing we ask you to do is to contribute your changes back. That way
everybody can profit from it.

Have fun!


[rca]: http://www.roboterclub.rwth-aachen.de
[eurobot]: http://www.eurobot.org/eng/
[rca_insitor]: http://www.roboterclub.rwth-aachen.de/cms/index.php?option=com_content&task=view&id=165&Itemid=114
[rca_eurobot_2010]: http://www.roboterclub.rwth-aachen.de/cms/index.php?option=com_content&task=view&id=169&Itemid=118

[mail]: mailto:roboterclub@rwth-aachen.de
