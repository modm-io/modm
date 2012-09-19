% Why use xpcc?
% 
% 2012-09-16

Why should I use this library?
==============================

![](images/rca_insitor.jpg "Insitor")

Some good reasons for using this library:

Open-Source
:	It's Open-Source, the complete source code is available. With the
	BSD-licence we chose a very moderate licence. Therefore you are
	free to do nearly everything you like with the code.

It's C++!
:	Some people think that C++ automatically brings a lot of bloat in
	form of code size. But that's not true.

	If you use the expensive features like virtual functions and copy
	constructors careless you get code that's slower and bigger than it
	needs to be. But if you use classes and templates in a sensible way
	you get fast and small software that is easier to extended than
	the C counterpart.

	Learn more about this on the page about \ref programming_techniques 
	"software design descisions".

Documentation
:	Full documentation for every part of the library is available
	online. It's not perfect, but we're working on it.

Reliability
:	A lot classes have corresponding unit-tests in place. Unit testing
	cannot catch every error in the program, but they can ensure a
	specific behaviour is present and that a bug, once found, does not
	appear again.

	A second advantage is that the tests provides a sort of embedded
	documentation of the system. You can learn about how to use a class
	by looking at its unit tests.

	Read more about this in our \ref unittests "developer section".

Just try it out ...
:	Get the code at [Github][xpcc] and see
	for yourself.

[xpcc]: https://github.com/roboterclubaachen/xpcc/
