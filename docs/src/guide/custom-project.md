# Starting a new Project

The modm library explicitly generates and copies its files from inside the modm
repository into your project using the lbuild code generator. It is intended and
very strongly recommended to *commit* all generated files from `lbuild build`
into your repository. This will make any changes very clear when you run `lbuild
build` again with an updated version of modm!

We recommend adding modm into your repository as a Git submodule so that you
control exactly what version of modm you're using. We recommend you place the
modm repository (and all other external code) into a folder called `ext/` or
similiar:

```
 $ git submodule add https://github.com/modm-io/modm.git ext/modm
 $ tree
.
└── ext
    └── modm
        ├── repo.lb
       ...
```

Next you want to create a folder with a `project.xml` and a `main.cpp` file. We
recommend you place that in a separate `app/` folder so that all your firmwares
are in the same place:

```
 $ tree
.
├── ext
│   └── modm
│       ├── repo.lb
│      ...
└── app
    └── project_name
        ├── main.cpp
        └── project.xml
```

!!! warning "Place your applications into their own folder!"
    All modm build systems search *recursively* for application sources inside
    the current folder. If you place the modm library repository into your
    application folder you will see build errors related to building sources
    twice:

    ```
    scons: *** Multiple ways to build the same target were specified for: ...
    ```

The `project.xml` must contain the (relative) path to modm's `repo.lb` file.
Note that the project name is inferred from the project folder, but you can
easily change it. modm also does not add a build system by default, so you
want [to choose one at this point](../../reference/build-systems). Add the
`modm:docs` module to generate custom API docs:

```xml
<library>
  <repositories>
    <repository><path>../../ext/modm/repo.lb</path></repository>
  </repositories>
  <options>
    <option name="modm:build:project.name">custom_name</option>
  </options>
  <modules>
    <module>modm:build:scons</module>
    <module>modm:docs</module>
  </modules>
</library>
```

You should now be able to [discover modm](../discover) from inside your
project folder:

```
 $ cd app/project_name
 $ lbuild discover
Parser(lbuild)
╰── Repository(modm @ ../../ext/modm)   modm: a barebone embedded library generator
    ├── Option(target) = REQUIRED in [stm32f469ngh6, stm32f469nih6, stm32f469vet6, ...
   ...
```

Note that we've not yet declared the `modm:target` option, so you cannot
discover all of modm yet. You can now choose from two levels of customization:

1. Inherit and extend a board support package from modm. This is particularly
   useful if your project is based on an existing development board.

2. Define your own `modm:target` option and required modules. This is necessary
   for using modm on a custom hardware design.


## Using a Board Support Package

Use lbuild to discover the specific BSP configuration you want to use:

```
 $ lbuild discover
Parser(lbuild)
╰── Repository(modm @ ../../ext/modm)   modm: a barebone embedded library generator
    ├── Option(target) = REQUIRED in [stm32f469ngh6, stm32f469nih6, stm32f469vet6, ...
    ├── Config(modm:disco-f469ni)   STM32F469IDISCOVERY
   ...
```

Then inherit from this configuration in your `project.xml`. This will add the
`modm:target` option and give you a set of modules that the BSP depends on.
Note that you can *overwrite* any options you don't agree with, however, you
cannot remove any inherited modules.

```xml
<library>
  <repositories>...</repositories>

  <extends>modm:disco-f469ni</extends>

  <options>...</options>
  <modules>...</modules>
</library>
```

You can now `lbuild discover` all of modm. Note that the selected modules are
now marked <u>underlined</u> as a subtle hint.

Our board support packages provide their configuration in the `Board` namespace,
which you can use to initialize the target and several board subsystems.
If a serial connection is available on the board, you can directly use the modm
logging functions.

A minimal `main.cpp` can look like this:

```cpp
#include <modm/board.hpp>

int main()
{
    Board::initialize();
    Board::Leds::setOutput();

    while (true)
    {
        Board::Leds::toggle();
        modm::delayMilliseconds(Board::Button::read() ? 250 : 500);
#ifdef MODM_BOARD_HAS_LOGGER
        static uint32_t counter(0);
        MODM_LOG_INFO << "Loop counter: " << (counter++) << modm::endl;
#endif
    }
    return 0;
}
```

You can find more details about the BSPs capabilities on [docs.modm.io][docs]
which contains the full API documentation for all BSPs. Alternatively you can
generate your own API docs locally:

```sh
(cd modm/docs && doxypress doxypress.json)
# open modm/docs/html/index.html
```

Please note that the BSPs contain an opinionated set of pre-defined
functionality and settings. This is great for getting started quickly, however,
if you want to change some hardcoded things (like which UART to use for logging),
it's better to use your own BSP:

1. Generate the BSP closest to your custom hardware, then copy the files from
   `modm/src/modm/board/{name}` to your own project and modify them.

2. In your `project.xml` remove the board config inheritance (`<extends>`) and
   instead copy the pre-defined options into your own config.

3. Check what modm modules you need to depend on and add them to your own
   project (check for `module.depends(...)` in the BSPs `module.lb`).

4. You may need to manually [add the pre-defined collector values to your project
   configuration](../../reference/build-systems/#customization)
   (check for `env.collect(...)` in the BSPs `module.lb`).


## Custom Configuration

For completely custom hardware, you start by definining the `modm:target` option
and then adding the modules with their options as needed. For example, a basic
configuration for the STM32F469NIH6 with startup, GPIO and clock modules looks
like this:

```xml
<library>
  <repositories>
    <repository><path>../../ext/modm/repo.lb</path></repository>
  </repositories>
  <options>
    <option name="modm:target">stm32f469nih6</option>
  </options>
  <modules>
    <module>modm:platform:clock</module>
    <module>modm:platform:core</module>
    <module>modm:platform:gpio</module>
    <module>modm:architecture:delay</module>
    <module>modm:build:scons</module>
    <module>modm:docs</module>
  </modules>
</library>
```

Remember to include the `modm:docs` module, since you need to generate your own
API docs from your specific configuration:

```sh
(cd modm/docs && doxypress doxypress.json)
# open modm/docs/html/index.html
```

A minimal `main.cpp` for this configuration can look like this. Since there is
no BSP, you need to `#include <modm/platform.hpp>` and add the `modm::platform`
namespace manually.

```cpp
#include <modm/platform.hpp>
using namespace modm::platform;
using namespace std::chrono_literals;

int main()
{
    GpioA0::setOutput();
    while (true)
    {
        GpioA0::toggle();
        modm::delay(0.5s);
    }
}
```

We recommend to start your custom projects with a known-good configuration from
one of our examples and then work your way into your specialization.

!!! tip "Visualize your dependencies"
    You can create a dependency graph with
    `lbuild dependencies | dot -Tsvg -Grankdir=BT -o dependencies.svg`
    to help you understand what code is pulled in when you generate your library.


## Generate, Compile and Upload

Once you have your `project.xml` set up, you can call `lbuild build`, which
generates the target and configuration specific library from modm.
This will create a few files and folders:

```
 $ cd app/project_name
 $ ls
main.cpp    project.xml
 $ lbuild build
 $ ls
SConstruct      main.cpp        modm            project.xml     project.xml.log
```

For this project we included the `modm:build:scons` generator, so we can just
call `scons build`, which will compile the entire source code and output the
resource consumption:

```
 $ scons build -j8
scons: Reading SConscript files ...
scons: done reading SConscript files.
scons: Building targets ...
Compiling C++·· build/main.o
...
Indexing······· build/libmodm.a
Linking········ build/project.elf
Memory usage··· build/project.elf

Program:    3612B (0.3% used)
(.fastcode + .fastdata + .hardware_init + .reset + .rodata + .table.copy.intern +
 .table.section_heap + .table.zero.intern + .text)

Data:       3184B (1.6% used) = 144B static (0.1%) + 3040B stack (1.5%)
(.bss + .fastdata + .stack)

Heap:     197520B (98.4% available)
(.heap0 + .heap1 + .heap2 + .heap5)
```

You can upload this firmware to your target by calling `scons program`.

Additional tools are documented in the
[SCons module documentation](../../reference/module/modm-build-scons).

!!! tip "Enable parallel builds"
    Remember to add the `-j8` flag to your `scons build` call to enable parallel
    builds, which are significantly faster than sequential builds. To make this
    permanent you can add `export SCONSFLAGS="-j8"` or `export MAKEFLAGS="-j8"`
    to your `.bashrc`!

If you have any questions, [open an issue][issues] or ping
[@salkinium](https://github.com/salkinium).


[examples]: https://github.com/modm-io/modm/tree/develop/examples
[issues]: https://github.com/modm-io/modm/issues
[lbuild]: https://github.com/modm-io/lbuild
[projname]: ../../reference/module/modm-build#projectname
[docs]: https://docs.modm.io
