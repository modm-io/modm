# Standard things
sp              := $(sp).x
dirstack_$(sp)  := $(d)
d               := $(dir)
BUILDDIRS       += $(BUILD_PATH)/$(d)
BUILDDIRS       += $(BUILD_PATH)/$(d)/usb
BUILDDIRS       += $(BUILD_PATH)/$(d)/usb/usb_lib

LIBMAPLE_INCLUDES := -I$(LIBMAPLE_PATH) -I$(LIBMAPLE_PATH)/usb -I$(LIBMAPLE_PATH)/usb/usb_lib

# Local flags
CFLAGS_$(d) = -I$(d) $(LIBMAPLE_INCLUDES) -Wall -Werror

# Local rules and targets
cSRCS_$(d) := adc.c                    \
              bkp.c                    \
              dac.c                    \
              dma.c                    \
              exti.c                   \
              flash.c                  \
              fsmc.c                   \
              gpio.c                   \
              iwdg.c                   \
              nvic.c                   \
              pwr.c		       \
              i2c.c                    \
              rcc.c                    \
              spi.c                    \
              syscalls.c               \
              systick.c                \
              timer.c                  \
              usart.c                  \
              util.c                   \
              usb/descriptors.c        \
              usb/usb.c                \
              usb/usb_callbacks.c      \
              usb/usb_hardware.c       \
              usb/usb_lib/usb_core.c   \
              usb/usb_lib/usb_init.c   \
              usb/usb_lib/usb_int.c    \
              usb/usb_lib/usb_mem.c    \
              usb/usb_lib/usb_regs.c

sSRCS_$(d) := exc.S

cFILES_$(d) := $(cSRCS_$(d):%=$(d)/%)
sFILES_$(d) := $(sSRCS_$(d):%=$(d)/%)

OBJS_$(d) := $(cFILES_$(d):%.c=$(BUILD_PATH)/%.o) $(sFILES_$(d):%.S=$(BUILD_PATH)/%.o)
DEPS_$(d) := $(OBJS_$(d):%.o=%.d)

$(OBJS_$(d)): TGT_CFLAGS := $(CFLAGS_$(d))
$(OBJS_$(d)): TGT_ASFLAGS :=

TGT_BIN += $(OBJS_$(d))

# Standard things
-include        $(DEPS_$(d))
d               := $(dirstack_$(sp))
sp              := $(basename $(sp))
