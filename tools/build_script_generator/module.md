# Build System Generators

This parent module defines a common set of functionality that is independent of
the specific build system generator implementation.
This includes straight-forward options like project name and build path
but also more complicated configuration for programming your target via
AvrDude or OpenOCD and debugging via GDB.

**Note that this module does not compile your project, you will need to choose
the `modm:build:scons` or `modm:build:cmake` submodule for that, or provide
your own build system.**


## Using OpenOCD

For accessing your ARM Cortex-M based device, we use OpenOCD by default and generate a
`modm/openocd.cfg` file with the target specific configuration:

- Search directories passed via the `openocd.search` metadata key.
- User configuration files passed via the `openocd.configfile` metadata key.
  Your custom `modm:build:openocd.cfg` is added here too.
- The target specific programming command `modm_program elffile`

You can now upload your program in a one-line command:

```sh
openocd -f modm/openocd.cfg -c "modm_program path/to/project.elf"
```
```
Open On-Chip Debugger 0.10.0
adapter speed: 2000 kHz
adapter_nsrst_delay: 100
none separate
srst_only separate srst_nogate srst_open_drain connect_deassert_srst
program_debug
Info : clock speed 1800 kHz
Info : STLINK v2 JTAG v28 API v2 SWIM v18 VID 0x0483 PID 0x374B
Info : using stlink api v2
Info : Target voltage: 3.260972
Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
adapter speed: 1800 kHz
** Programming Started **
auto erase enabled
Info : device id = 0x10006434
Info : flash size = 2048kbytes
wrote 16384 bytes from file path/to/project.elf in 0.589551s (27.139 KiB/s)
** Programming Finished **
** Verify Started **
verified 13064 bytes in 0.328969s (38.781 KiB/s)
** Verified OK **
adapter speed: 1800 kHz
target halted due to debug-request, current mode: Thread
xPSR: 0x01000000 pc: 0x0800263c msp: 0x10000be0
shutdown command invoked
```


## Using GDB

For debugging your program on ARM Cortex-M device, we provide the `modm/gdbinit`
and `modm/openocd_gdbinit` files for the `arm-none-eabi-gdb` debugger connected
to your target via OpenOCD.

Two commands are provided for convenience:

- `restart` resets the device and halts.
- `rerun` resets the device and continues execution.

GDB is configured in TUI mode and continues running the target after attaching,
but *does not* load an ELF file! Please pass the ELF file as a command line
argument.

You can start your debug session by launching OpenOCD and then GDB:

```sh
# Run OpenOCD without any commands in the background
openocd -f modm/openocd.cfg
# Open another shell with your projects ELF file
arm-none-eabi-gdb -x modm/gdbinit -x modm/openocd_gdbinit path/to/project.elf
```

```
   ┌——main.cpp———————————————————————————————————————————————————————┐
  >│194             DRAW(x+1, y+3);                                  │
   │195             DRAW(x+2, y+3);                                  │
   │196     #else                                                    │
   │197             DRAW(x  , y  );                                  │
   │198     #endif                                                   │
   │199     #undef DRAW                                              │
   │200     }                                                        │
   │201                                                              │
   │202     static inline void drawScreen(framebuffer_t before, frame│
   └—————————————————————————————————————————————————————————————————┘
  >│0x80017a0 <game_of_life()+1692> strh.w r3, [r4, r12, lsl #1]     │
   │0x80017a4 <game_of_life()+1696> add    r0, lr                    │
   │0x80017a6 <game_of_life()+1698> ldr    r2, [r2, #0]              │
   │0x80017a8 <game_of_life()+1700> strh.w r3, [r2, r0, lsl #1]      │
   │0x80017ac <game_of_life()+1704> ldr    r3, [sp, #12]             │
   │0x80017ae <game_of_life()+1706> ldr    r2, [sp, #0]              │
   │0x80017b0 <game_of_life()+1708> add    r2, r3                    │
   │0x80017b2 <game_of_life()+1710> ldrb   r3, [r7, r1]              │
   │0x80017b4 <game_of_life()+1712> strb   r3, [r2, r1]              │
   └—————————————————————————————————————————————————————————————————┘
extended-r Remote target In: game_of_life         L194  PC: 0x80017a0

Program received signal SIGINT, Interrupt.
0x080017a0 in drawPixel (color=<optimized out>, y=42, x=578) at main.c
(gdb)
```

!!! warning "Be careful attaching to a running target"
    Due to the OpenOCD implementation, the target is halted for a very short
    period of time, while the device's debug peripheral is initialized.
    This time is dependent on the debug adapter and may range from just a few
    milliseconds to hundreds. Make sure your hardware can handle that!


## Using AvrDude

Unfortunately AvrDude does not support a user configuration file like OpenOCD
does, so there is no convenient one-line command to issue.
You have to use the wrapper support of the specific build system.


## Compiler Options

We maintain a common set of compiler options for all build system generator, so
that they all have feature parity. We currently only support compiling modm
with GCC for AVR, ARM Cortex-M and x86/posix with the options mentioned in the
[offical GCC options documentation][options].

You can find all compiler options inside the generated build script for your
project, the options presented here are only the most important ones.

#### Shared for C and C++

- `-W{all, extra}`: a basic set of warnings.
- `-Werror={format, maybe-uninitialized, overflow, sign-compare}`: these warnings are treated as errors.
- `-f{data, function}-sections`: puts data and functions into their own linker section.
- `-funsigned-{char, bitfields}`: modm tries to use `stdint.h` types everywhere, but just in case.

For *release builds*:

- `-Os`: optimize for smaller size.

For *debug builds*:

- `-Og`: optimize for debugging experience.

#### Only C

- `-std=gnu11`: use C11 with GNU extensions (for `asm volatile`).

#### Only C++

- `-std=c++17`: use C++17
- `-fno-exceptions`: no exceptions.
- `-fno-rtti`: no run-time type information.

#### Linker

- `--gc-section`: garbage collecting sections throws out a lot of unused data/code.
- `-L{linkdir} -Tlinkerscript.ld`: modm uses a custom linkerscript.

### Only AVR

- `-mmcu={target}`: the target to compile for.

### Only ARM Cortex-M

- `-mcpu=cortex-m{type}`: the target to compile for.
- `-mthumb`: only Thumb2 instruction set is supported.
- `-mfloat-abi=hard`: if FPU available use the fastest ABI available.
- `-mfpu=fpv{4, 5}-{sp}-d16`: single or double precision FPU.
- `-fsingle-precision-constant`: if SP-FPU, treat all FP constants as SP.
- `-Wdouble-promotion`: if SP-FPU, warn if FPs are promoted to doubles.

In addition, these linker options are added:

- `-nostartfiles`: modm implements its own startup script.
- `--specs=nano.specs`: use Newlib Nano.
- `--specs=nosys.specs`: modm does not use C stdlib features.
- `-wrap,_{calloc, malloc, realloc, free}_r`: reimplemented Newlib with our own allocator.


[options]: https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html

<!--Links-->
[bmp]: https://github.com/blacksphere/blackmagic
