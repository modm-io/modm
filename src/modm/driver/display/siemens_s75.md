# Siemens S75/CX75/C81/M75 Display

The controller in displays manufactured by Alps is a SOLOMON SYSTECH SSD1286
which can drive color displays of up to 132 x 176 pixels in 18 bit color.

The portrait mode is a bit more 'native' for this display because
`modm::GraphicDisplay` requests that the vertical resolution is
divisible by 8.

In portrait mode the connector is at the top.
In landscapeLeft  mode the connector is at the left border.
In landscapeRight mode the connector is at the right border.

Pinout at LCD:

```
 1  IN  CD         Command / Data                      Low = Command (Register) High = Data.
 2  IN  RESET      Resets the display.  Active low.    Low = Reset,             High = No reset.
 3  IN  CS         Chip select.         Active low.    Low = Chip is selected,  High = Chip is not selected.
 4  OUT VD         Sync Output.         Unused.
 5      GND        Supply
 6  IN  D0         Bit 0 of parallel data
 7      3V3 (2V9)  Supply
 8      GND        Supply
 9      VLED +     Supply Backlight
10      VLED -     Supply Backlight
11      GND        Supply
12  IN  RD         Read Strobe.         Must be tied high.
13  IN  WR         Write Strobe.        High-to-Low strobe write data to display memory.
14  IN  D1         Bit 1 of parallel data.
15  IN  D2         Bit 2 of parallel data.
16  IN  D3         Bit 3 of parallel data.
17  IN  D4         Bit 4 of parallel data.
18  IN  D5         Bit 5 of parallel data.
19  IN  D6         Bit 6 of parallel data.
20  IN  D7         Bit 7 of parallel data.
```

Pin 1 is unmarked. Pin 5 can be recognized by a thicker trace which is GND.

The backlight (VLED +, VLED -) consists of four white LEDs in series.
The forward voltage is about 12 volts.