# Graphic

Code for writing and drawing on graphical displays

## Coordinate System

{0, 0}
   ┌─────────────────────┐
   │ ┌───➤ X             │
   │ │                   │
   │ 🠛 Y                 │
   │                     │
   │                     │
   │                     │
   │                     │
   └─────────────────────┘
               e.g. {127, 63}

The size (width and height) of a graphics primitive always correspond
to its mathematical model, ignoring the rendered width. As everything
is drawn one pixel wide, the pixels will be rendered to the right and
below the mathematically defined points.

## Example

```cpp
#include <modm/ui/graphic.hpp>

buffer = GraphicBuffer<bool, 128, 64>
buffer << "Hello World"
ssd1306.write(buffer);

′′′