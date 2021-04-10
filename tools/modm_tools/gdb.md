### GDB

For debugging your program on ARM Cortex-M device, this Python tool wraps
`arm-none-eabi-gdb` and connects it to a number of programmers running in the
background or remotely.

The tool can be called from the command line. Here is a typical use-case using
the openocd backend with the common configuration files:

```sh
python3 modm/modm_tools/gdb.py path/to/project.elf --ui=tui \
        -x modm/gdbinit -x modm/openocd_gdbinit \
        openocd -f modm/openocd.cfg
```

Or you can call the Python API directly:

```python
import sys
sys.path.append("modm")
from modm_tools import gdb
from modm_tools.openocd import OpenOcdBackend

backend = OpenOcdBackend(config="modm/openocd.cfg")
gdb.call(source="path/to/project.elf", backend=backend,
         config=["modm/gdbinit", "modm/openocd_gdbinit"], ui="tui")
```

This configuration starts the OpenOCD process in the background for you,
however, if you want to connect to an already running process, you can use the
remote backend with the `--host={ip or hostname}` via the command line:

```sh
# Extended-Remote running remotely
python3 modm/modm_tools/gdb.py path/to/project.elf -x modm/gdbinit --ui=tui \
        remote --host 123.45.67.89
```

Note that you can use different programmer backends to GDB:

```sh
# Black Magic Probe
python3 modm/modm_tools/gdb.py path/to/project.elf -x modm/gdbinit --ui=tui \
        bmp --port /dev/tty.usbserial-123

# CrashDebug for Post-Mortem debugging
python3 modm/modm_tools/gdb.py path/to/project.elf -x modm/gdbinit --ui=tui \
        crashdebug --binary-path modm/ext/crashcatcher/bins --dump coredump.txt
```

(\* *only ARM Cortex-M targets*)

Currently two UIs are implemented for debugging:

- `--ui=tui`: Text-based UI in your shell.
- `--ui=web`: Web-based UI in your browser, based on [gdbgui][].


#### Text UI

This UI is builtin to GDB and is therefore always available.

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


#### Web UI

This UI simply uses the [gdbgui][] project and works very well as an advanced
IDE-independent debugging solution.

![](https://github.com/cs01/gdbgui/raw/master/screenshots/gdbgui_animation.gif)

[gdbgui]: https://www.gdbgui.com
