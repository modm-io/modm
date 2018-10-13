# Interpolation Methods

Simple interpolation methods between a list of points.

## Linear Interpolation

Example:

```cpp
// Definition of the supporting points. The first type is the
// input type, the second the output type
using Point = modm::Pair<int8_t, int16_t>;

// Create a array of supporting points describing the curve.
const Point supportingPoints[6] =
{
    { 30, -200 },
    { 50, 0 },
    { 90, 50 },
    { 150, 2050 },
    { 200, 3000 },
    { 220, 20000 }
};

modm::interpolation::Linear<Point> value(supportingPoints, 6);
// ...

int8_t  a = 40;
int16_t b = value.interpolate(a);
```

Example with supporting points read from flash:

```cpp
// Definition of a supporting point
using Point = modm::Pair<int8_t, int16_t>;

// Array of supporting points in flash
FLASH_STORAGE(Point supportingPoints[6]) =
{
    { 30, -200 },
    { 50, 0 },
    { 90, 50 },
    { 150, 2050 },
    { 200, 3000 },
    { 220, 20000 }
};

// Create an interpolator object which reads the
// supporting points from flash.
modm::interpolation::Linear<Point, modm::accessor::Flash>
        value(modm::accessor::asFlash(supportingPoints), 6);
// ...

int8_t  a = 20;
int16_t b = value.interpolate(a);
```


## Lagrange Interpolation

Example:

```cpp
using Point = modm::Pair<float, float>;

// interpolate x^2 over the range of 1 <= x <= 3
Point points[3] =
{
    { 1, 1 },
    { 2, 4 },
    { 3, 9 }
};

modm::interpolation::Lagrange<Point> value(points, 3);
// ...

float output = value.interpolate(1.5f);
// output => 2.25;
```

!!!warning
    Only floating points types are allowed as second type of `modm::Pair`,
    otherwise the calculation will deliver wrong results!

See [Wikipedia](http://en.wikipedia.org/wiki/Lagrange_interpolation).