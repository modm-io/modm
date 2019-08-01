# General Purpose Registers

Data structures to provide a native register abstraction.

These data structures are used to describe the relationship of single bits,
bit groups and bit configurations in registers with type-safe access.

Registers can be made up of three things:

- Bits: a single bit (position N),
- Configurations: a combination of bits where the meaning does not correspond to its numeric value (position [N, M])
- Values: a numeric value (position [N, M])

Example of an 8-bit register called Control

```
   7    6    5      4      3      2      1    0
| EN | FS | PRE1 | PRE0 | DEL2 | DEL1 | DEL0 | |
```

- Bit 7: Enable
- Bit 6: Full Scale
- Configuration [5, 4]: Prescaler
    - 00: Divide by 1
    - 01: Divide by 2
    - 10: Divide by 4
    - 11: Divide by 8
- Value [3, 1]: Start-Up Delay in ms

## Register Bits

The bits can be modelled using strongly-typed enums and the Flags template class as follows:

```cpp
enum class
Control : uint8_t
{
    EN = Bit7,	///< bit documentation
    FS = Bit6,

    PRE1 = Bit5,
    PRE0 = Bit4,

    DEL2 = Bit3,
    DEL1 = Bit2,
    DEL0 = Bit1,
};
MODM_FLAGS8(Control);
// expands to
// typedef modm::Flags8< Control >  Control_t;
// and some enum operator overloading magic
```

You can handle all its register bits as you would expect:

```cpp
Control_t control = Control::EN;
control = Control::EN | Control::FS;
control &= ~Control::FS;
control |= Control::FS;
control ^= Control::PRE1;
bool isSet = control & Control::FS;

control.reset(Control::PRE1 | Control::PRE0);
control.set(Control::DEL0);

bool noneSet = control.none(Control::PRE1 | Control::PRE0);
bool allSet = control.all(Control::EN | Control::FS);
```

You still get raw access if you really need it:

```cpp
uint8_t raw = control.value; // the underlying type
control.value = 0x24;
```

The access is type-safe, you cannot use bits from two different registers:

```cpp
enum class Control2 : uint8_t
{
    DIS = Bit4,
    HS = Bit3,
};
MODM_FLAGS8(Control2);

auto control = Control::EN | Control2::HS; // compile error
```

You can even overload functions on argument type now:

```cpp
void write(Control_t control);
void write(Control2_t control);

write(Control::EN | Control::FS);  // calls #1
write(Control2::DIS);              // calls #2
```


## Register Configurations

Configurations are also described as a strongly-typed enum and then wrapped into the Configuration template class.

```cpp
enum class
Prescaler : uint8_t
{
    Div1 = 0,				///< configuration documentation
    Div2 = int(Control::PRE0),
    Div4 = int(Control::PRE1),
    Div8 = int(Control::PRE1) | int(Control::PRE0),
};
typedef Configuration< Control_t, Prescaler, (Bit5 | Bit4) >  Prescaler_t;
```

The `Prescaler` enum values are already shifted in this example (hence the `(Bit5 | Bit4)` mask), however you can also declare the prescaler values non-shifted and let the wrapper shift it:

```cpp
enum class Prescaler : uint8_t
{
    Div1 = 0,
    Div2 = 1,
    Div4 = 2,
    Div8 = 3,
};
typedef Configuration<Control_t, Prescaler, 0b11, 4> Prescaler_t;
```

Why? If you have two or more configurations with the same selections in the same register,  you can simply add another one:

```cpp
typedef Configuration< Control_t, Prescaler, 0b11, 6 >  Prescaler2_t;
```

Configurations can be used inline:

```cpp
Control_t control = Control::EN | Prescaler_t(Prescaler::Div2);
Control_t control &= ~Prescaler_t::mask();
```

But do not have to:

```cpp
Prescaler_t::set(control, Prescaler::Div2);
Prescaler_t::reset(control);
Prescaler prescaler = Prescaler_t::get(control);
```


## Register Values

Values are described using the Value template class which masks and shifts the value as required.
In our example the value has a width of 3 bits and needs to be shifted 1 bit:

```cpp
typedef Value< Control_t, 3, 1 >  Delay_t;
```

This can be used the same way as the Configuration:

```cpp
Control_t control = Control::EN | Prescaler_t(Prescaler::Div2) | Delay_t(4);
Control_t control &= ~Delay_t::mask();

Delay_t::set(control, 7);
Delay_t::reset(control);
uint8_t delay = Delay_t::get(control);
```

See [Typesafe Register Access in C++](https://blog.salkinium.com/typesafe-register-access-in-c++/)
for a more detailed background on this implementation.
