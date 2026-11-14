# Zephyr Memory Storage (ZMS)

A key-value store for small values on flash, from the Zephyr RTOS, with 32-bit
IDs. ZMS writes entries round-robin into sectors: the data grows from the start
of a sector, the allocation table entries (ATE) with ID, length, CRCs and a
cycle counter grow from the end. Full sectors are closed and their newest
entries are moved by the garbage collector into the current sector, one sector
always stays erased. Values of up to 8 bytes are stored inside the ATE.

```cpp
#include <modm/ext/zms/zms.hpp>

zms_fs fs;
// Four sectors at the end of the flash, each of the largest page size
modm::zms::setup(fs, Flash::Size - 4*Flash::MaxPageSize, 4, Flash::MaxPageSize);
if (zms_mount(&fs)) { /* error */ }

uint32_t boot_count{};
zms_read(&fs, 1, &boot_count, sizeof(boot_count));
boot_count++;
zms_write(&fs, 1, &boot_count, sizeof(boot_count));
```

ZMS works on any `modm::BlockDevice`, for example a range of the internal
flash or an external flash, with offsets relative to the block device:

```cpp
modm::platform::BdFlash<Flash::Size - 16*1024> storage;
// Four sectors of one erase block each
modm::zms::setup<storage>(fs, 0, 4);
```

`zms_mount()` formats an erased area. `modm::zms::setup()` without a block
device returns false if the sectors overlap the program image.

For storage written at runtime, put the sectors at the end of the internal
flash. On dual-bank devices this is the second bank, which the program does not
run from, so writes and erases don't stall the CPU (see `modm:platform:flash`).
On single-bank devices every flash operation stalls the CPU and by default masks
interrupts. A sector size of `Flash::MaxPageSize` works for both bank modes of
devices with a dual-bank option byte, like the G4 with 2 kB pages in dual-bank
and 4 kB pages in single-bank mode. On devices with large sectors at the end,
like the 128 kB sectors of the F2/F4/F7, an external flash may be the better
choice.

The API is documented in `<zephyr/kvss/zms.h>`, see also the [Zephyr documentation][zms].

On the internal flash ZMS relies on `BdFlash`, whose `program()` refuses
non-erased targets and skips units of only erased bytes, and on
`modm::readMarked()`, which reads words with a double ECC error as invalid.

Sectors must consist of whole erase blocks of the same size. Every value costs
one ATE of 16 bytes (or `BlockSizeWrite`, if larger) plus the value rounded
up to `BlockSizeWrite` bytes. A sector holds at most `sector_size - 5 ATEs`
bytes of data.

ZMS logs nothing by default. With the `modm:debug` module, define
`CONFIG_ZMS_LOG_LEVEL` to forward its messages to the modm logger: `1` for
errors, `2` also for warnings, `3` also for info and `4` also for debug
messages, for example:

```xml
<collect name="modm:build:cppdefines">CONFIG_ZMS_LOG_LEVEL=2</collect>
```

Most errors are about an invalid configuration, which the functions only
report as `-EINVAL`. A blocking logger changes the timing, so log via RTT or
a large buffer when testing power losses.

The ZMS functions are not thread-safe. If several fibers or interrupts access
the same `zms_fs`, implement `k_mutex_lock()` and `k_mutex_unlock()`, which
are weak empty functions.

[zms]: https://docs.zephyrproject.org/latest/services/storage/zms/zms.html
