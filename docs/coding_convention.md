# Developer: Coding Conventions

On this page you'll find guidelines on how to write your code. The main reason
for this collection is to increase the readability and therefore reduce the
amount of errors.

## General Recommendations

- Any violation to the guide is allowed if it enhances readability.
  The main goal of this guidelines is to improve readability and thereby the
  understanding and the maintainability and general quality of the code. It is
  not possible to cover all the specific cases in a general guide.
- **Keep it simple!**
  Ease of understanding and use code is prefered about flexibility and
  complexity
- **Devide**
  Each class/methode has only one task.


## Documentation

- **Do it!**
- Documentation on how to use a class goes to the header file. Use
  [doxygen style](http://www.doxygen.org) therefore. Documentation about the
  internal structure goes to the source file.
- Start writing the documentation before programming!
  Describe in a few sentences what the class should do. If you cannot describe
  the class well it maybe that the overall structure of the class is wrong.
- Documentation and code are connected. So if you change the code remember to
  update the documentation.
  Comments that contradict the code are worse than no comments.  Always make
  a priority of keeping the comments up-to-date when the code changes!

For doxygen use `///` instead of `//!` for single line comments since it is
easier to read, and faster to write.

Please escape all doxygen keywords with `@` instead of `\\` .
This helps the readability of C++ code, since `@` is not used in C++.

When writing doxygen descriptions, you should prefer the Markdown syntax for
highlighting text, creating lists and references, whenever possible.
You can and should also create hyperlinks using Markdown.
Check out the [full Markdown syntax here](http://daringfireball.net/projects/markdown/syntax).

Specifically **do not** use:

- \\c code, \\p code,
- \\a italics, \\e italics, \\em italics, or
- \\b bold.

For these text highlights please use \`code\`, \*italic\*, \*\*bold\*\*.

While this does not affect the HTML output, it makes the raw description
text (in ASCII) a lot more intuitive to read for the developers


## Unittests

- Unittests are simple to write but very powerful in avoiding long debug
  sessions, so use them.
- Don't write examples, write unittests. Both show how a class is intended to
  be used, but unittests assure that the code is compilable and works
  as expected!
- It is not possible to write unittests for code that directly interacts with
  hardware, but for all other code it is!
- Use clean interfaces and your code will be easy to test **and** to understand.


## Naming Conventions

- All names except namespaces and named constants must be CamelCase.

  - Classes and types start with uppercase, variables and functions
    with lowercase
  - Global constants and defines used internally by modm must be all
    UPPERCASE using underscores to separate words.
  - Scoped constants, enums, structs must be Capitalized and **not**
    uppercased.
  - Namespaces should be all lowercase with underscores.

- Use names that describe what a method is doing. Use the following prefixes:

  - `get/set` for functions accessing class attributes
  - `is` for functions returning boolean values (isEnabled(), isActive(), etc.)
  - `acknowledge` for acknowledging hardware flags

- All names should in American English using only ASCII characters (initialize, _not_ initialise!).
- Abbreviations in names should be avoided (do not write cmd instead of command)
- If you cannot find a good name for a variable/method it could be a
  signal that the structure of your code is wrong


## Statements

- **Never** use the `using namespace`-statement in a header file.
- Type conversions must always be done explicitly. Never rely on implicit type
  conversion.
  By this, the programmer indicates that he is aware of the different types
  involved and that the mix is intentional.
- Variables should be declared in the smallest scope possible.
- Be **very** careful what you `#define` inside a header file. It will influence
  code you're not even aware of. If you absolutely have to, prefix it with a
  reasonable namespace.

## Files

- File extensions:

  - `*.cpp` for C++ sources
  - `*.hpp` for C++ header
  - `*_impl.hpp` for C++ template implementations
  - `*.h` for C header
  - `*.c` for C sources

- Use filename = classname
- When writing a template class put the implementation in a file with
  the extension `*_impl.hpp` and include this file at the end of your
  template class.
- Use UTF-8!
- Use Unix-style line endings.


## Layout

- Limit all lines to a maximum of 120 characters.
- Use **real TAB** with width of 4 spaces (do not replace by spaces)
- Whitespaces and empty lines are for free, so use them to structure your code
  and align the commands.
- Remove trailing whitespace. Most editors can be configured to automatically
  strip trailing whitespace on file save, so use that option.
- Always surround binary operators with a single space on either side:
  assignment (=), augmented assignment (+=, -= etc.),
  comparisons (==, <, >, !=, <>, <=, >=), etc.
- Use spaces around arithmetic operators

For everything else see the examples section.


## Optimization

Don't optimize on code level, only do it on structural level! Changing the code
to reduce one or two if-statements has only little effect and increase the
possibility to errors.

This is a list of expensive statements:

- open/closing files
- `cout` (writing to the console or to a file)
- search / sort => use well approved algorithm
- use the best fitting container (list, queue, map, ...)
- allocation of memory (heap needs ~100 times more time than the stack)

## Examples

Header file:

```c++
/*
 * Copyright (c) {year}, {your name}
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// do _not_ use double underscores "__", they are reserved by your **system**!
#ifndef MODM_{MODULE}_{CLASSNAME}_HPP
#define MODM_{MODULE}_{CLASSNAME}_HPP

#include "path_specific_file.hpp"
#include <modm/modm_file.hpp>

/// global constants used for modm internal use are all CAPS
static constexpr uint8_t MODM_INTERNAL_CONSTANT_IN_ALL_CAPS = 0x04;
/// modm defines are also all CAPS
#define MODM_DEFINE 0x2

// this is almost always the top namespace!
namespace modm
{

// do not indent namespaces, since it reduces the available horizontal space
// but put a newline here

// by using a struct instead of a namespace
// other classes can inherit from this
// You should use this especially for template classes!
struct Module
{
	// put common enums, constexpr here

	/// the answer to life the universe and everything
	static constexpr uint8_t PublicAndScopedConstantInCamelCase = 0x42;

	/// This a description of a group of stuff, which will group the following
	/// together in the documentation
	/// @{

	// prefer enum classes over simple enums whenever applicable
	// in this case there are no name collisions of `Stop`

	/// Enum class enums are Capitalized, but **not** in CAPITAL letters!
	enum class
	// put a newline here
	Operation : uint8_t
	{
		Stop = 0,		///< Generate a Stop Condition
		Restart = 1,	///< Generate a Restart
		Write = 2,		///< Write data to the slave
		Read = 3,		///< Read data from the slave
	};

	/// Further operations after write operation.
	enum class
	OperationAfterWrite
	{
		Stop = 0,
		Restart = 1,
		Write = 2,
	};
	/// @}
};

/**
 * Put a brief description of this class here ending in a dot or newline.
 *
 * More details about the class here.
 * @see	Reference to other class or similar
 *
 * The author tag should name the person that wrote or added this code.
 * It is then easier to know who to contact.
 *
 * Even when you only adapted this code, put your name there and add this:
 * Adapted from {the code} of {original author}.
 *
 * @author	{Name of author/adapter}
 * @author	{Name of author/contributor}
 * @ingroup	doxygen_group
 */
class ModuleSpecificClass : public ::modm::SuperClass, public Module
{
	/// one line descriptions should _not_ use /** */
	static void
	// put a newline here
	methodNameUsingCamelCase();

	/// efficient inline function
	static inline void
	methodName1()
	{
		// remember to put inline functions in the `*.hpp` or better
		// the `*_impl.hpp`
	}

	/**
	 * Short description of functionality
	 *
	 *
	 * @param	arg1
	 *	Say what arg1 is for and use a new line for it
	 * @param	epic
	 *	epic description and don't forget the tab in front
	 * @param	longArgumentName
	 *	long descriptions can be annoying and by putting the description
	 *	in a new line, you gain a lot of space and do not have to cram
	 *	it into the remaining space to the right of the argument name.
	 *
	 * @return	what it returns
	 */
	static bool
	methodName2(uint8_t arg1, EpicEnum epic, uint32_t longArgumentName);
};

// template class
/**
 * Brief description of this class.
 *
 * @tparam	Timer
 *	Describe what Timer is
 *
 * @author	{Name of author}
 * @ingroup	doxygen_group
 */
template< typename Timer >
class TemplateClass : public Module
{
public:
	// Do NOT add any enums, structs, constexpr here,
	// that have nothing to do with the dependency on TIMER!
	//
	// Otherwise you MUST provide TIMER whenever you access this enum class:
	// modm::TemplateClass<TIMER>::Options::InconvenientEnumElement !
	enum class
	Options
	{
		InconvenientEnumElement,
		// options
	};

};

}	// namespace modm

#endif	// MODM_{MODULE}_{CLASSNAME}_HPP
```

Source file:

```c++
/*
 * Copyright (c) {year}, {your name}
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/utils/misc.hpp>

#include "filename.hpp"

modm::MyClass::MyClass() :
myValue(false)
{
}

virtual void
modm::MyClass::function(const Type& argument);
{
	if (argument.myFunction() > 7)
	{
		// explicitly cast types!
		floatValue = static_cast<float>(intValue);

		// WRONG!
		floatValue = intValue;

		// Give operators enough whitespace
		floatValue = floatValue * 2.0f;

		// Use brackets
		floatValue = (floatValue * 2.0f) + (floatValue - 2.0f);

		// For boolean operations there is and, or, xor, et cetera
		bool boolA;
		bool boolB;
		if ((boolA and boolB) or (boolA xor boolB)) {
			boolA = true;
		}

	}
}

bool
modm::MyClass::anotherFunction()
{
	// for long if else branches, use a newline before the bracket
	if (...)
	{
		// long code
		// that spans
		// several lines
	}
	else
	{
		// else branch
	}

	// For short code you can leave the bracket on the same line but on the
	// condition, that the branches have easy to understand code
	if (...) {
		// one or two lines
	}
	else {
		// keep it short
	}

	// For `while` and `for` loops, the loop condition mostly does not have a
	// lot to do with the code within the loop, so give it a newline.
	while(...)
	{
		// always use a newline
	}

	for(...)
	{
		// always use a newline
	}
}
```
