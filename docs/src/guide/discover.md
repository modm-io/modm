# Discover modm

modm uses the [lbuild code generator][lbuild] to discover available modules and
their configuration options. Let's use the lbuild CLI to poke around modm!

Make sure you've [installed all tools required for building modm](../installation)
and you've cloned the modm repository *recursively* and change directory into
modm:

```sh
git clone --recurse-submodules --jobs 8 https://github.com/modm-io/modm.git
cd modm
```

*lbuild* operates on repositories which contain modules and options in a tree
format. The `modm/repo.lb` file must be declared to find all modm modules:

```
 $ lbuild --repository repo.lb discover
Parser(lbuild)
╰── Repository(modm @ .)   modm: a barebone embedded library generator
    ├── Option(target) = REQUIRED in [stm32f469ngh6, stm32f469nih6, stm32f469vet6, ...
    ├── Config(modm:disco-f469ni)   STM32F469IDISCOVERY
   ...
```

This gives you an overview of the repositories and their options and
configurations. Here you can see the `modm:target` option is marked as
*REQUIRED*, since the HAL modules obviously depend on the target device, so we
need to discover the option in detail:

```
 $ lbuild -r repo.lb discover modm:target
>> modm:target  [TargetOption]

Meta-HAL target device

Value: REQUIRED
Inputs: [at90can128-16ai, at90can128-16au, at90can128-16mi, at90can128-16mu,
         ... a really long list ...
        stm32l4s9aii6, stm32l4s9vit6, stm32l4s9zij6, stm32l4s9zit6, stm32l4s9ziy6]
```

!!! info "Options are checked"
    *lbuild* checks all your project options against the possible values in the
    module and outputs an error if they are incorrect. Try passing a wrong value
    to see what happens.

You can now choose a value for this repository option and discover the available
modules for this specific target. We will choose the `stm32f469nih6` device:

```
 $ lbuild -r repo.lb --option modm:target=stm32f469nih6 discover
Parser(lbuild)
╰── Repository(modm @ .)   modm: a barebone embedded library generator
    ├── Option(target) = stm32f469nih6 in [stm32f407vgt6, stm32f469nih6, ...
    ├── Config(modm:disco-f469ni)   STM32F469IDISCOVERY
   ...
    ├── Module(modm:board)   Board Support Packages
    │   ╰── Module(modm:board:disco-f469ni)   STM32F469IDISCOVERY
   ...
    ├── Module(modm:build)   Build System Generators
    │   ├── Option(build.path) = build/parent-folder in [String]
    │   ├── Option(project.name) = parent-folder in [String]
    │   ╰── Module(modm:build:scons)  SCons Build Script Generator
   ...
    ├── Module(modm:platform)   Platform HAL
    │   ├── Module(modm:platform:cortex-m)   ARM Cortex-M Core
    │   │   ├── Option(float-abi) = hard in [hard, soft, softfp]   Floating point ABI
    │   │   ├── Option(main_stack_size) = 3*1024 (3072) in [256 .. 3*1024 .. 65536]
    │   │   ├── Option(vector_table_location) = rom in [ram, rom]   Vector table location
   ... ...
    │   ├── Module(modm:platform:uart)   Universal Asynchronous Receiver Transmitter (UART)
    │   │   ├── Module(modm:platform:uart:1)   Instance 1
    │   │   │   ├── Option(buffer.rx) = 0 in [0 ... 65534]
    │   │   │   ╰── Option(buffer.tx) = 0 in [0 ... 65534]
```

Since there are no more REQUIRED options, you can now discover all module and
option descriptions. Let's have a look at the [`modm:platform:cortex-m` module][cmdocs],
which deals with the startup code of Cortex-M devices. Note that you can leave
out parts of any name as long as it remains unique:

```
 $ lbuild -r repo.lb -D :target=stm32f469nih6 discover ::cortex-m
>> modm:platform:cortex-m  [Module]

# ARM Cortex-M Core

This module generates the startup code, vector table, linkerscript as well as
initialize the heap, deal with assertions, provide blocking delay functions,
atomic and unaligned access and the GNU build ID.

>>>> modm:platform:cortex-m:vector_table_location  [EnumerationOption]

# Vector table location in ROM or RAM

The vector table is always stored in ROM and copied to RAM by the startup script
if required. You can modify the RAM vector table using the CMSIS NVIC functions:

- `void NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)`
- `uint32_t NVIC_GetVector(IRQn_Type IRQn)`

Value: rom
Inputs: [ram, rom]
```

Once you've made your choices about which modules and options you want, you can
generate a custom modm library using the build command:

```sh
lbuild -r repo.lb -D :target=stm32f469nih6 -D :::vector_table_location=ram \
    build --module ::cortex-m
```

Which will create a folder `modm` at the callsite which contains the minimal
startup code for the STM32F469NIH6 device.

```
 $ tree modm
modm
├── ext
│   └── cmsis
│       └── device
│           ├── stm32f469xx.h
│           └── system_stm32f4xx.h
└── src
    └── modm
        ├── platform
        │   ├── core
        │   │   ├── reset_handler.sx
        │   │   ├── startup.c
        │   │   └── vectors.c
        │   └── device.hpp
        └── platform.hpp
```

However, declaring everything on the command line is a bit annoying, so instead
we can create a `project.xml` with the same information:

```xml
<library>

  <repositories>
    <repository>
      <path>repo.lb</path>
    </repository>
  </repositories>

  <options>
    <option name="modm:target">stm32f469nih6</option>
    <option name="modm:platform:cortex-m:vector_table_location">ram</option>
  </options>

  <modules>
    <module>modm:platform:cortex-m</module>
  </modules>

</library>
```

This file is now automatically used for all calls to *lbuild*:

```sh
lbuild discover
lbuild build
```

You can read the [lbuild reference manual][lbuild] if you want to know the
details about config inheritance, collectors and queries. You can discover the
whole repository view with the `--developer` option:

```sh
lbuild discover --developer
```

[examples]: https://github.com/modm-io/modm/tree/develop/examples
[issues]: https://github.com/modm-io/modm/issues
[lbuild]: https://github.com/modm-io/lbuild
[cmdocs]: ../../reference/module/modm-platform-cortex-m
