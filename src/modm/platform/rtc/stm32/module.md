# Real Time Clock (RTC)

The STM32 RTC implements a full calendar in hardware to provide a date and time
in binary-coded decimal (BCD) format. Several optimized methods are provided to
provide an efficient conversion from this hardware format to a software
representation.

The epoch of the RTC is chosen to be the 1st of January 1970 to be compatible
with UNIX timestamps. Since the year is limited to two BCD digits, the RTC will
roll over in 2070.

Note that the RTC hardware has no support for time zones, so you have to handle
that in software.


## Initialization

The RTC keeps running during a reset of the microcontroller when the backup
domain is powered. To prevent clock drift, the `initialize()` function will
check if the RTC is already running and only initialize the prescaler differs
from the programmed one. If the return value is `false` the RTC was already
initialized and running:

```cpp
struct SystemClock
{
	static constexpr uint32_t Rtc = 32'768;
};
const bool inited = Rtc::initialize<SystemClock>();
if (not inited) { /* RTC was already running from before reset */ }
```

To always initialize the RTC, set the `forced` argument to `true`:

```cpp
Rtc::initialize<SystemClock>(true);
```

To give the RTC an initial date and time, use the `setDateTime()` function. You
can use the compile time as a basic reference time, and only set the time
forward to not reset the time on every reset:

```cpp
constexpr auto cdt = modm::DateTime::fromBuildTime();
if (Rtc::dateTime() < cdt) Rtc::setDateTime(cdt);
```


## Accessing Date and Time

The RTC hardware provides the date and time in BCD format which can be
atomically read out with the `dateTime()` function, which returns a `DateTime`
object that can be used to access the individual date and time components:

```cpp
const auto dt = Rtc::dateTime();
dt.year_month_day();
dt.day_of_year();
dt.weekday();

dt.hours();
dt.minutes();
dt.seconds();
dt.subseconds();

// prints ISO encoding: 2024-12-22 18:39:21.342
MODM_LOG_INFO << dt << modm::endl;

// Efficient conversion to std::tm
const std::tm tm = dt.tm();
```

Please note that while the `DateTime` object provides methods to compute to
seconds and milliseconds since epoch, these are slow and should be avoided.
Instead, use the `Rtc::now()`, `Rtc::time_t()` and `Rtc::timeval()` functions
to access optimized and cached conversion methods which are much faster:

```cpp
const Rtc::time_point tp = Rtc::now();
// instead of Rtc::dateTime().time_since_epoch();
const std::time_t tt = Rtc::time_t();
// instead of Rtc::dateTime().time_t();
const struct timeval tv = Rtc::timeval();
// instead of Rtc::dateTime().timeval();
```
