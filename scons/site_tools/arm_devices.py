devices = \
{
	# -------------------------------------------------------------------------
    # NXP ARM7TDMI devices Series LPC21xx, LPC22xx, LPC23xx, LPC24xx
	"lpc2129":
	{
		"defines": ["__ARM_LPC2000__"],
		"linkerscript": "arm7/lpc/linker/lpc2129.ld",
		"size": { "flash": 262144, "ram": 16384 },
	},
	"lpc2368":
	{
		"defines": ["__ARM_LPC2000__", "__ARM_LPC23_24__"],
		"linkerscript": "arm7/lpc/linker/lpc2368.ld",
		"size": { "flash": 524288, "ram": 32768 },
	},
	"lpc2468":
	{
		"defines": ["__ARM_LPC2000__", "__ARM_LPC23_24__"],
		"linkerscript": "arm7/lpc/linker/lpc2468.ld",
		"size": { "flash": 524288, "ram": 65536 },
	},
	
	# -------------------------------------------------------------------------
    # NXP Cortex-M0 devices Series LPC11xx and LPC11Cxx
    "lpc1112_301":
    {
        "defines": ["__ARM_LPC11XX__",],
        "linkerscript": "cortex_m0/lpc/linker/lpc1112_301.ld",
        "size": { "flash": 16384, "ram": 8192 },
    },

	"lpc1114_301":
	{
		"defines": ["__ARM_LPC11XX__",],
		"linkerscript": "cortex_m0/lpc/linker/lpc1114_301.ld",
		"size": { "flash": 32768, "ram": 8192 },
	},
	
    # Integrated CAN transceiver
	"lpc11c22_301":
	{
		"defines": ["__ARM_LPC11XX__", "__ARM_LPC11CXX__"],
		"linkerscript": "cortex_m0/lpc/linker/lpc11c22.ld",
		"size": { "flash": 16384, "ram": 8192 },
	},

    # Integrated CAN transceiver
    "lpc11c24_301":
    {
        "defines": ["__ARM_LPC11XX__", "__ARM_LPC11CXX__"],
        "linkerscript": "cortex_m0/lpc/linker/lpc11c24.ld",
        "size": { "flash": 32768, "ram": 8192 },
    },

    # -------------------------------------------------------------------------
    # NXP Cortex-M3 devices Series LPC13xx
    # TODO
	
    # -------------------------------------------------------------------------
    # NXP Cortex-M3 devices Series LPC17xx
	"lpc1769":
	{
		"defines": ["__ARM_LPC17XX__"],
		"linkerscript": "cortex_m3/lpc/linker/lpc1769.ld",
		"size": { "flash": 524288, "ram": 65536 },	# 32kB local SRAM + 2x16kB AHB SRAM
	},
	
	# -------------------------------------------------------------------------
	# AT91SAM7S
	"at91sam7s32": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7S__"],
		"linkerscript": "arm7/at91/linker/at91sam7s32.ld",
		"size": { "flash": 32768, "ram": 4096 },
	},
	"at91sam7s321": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7S__"],
		"linkerscript": "arm7/at91/linker/at91sam7s32.ld",
		"size": { "flash": 32768, "ram": 8192 },
	},
	"at91sam7s64": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7S__"],
		"linkerscript": "arm7/at91/linker/at91sam7s64.ld",
		"size": { "flash": 65536, "ram": 16384 },
	},
	"at91sam7s128": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7S__"],
		"linkerscript": "arm7/at91/linker/at91sam7s128.ld",
		"size": { "flash": 131072, "ram": 32768 },
	},
	"at91sam7s256": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7S__"],
		"linkerscript": "arm7/at91/linker/at91sam7s256.ld",
		"size": { "flash": 262144, "ram": 65536 },
	},
	"at91sam7s512": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7S__"],
		"linkerscript": "arm7/at91/linker/at91sam7s512.ld",
		"size": { "flash": 524288, "ram": 65536 },
	},
	
	# -------------------------------------------------------------------------
	# AT91SAM7X
	"at91sam7x128": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7X__"],
		"linkerscript": "arm7/at91/linker/at91sam7x128.ld",
		"size": { "flash": 131072, "ram": 32768 },
	},
	"at91sam7x256": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7X__"],
		"linkerscript": "arm7/at91/linker/at91sam7x256.ld",
		"size": { "flash": 262144, "ram": 65536 },
	},
	"at91sam7x512": 
	{
		"defines": ["__ARM_AT91__", "__ARM_AT91SAM7X__"],
		"linkerscript": "arm7/at91/linker/at91sam7s512.ld",
		"size": { "flash": 524288, "ram": 131072 },
	},
	
	# -------------------------------------------------------------------------
	# STM32F103 p s
	# 
	# Pins (p):
	#  T |  36 pins
	#  C |  48 pins
	#  R |  64 pins
	#  V | 100 pins
	#  Z | 144 pins
	# 
	# Size (s):
	#  4 |  16 kB Flash,  6 kB RAM 	low density line (T, C, R)
	#  6 |  32 kB Flash, 10 kB RAM
	#  8 |  64 kB Flash, 20 kB RAM	medium density (T, C, R, V)
	#  B | 128 kB Flash, 20 kB RAM
	#  C | 256 kB Flash, 48 kB RAM	high density (R, V, Z)
	#  D | 384 kB Flash, 64 kB RAM
	#  E | 512 kB Flash, 64 kB RAM
	#  F | 768 kB Flash, 96 kB RAM	xl density (R, V, Z)
	#  G |   1 MB Flash, 96 kB RAM
	# 
	"stm32f103_4": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_LD", "STM32_LOW_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_4.ld",
		"size": { "flash": 16384, "ram": 6144 },
	},
	"stm32f103_6": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_LD", "STM32_LOW_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_6.ld",
		"size": { "flash": 32768, "ram": 10240 },
	},
	"stm32f103_8": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_MD", "STM32_MEDIUM_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_8.ld",
		"size": { "flash": 65536, "ram": 20480 },
	},
	"stm32f103_b": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_MD", "STM32_MEDIUM_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_b.ld",
		"size": { "flash": 131072, "ram": 20480 },
	},
	"stm32f103_c": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_HD", "STM32_HIGH_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_c.ld",
		"size": { "flash": 262144, "ram": 49152 },
	},
	"stm32f103_d": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_HD", "STM32_HIGH_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_d.ld",
		"size": { "flash": 393216, "ram": 65536 },
	},
	"stm32f103_e": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_HD", "STM32_HIGH_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_e.ld",
		"size": { "flash": 524288, "ram": 65536 },
	},
	"stm32f103_f": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_XL", "STM32_XL_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_f.ld",
		"size": { "flash": 786432, "ram": 98304 },
	},
	"stm32f103_g": 
	{
		"defines": ["__STM32F103__", "__ARM_STM32__", "STM32F10X", "STM32F10X_XL", "STM32_XL_DENSITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f103_g.ld",
		"size": { "flash": 1048576, "ram": 98304 },
	},
	
	# STM32F105 p s		(pins: R, V)
	# 
	# Size (s):
	#  8 |  64 kB Flash, 20 kB RAM
	#  B | 128 kB Flash, 32 kB RAM
	#  C | 256 kB Flash, 64 kB RAM
	# 
	"stm32f105_8": 
	{
		"defines": ["__STM32F105__", "__ARM_STM32__", "STM32F10X", "STM32F10X_CL", "STM32_CONNECTIVITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f105_8.ld",
		"size": { "flash": 65536, "ram": 20480 },
	},
	"stm32f105_b": 
	{
		"defines": ["__STM32F105__", "__ARM_STM32__", "STM32F10X", "STM32F10X_CL", "STM32_CONNECTIVITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f105_b.ld",
		"size": { "flash": 131072, "ram": 32768 },
	},
	"stm32f105_c": 
	{
		"defines": ["__STM32F105__", "__ARM_STM32__", "STM32F10X", "STM32F10X_CL", "STM32_CONNECTIVITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f105_c.ld",
		"size": { "flash": 262144, "ram": 65536 },
	},
	
	# STM32F107 p s		(pins: R, V)
	# 
	# Size (s):
	#  B | 128 kB Flash, 48 kB RAM
	#  C | 256 kB Flash, 64 kB RAM
	# 
	"stm32f107_b": 
	{
		"defines": ["__STM32F107__", "__ARM_STM32__", "STM32F10X", "STM32F10X_CL", "STM32_CONNECTIVITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f107_b.ld",
		"size": { "flash": 131072, "ram": 49152 },
	},
	"stm32f107_c": 
	{
		"defines": ["__STM32F107__", "__ARM_STM32__", "STM32F10X", "STM32F10X_CL", "STM32_CONNECTIVITY"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f107_c.ld",
		"size": { "flash": 262144, "ram": 65536 },
	},
	
	# -------------------------------------------------------------------------
	# STM32F205 p s
	# 
	# Pins (p):
	#  R |  64 pins
	#  V | 100 pins
	#  Z | 144 pins
	# 
	# Size (s):
	#  B | 128 kB Flash,  48+16 kB RAM  (R, V)
	#  C | 256 kB Flash,  80+16 kB RAM  (R, V, Z)
	#  E | 512 kB Flash, 112+16 kB RAM  (R, V, Z)
	#  F | 768 kB Flash, 112+16 kB RAM  (R, V, Z)
	#  G |   1 MB Flash, 112+16 kB RAM  (R, V, Z)
	# 
	"stm32f205_b":
	{
		"defines": ["__STM32F205__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f205_b.ld",
		"size": { "flash": 131072, "ram": 49152 },
	},
	"stm32f205_c":
	{
		"defines": ["__STM32F205__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f205_c.ld",
		"size": { "flash": 262144, "ram": 81920 },
	},
	"stm32f205_e":
	{
		"defines": ["__STM32F205__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f205_e.ld",
		"size": { "flash": 524288, "ram": 114688 },
	},
	"stm32f205_f":
	{
		"defines": ["__STM32F205__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f205_f.ld",
		"size": { "flash": 786432, "ram": 114688 },
	},
	"stm32f205_g":
	{
		"defines": ["__STM32F205__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f205_g.ld",
		"size": { "flash": 1048576, "ram": 114688 },
	},
	
	# STM32F207 p s
	# 
	# Pins (p):
	#  V | 100 pins
	#  Z | 144 pins
	#  I | 176 pins
	# 
	# Size (s):
	#  C | 256 kB Flash, 112+16 kB RAM
	#  E | 512 kB Flash, 112+16 kB RAM
	#  F | 768 kB Flash, 112+16 kB RAM
	#  G |   1 MB Flash, 112+16 kB RAM
	# 
	"stm32f207_c":
	{
		"defines": ["__STM32F207__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f207_c.ld",
		"size": { "flash": 262144, "ram": 114688 },
	},
	"stm32f207_e":
	{
		"defines": ["__STM32F207__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f207_e.ld",
		"size": { "flash": 524288, "ram": 114688 },
	},
	"stm32f207_f":
	{
		"defines": ["__STM32F207__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f207_f.ld",
		"size": { "flash": 786432, "ram": 114688 },
	},
	"stm32f207_g":
	{
		"defines": ["__STM32F207__", "__ARM_STM32__", "STM32F2XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f207_g.ld",
		"size": { "flash": 1048576, "ram": 114688 },
	},
	
	# -------------------------------------------------------------------------
	# STM32F405 p s
	# 
	# Pins (p):
	#  R |  64 pins
	#  V | 100 pins
	#  Z | 144 pins
	# 
	# Size (s):
	#  G |   1 MB Flash, 112+64+16 kB RAM
	# 
	"stm32f405_g":
	{
		"defines": ["__STM32F405__", "__ARM_STM32__", "STM32F4XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f4xx_g.ld",
		"size": { "flash": 1048576, "ram": 114688 },
	},
	
	# STM32F407 p s
	# 
	# Pins (p):
	#  V | 100 pins
	#  Z | 144 pins
	#  I | 176 pins
	# 
	# Size (s):
	#  E | 512 kB Flash, 112+64+16 kB RAM
	#  G |   1 MB Flash, 112+64+16 kB RAM
	# 
	"stm32f407_e":
	{
		"defines": ["__STM32F407__", "__ARM_STM32__", "STM32F4XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f4xx_e.ld",
		"size": { "flash": 524288, "ram": 114688 },
	},
	"stm32f407_g":
	{
		"defines": ["__STM32F407__", "__ARM_STM32__", "STM32F4XX"],
		"linkerscript": "cortex_m3/stm32/linker/stm32f4xx_g.ld",
		"size": { "flash": 1048576, "ram": 114688 },
	},
}
