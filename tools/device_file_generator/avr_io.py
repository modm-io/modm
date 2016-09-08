# -*- coding: utf-8 -*-
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# The file is part of the xpcc library and is released under the 3-clause BSD
# license. See the file `LICENSE` for the full license governing this code.
# -----------------------------------------------------------------------------

xmega_pins = \
[
	{
		"type": "a1",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 255 },
			{ "port": "F", "mask": 255 },
			{ "port": "H", "mask": 255 },
			{ "port": "J", "mask": 255 },
			{ "port": "K", "mask": 255 },
			
			{ "port": "Q", "mask": 15 },
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "a3",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 255 },
			{ "port": "F", "mask": 255 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "a4",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 15 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 15 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "b1",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 7 },
			{ "port": "E", "mask": 255 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "b3",
		"gpio":
		[
			{ "port": "B", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 3 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "c3",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 255 },
			{ "port": "F", "mask": 255 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "c4",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 15 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 15 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "d3",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 255 },
			{ "port": "F", "mask": 255 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "d4",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "B", "mask": 15 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			{ "port": "E", "mask": 15 },
			
			{ "port": "R", "mask": 3 }
		]
	},
	
	{
		"type": "e5",
		"gpio":
		[
			{ "port": "A", "mask": 255 },
			{ "port": "C", "mask": 255 },
			{ "port": "D", "mask": 255 },
			
			{ "port": "R", "mask": 3 }
		]
	}
]

# the pins of the peripherals are always on the same pins on all ports	
xmega_peripheral_pins = \
{
	"spi":
	[
		{ "id": "4", "name": "ss", "dir": "out" },
		{ "id": "5", "name": "mosi", "dir": "out", "remap": False },
		{ "id": "6", "name": "miso", "dir": "in" },
		{ "id": "7", "name": "sck", "dir": "out", "remap": False },
		
		{ "id": "7", "name": "mosi", "dir": "out", "remap": True },
		{ "id": "5", "name": "sck", "dir": "out", "remap": True }
	],
	"usart":
	[
		{ "id": "1", "name": "xck", "dir": "out", "instance": "0", "remap": False },
		{ "id": "2", "name": "rxd", "dir": "in", "instance": "0", "remap": False },
		{ "id": "3", "name": "txd", "dir": "out", "instance": "0", "remap": False },
		
		{ "id": "5", "name": "xck", "dir": "out", "instance": "0", "remap": True },
		{ "id": "6", "name": "rxd", "dir": "in", "instance": "0", "remap": True },
		{ "id": "7", "name": "txd", "dir": "out", "instance": "0", "remap": True },
		
		{ "id": "5", "name": "xck", "dir": "out", "instance": "1" },
		{ "id": "6", "name": "rxd", "dir": "in", "instance": "1" },
		{ "id": "7", "name": "txd", "dir": "out", "instance": "1" }
	],
	"twi":
	[
		{ "id": "0", "name": "sda", "dir": "io" },
		{ "id": "1", "name": "scl", "dir": "out" }
	],
	"tc":
	[
		{ "id": "0", "name": "OCA", "dir": "out", "instance": "0", "remap": False },
		{ "id": "1", "name": "OCB", "dir": "out", "instance": "0", "remap": False },
		{ "id": "2", "name": "OCC", "dir": "out", "instance": "0", "remap": False },
		{ "id": "3", "name": "OCD", "dir": "out", "instance": "0", "remap": False },
		
		{ "id": "4", "name": "OCA", "dir": "out", "instance": "0", "remap": True },
		{ "id": "5", "name": "OCB", "dir": "out", "instance": "0", "remap": True },
		{ "id": "6", "name": "OCC", "dir": "out", "instance": "0", "remap": True },
		{ "id": "7", "name": "OCD", "dir": "out", "instance": "0", "remap": True },
		
		{ "id": "4", "name": "OCA", "dir": "out", "instance": "1" },
		{ "id": "5", "name": "OCB", "dir": "out", "instance": "1" }
	],
	"awex":
	[
		{ "id": "0", "name": "ALS", "dir": "out" },
		{ "id": "1", "name": "AHS", "dir": "out" },
		
		{ "id": "2", "name": "BLS", "dir": "out" },
		{ "id": "3", "name": "BHS", "dir": "out" },
		
		{ "id": "4", "name": "CLS", "dir": "out" },
		{ "id": "5", "name": "CHS", "dir": "out" },
		
		{ "id": "6", "name": "DLS", "dir": "out" },
		{ "id": "7", "name": "DHS", "dir": "out" }
	],
	"usb":
	[
		{ "id": "0", "name": "DM", "dir": "io" },
		{ "id": "1", "name": "DP", "dir": "io" }
	]
}

pins = \
[
	{
		"devices": ["atmega8", "atmega8a"],
		"extint": 
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" }
		],
		"spi":
		[
			{ "port": "B", "id": "3", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "4", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "2", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "5", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "4", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["atmega16", "atmega16a", "atmega32", "atmega32a"],
		"extint": 
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" }
		],
		"spi":
		[
			{ "port": "B", "id": "5", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "6", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "1", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["atmega64", "atmega64a", "atmega128", "atmega128a"],
		"extint": 
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" }
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "E", "id": "2", "name": "xck", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["atmega8u2", "atmega16u2", "atmega32u2"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "C", "id": "6", "int": "8" },
			{ "port": "C", "id": "5", "int": "9" },
			{ "port": "C", "id": "4", "int": "10" },
			{ "port": "C", "id": "2", "int": "11" },
			
			{ "port": "D", "id": "5", "int": "12" },
		],
		"extint": 
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "D", "id": "4", "int": "4" },
			{ "port": "D", "id": "5", "int": "5" },
			{ "port": "D", "id": "6", "int": "6" },
			{ "port": "D", "id": "7", "int": "7" }
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega8hva", "atmega16hva"],
		"pcint": [],
		"extint": 
		[
			{ "port": "C", "id": "0", "int": "0" },
			{ "port": "B", "id": "2", "int": "1" },
			{ "port": "B", "id": "3", "int": "2" },
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "C", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "C", "id": "0", "name": "txd", "dir": "out" }
		]
	},
	
	{
		"devices": ["atmega16c1", "atmega16m1", "atmega32c1", "atmega32m1", "atmega64c1", "atmega64m1"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "C", "id": "0", "int": "8" },
			{ "port": "C", "id": "1", "int": "9" },
			{ "port": "C", "id": "2", "int": "10" },
			{ "port": "C", "id": "3", "int": "11" },
			{ "port": "C", "id": "4", "int": "12" },
			{ "port": "C", "id": "5", "int": "13" },
			{ "port": "C", "id": "6", "int": "14" },
			{ "port": "C", "id": "7", "int": "15" },
			
			{ "port": "D", "id": "0", "int": "16" },
			{ "port": "D", "id": "1", "int": "17" },
			{ "port": "D", "id": "2", "int": "18" },
			{ "port": "D", "id": "3", "int": "19" },
			{ "port": "D", "id": "4", "int": "20" },
			{ "port": "D", "id": "5", "int": "21" },
			{ "port": "D", "id": "6", "int": "22" },
			{ "port": "D", "id": "7", "int": "23" },
			
			{ "port": "E", "id": "0", "int": "24" },
			{ "port": "E", "id": "1", "int": "25" },
			{ "port": "E", "id": "2", "int": "26" }
		],
		"extint": 
		[
			{ "port": "D", "id": "6", "int": "0" },
			{ "port": "B", "id": "2", "int": "1" },
			{ "port": "B", "id": "5", "int": "2" },
			{ "port": "C", "id": "0", "int": "3" },
		],
		"spi":
		[
			{ "port": "B", "id": "3", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "2", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "4", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "1", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "4", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" }
		]
	},
	
	{
		"devices": ["atmega16hvb", "atmega16hvbrevb", "atmega32hvb", "atmega32hvbrevb"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			
			{ "port": "B", "id": "0", "int": "4" },
			{ "port": "B", "id": "1", "int": "5" },
			{ "port": "B", "id": "2", "int": "6" },
			{ "port": "B", "id": "3", "int": "7" },
			{ "port": "B", "id": "4", "int": "8" },
			{ "port": "B", "id": "5", "int": "9" },
			{ "port": "B", "id": "6", "int": "10" },
			{ "port": "B", "id": "7", "int": "11" }
		],
		"extint": 
		[
			{ "port": "C", "id": "0", "int": "0" },
			{ "port": "C", "id": "1", "int": "1" },
			{ "port": "C", "id": "2", "int": "2" },
			{ "port": "C", "id": "3", "int": "3" },
		],
		"spi":
		[
			{ "port": "B", "id": "6", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "7", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "4", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "3", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["atmega32hve2", "atmega64hve2"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			
			{ "port": "B", "id": "0", "int": "2" },
			{ "port": "B", "id": "1", "int": "3" },
			{ "port": "B", "id": "2", "int": "4" },
			{ "port": "B", "id": "3", "int": "5" },
			{ "port": "B", "id": "4", "int": "6" },
			{ "port": "B", "id": "5", "int": "7" },
			{ "port": "B", "id": "6", "int": "8" },
			{ "port": "B", "id": "7", "int": "9" },
		],
		"extint": 
		[
			{ "port": "B", "id": "7", "int": "0" },
		],
		"spi":
		[
			{ "port": "B", "id": "6", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "7", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "B", "id": "1", "name": "rxd", "dir": "in" },
			{ "port": "B", "id": "3", "name": "txd", "dir": "out" }
		]
	},
	
	{
		"devices": ["atmega16u4", "atmega32u4"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
		],
		"extint": 
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			
			{ "port": "E", "id": "6", "int": "6" },
		],
		"spi":
		[
			{ "port": "B", "id": "3", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "2", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega48", "atmega48a", "atmega48p", "atmega48pa",
					"atmega88", "atmega88a", "atmega88p", "atmega88pa",
					"atmega168", "atmega168a", "atmega168p", "atmega168pa",
					"atmega328", "atmega328a", "atmega328p", "atmega328pa"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "C", "id": "0", "int": "8" },
			{ "port": "C", "id": "1", "int": "9" },
			{ "port": "C", "id": "2", "int": "10" },
			{ "port": "C", "id": "3", "int": "11" },
			{ "port": "C", "id": "4", "int": "12" },
			{ "port": "C", "id": "5", "int": "13" },
			{ "port": "C", "id": "6", "int": "14" },
			
			{ "port": "D", "id": "0", "int": "16" },
			{ "port": "D", "id": "1", "int": "17" },
			{ "port": "D", "id": "2", "int": "18" },
			{ "port": "D", "id": "3", "int": "19" },
			{ "port": "D", "id": "4", "int": "20" },
			{ "port": "D", "id": "5", "int": "21" },
			{ "port": "D", "id": "6", "int": "22" },
			{ "port": "D", "id": "7", "int": "23" },
		],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
		],
		"spi":
		[
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "4", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "2", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "4", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "5", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "4", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega64rfa1", "atmega64rfr2",
					"atmega128rfa1", "atmega128rfr2",
					"atmega256rfa1", "atmega256rfr2"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "E", "id": "0", "int": "8" },
		],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" },
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega162"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "C", "id": "0", "int": "8" },
			{ "port": "C", "id": "1", "int": "9" },
			{ "port": "C", "id": "2", "int": "10" },
			{ "port": "C", "id": "3", "int": "11" },
			{ "port": "C", "id": "4", "int": "12" },
			{ "port": "C", "id": "5", "int": "13" },
			{ "port": "C", "id": "6", "int": "14" },
			{ "port": "C", "id": "7", "int": "15" },
		],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
			{ "port": "E", "id": "0", "int": "2" },
		],
		"spi":
		[
			{ "port": "B", "id": "5", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "6", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "4", "name": "xck", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "B", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "B", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "2", "name": "xck", "dir": "out" }
		]
	},
	
	{
		"devices": ["atmega164", "atmega164a", "atmega164p", "atmega164pa",
					"atmega324", "atmega324a", "atmega324p", "atmega324pa",
					"atmega644", "atmega644a", "atmega644p", "atmega644pa",
					"atmega1284", "atmega1284a", "atmega1284p", "atmega1284pa"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			{ "port": "B", "id": "4", "int": "12" },
			{ "port": "B", "id": "5", "int": "13" },
			{ "port": "B", "id": "6", "int": "14" },
			{ "port": "B", "id": "7", "int": "15" },
			
			{ "port": "C", "id": "0", "int": "16" },
			{ "port": "C", "id": "1", "int": "17" },
			{ "port": "C", "id": "2", "int": "18" },
			{ "port": "C", "id": "3", "int": "19" },
			{ "port": "C", "id": "4", "int": "20" },
			{ "port": "C", "id": "5", "int": "21" },
			{ "port": "C", "id": "6", "int": "22" },
			{ "port": "C", "id": "7", "int": "23" },
			
			{ "port": "D", "id": "0", "int": "24" },
			{ "port": "D", "id": "1", "int": "25" },
			{ "port": "D", "id": "2", "int": "26" },
			{ "port": "D", "id": "3", "int": "27" },
			{ "port": "D", "id": "4", "int": "28" },
			{ "port": "D", "id": "5", "int": "29" },
			{ "port": "D", "id": "6", "int": "30" },
			{ "port": "D", "id": "7", "int": "31" },
		],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
		],
		"spi":
		[
			{ "port": "B", "id": "5", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "6", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "B", "id": "0", "name": "xck", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "4", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega165", "atmega165a", "atmega165p", "atmega165pa",
					"atmega325", "atmega325a", "atmega325p", "atmega325pa",
					"atmega645", "atmega645a", "atmega645p", "atmega645pa",
					"atmega169", "atmega169a", "atmega169p", "atmega169pa",
					"atmega329", "atmega329a", "atmega329p", "atmega329pa",
					"atmega649", "atmega649a", "atmega649p", "atmega649pa"],
		"pcint":
		[
			{ "port": "E", "id": "0", "int": "0" },
			{ "port": "E", "id": "1", "int": "1" },
			{ "port": "E", "id": "2", "int": "2" },
			{ "port": "E", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			{ "port": "B", "id": "4", "int": "12" },
			{ "port": "B", "id": "5", "int": "13" },
			{ "port": "B", "id": "6", "int": "14" },
			{ "port": "B", "id": "7", "int": "15" },
		],
		"extint":
		[
			{ "port": "D", "id": "1", "int": "0" },
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "E", "id": "2", "name": "xck", "dir": "out" }
		],
		"usi":
		[
			{ "port": "E", "id": "4", "name": "usck", "dir": "out" },
			{ "port": "E", "id": "6", "name": "do", "dir": "out" },
			{ "port": "E", "id": "5", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["atmega3250", "atmega3250a", "atmega3250p", "atmega3250pa",
					"atmega6450", "atmega6450a", "atmega6450p", "atmega6450pa",
					"atmega3290", "atmega3290a", "atmega3290p", "atmega3290pa",
					"atmega6490", "atmega6490a", "atmega6490p", "atmega6490pa"],
		"pcint":
		[
			{ "port": "E", "id": "0", "int": "0" },
			{ "port": "E", "id": "1", "int": "1" },
			{ "port": "E", "id": "2", "int": "2" },
			{ "port": "E", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			{ "port": "B", "id": "4", "int": "12" },
			{ "port": "B", "id": "5", "int": "13" },
			{ "port": "B", "id": "6", "int": "14" },
			{ "port": "B", "id": "7", "int": "15" },
			
			{ "port": "H", "id": "0", "int": "16" },
			{ "port": "H", "id": "1", "int": "17" },
			{ "port": "H", "id": "2", "int": "18" },
			{ "port": "H", "id": "3", "int": "19" },
			{ "port": "H", "id": "4", "int": "20" },
			{ "port": "H", "id": "5", "int": "21" },
			{ "port": "H", "id": "6", "int": "22" },
			{ "port": "H", "id": "7", "int": "23" },
			
			{ "port": "J", "id": "0", "int": "24" },
			{ "port": "J", "id": "1", "int": "25" },
			{ "port": "J", "id": "2", "int": "26" },
			{ "port": "J", "id": "3", "int": "27" },
			{ "port": "J", "id": "4", "int": "28" },
			{ "port": "J", "id": "5", "int": "29" },
			{ "port": "J", "id": "6", "int": "30" },
		],
		"extint":
		[
			{ "port": "D", "id": "1", "int": "0" },
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "E", "id": "2", "name": "xck", "dir": "out" }
		],
		"usi":
		[
			{ "port": "E", "id": "4", "name": "usck", "dir": "out" },
			{ "port": "E", "id": "6", "name": "do", "dir": "out" },
			{ "port": "E", "id": "5", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["atmega1281", "atmega2561"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "E", "id": "0", "int": "8" },
		],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" },
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega640", "atmega1280", "atmega2560"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "E", "id": "0", "int": "8" },
			
			{ "port": "J", "id": "0", "int": "9" },
			{ "port": "J", "id": "1", "int": "10" },
			{ "port": "J", "id": "2", "int": "11" },
			{ "port": "J", "id": "3", "int": "12" },
			{ "port": "J", "id": "4", "int": "13" },
			{ "port": "J", "id": "5", "int": "14" },
			{ "port": "J", "id": "6", "int": "15" },
			
			{ "port": "K", "id": "0", "int": "16" },
			{ "port": "K", "id": "1", "int": "17" },
			{ "port": "K", "id": "2", "int": "18" },
			{ "port": "K", "id": "3", "int": "19" },
			{ "port": "K", "id": "4", "int": "20" },
			{ "port": "K", "id": "5", "int": "21" },
			{ "port": "K", "id": "6", "int": "22" },
			{ "port": "K", "id": "7", "int": "23" },
		],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" },
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "E", "id": "2", "name": "xck", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"uart2":
		[
			{ "port": "H", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "H", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "H", "id": "2", "name": "xck", "dir": "out" }
		],
		"uart3":
		[
			{ "port": "J", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "J", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "J", "id": "2", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["atmega8515"],
		"pcint": [],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
			{ "port": "E", "id": "0", "int": "2" },
		],
		"spi":
		[
			{ "port": "B", "id": "5", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "6", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "01", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "4", "name": "xck", "dir": "out" }
		]
	},
	
	{
		"devices": ["atmega8535"],
		"pcint": [],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
		],
		"spi":
		[
			{ "port": "B", "id": "5", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "6", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "4", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "B", "id": "0", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "1", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["atmega644rfr2", "atmega1284rfr2", "atmega2564rfr2"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "F", "id": "0", "int": "8" },
		],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" },
		],
		"spi":
		[
			{ "port": "B", "id": "3", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "2", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "E", "id": "2", "name": "xck", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	# ATtiny devices
	
	{
		"devices": ["attiny4", "attiny5", "attiny9", "attiny10"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
		],
		"extint":
		[
			{ "port": "B", "id": "2", "int": "0" },
		],
		#"spi":
		#[
		#	{ "port": "B", "id": "0", "name": "miso", "dir": "in" },
		#	{ "port": "B", "id": "1", "name": "mosi", "dir": "out" },
		#	{ "port": "B", "id": "2", "name": "sck", "dir": "out" }
		#]
	},
	
	{
		"devices": ["attiny13", "attiny13a"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
		],
		"extint":
		[
			{ "port": "B", "id": "1", "int": "0" },
		],
	},
	
	{
		"devices": ["attiny20"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "4", "int": "8" },
			{ "port": "B", "id": "5", "int": "9" },
			{ "port": "B", "id": "6", "int": "10" },
			{ "port": "B", "id": "7", "int": "11" },
		],
		"extint":
		[
			{ "port": "B", "id": "2", "int": "0" },
		],
		"spi":
		[
			{ "port": "A", "id": "1", "name": "miso", "dir": "in" },
			{ "port": "A", "id": "2", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "6", "name": "ss", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "A", "id": "7", "name": "scl", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["attiny24", "attiny24a",
					"attiny44", "attiny44a",
					"attiny84", "attiny84a"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "4", "int": "8" },
			{ "port": "B", "id": "5", "int": "9" },
			{ "port": "B", "id": "6", "int": "10" },
			{ "port": "B", "id": "7", "int": "11" },
		],
		"extint":
		[
			{ "port": "B", "id": "2", "int": "0" },
		],
		"spi":
		[
			{ "port": "B", "id": "6", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "5", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "4", "name": "sck", "dir": "out" }
		],
		"usi":
		[
			{ "port": "A", "id": "4", "name": "usck", "dir": "out" },
			{ "port": "A", "id": "5", "name": "do", "dir": "out" },
			{ "port": "A", "id": "6", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["attiny25", "attiny45", "attiny85"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
		],
		"extint":
		[
			{ "port": "B", "id": "2", "int": "0" },
		],
		#"spi":
		#[
		#	{ "port": "B", "id": "1", "name": "miso", "dir": "in" },
		#	{ "port": "B", "id": "2", "name": "mosi", "dir": "out" },
		#	{ "port": "B", "id": "3", "name": "sck", "dir": "out" }
		#],
		"usi":
		[
			{ "port": "B", "id": "2", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "1", "name": "do", "dir": "out" },
			{ "port": "B", "id": "0", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["attiny26"],
		"pcint":
		[
			{ "port": "A", "id": "3", "int": "1" },
			{ "port": "A", "id": "6", "int": "1" },
			{ "port": "A", "id": "7", "int": "1" },
			
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
		],
		"extint":
		[
			{ "port": "B", "id": "6", "int": "0" },
		],
		"usi":
		[
			{ "port": "B", "id": "2", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "1", "name": "do", "dir": "out" },
			{ "port": "B", "id": "0", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["attiny40"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			
			{ "port": "C", "id": "0", "int": "12" },
			{ "port": "C", "id": "1", "int": "13" },
			{ "port": "C", "id": "2", "int": "14" },
			{ "port": "C", "id": "3", "int": "15" },
			{ "port": "C", "id": "4", "int": "16" },
			{ "port": "C", "id": "5", "int": "17" },
		],
		"extint":
		[
			{ "port": "C", "id": "2", "int": "0" },
		],
		"spi":
		[
			{ "port": "C", "id": "4", "name": "miso", "dir": "in" },
			{ "port": "C", "id": "2", "name": "mosi", "dir": "out" },
			{ "port": "C", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "C", "id": "0", "name": "ss", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "1", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "4", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["attiny43u"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			{ "port": "B", "id": "4", "int": "12" },
			{ "port": "B", "id": "5", "int": "13" },
			{ "port": "B", "id": "6", "int": "14" },
			{ "port": "B", "id": "7", "int": "15" },
		],
		"extint":
		[
			{ "port": "B", "id": "7", "int": "0" },
		],
		"usi":
		[
			{ "port": "B", "id": "6", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "5", "name": "do", "dir": "out" },
			{ "port": "B", "id": "4", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["attiny48", "attiny88"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "24" },
			{ "port": "A", "id": "1", "int": "25" },
			{ "port": "A", "id": "2", "int": "26" },
			{ "port": "A", "id": "3", "int": "27" },
			
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "C", "id": "0", "int": "8" },
			{ "port": "C", "id": "1", "int": "9" },
			{ "port": "C", "id": "2", "int": "10" },
			{ "port": "C", "id": "3", "int": "11" },
			{ "port": "C", "id": "4", "int": "12" },
			{ "port": "C", "id": "5", "int": "13" },
			{ "port": "C", "id": "6", "int": "14" },
			{ "port": "C", "id": "7", "int": "15" },
			
			{ "port": "D", "id": "0", "int": "16" },
			{ "port": "D", "id": "1", "int": "17" },
			{ "port": "D", "id": "2", "int": "18" },
			{ "port": "D", "id": "3", "int": "19" },
			{ "port": "D", "id": "4", "int": "20" },
			{ "port": "D", "id": "5", "int": "21" },
			{ "port": "D", "id": "6", "int": "22" },
			{ "port": "D", "id": "7", "int": "23" },
		],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
		],
		"spi":
		[
			{ "port": "B", "id": "3", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "4", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "2", "name": "ss", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "C", "id": "5", "name": "scl", "dir": "out" },
			{ "port": "C", "id": "4", "name": "sda", "dir": "io" }
		]
	},
	
	{
		"devices": ["attiny87", "attiny167"],
		"extint":
		[
			{ "port": "B", "id": "6", "int": "0" },
			{ "port": "A", "id": "3", "int": "1" },
		],
		"spi":
		[
			{ "port": "A", "id": "4", "name": "miso", "dir": "in" },
			{ "port": "A", "id": "2", "name": "mosi", "dir": "out" },
			{ "port": "A", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "A", "id": "6", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "A", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "A", "id": "1", "name": "txd", "dir": "out" }
		],
		"usi":
		[
			{ "port": "B", "id": "2", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "1", "name": "do", "dir": "out" },
			{ "port": "B", "id": "0", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["attiny261", "attiny261a",
					"attiny461", "attiny461a",
					"attiny861", "attiny861a"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			{ "port": "B", "id": "4", "int": "12" },
			{ "port": "B", "id": "5", "int": "13" },
			{ "port": "B", "id": "6", "int": "14" },
			{ "port": "B", "id": "7", "int": "15" },
		],
		"extint":
		[
			{ "port": "B", "id": "6", "int": "0" },
			{ "port": "A", "id": "2", "int": "1" },
		],
		"usi":
		[
			{ "port": "B", "id": "2", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "1", "name": "do", "dir": "out" },
			{ "port": "B", "id": "0", "name": "di", "dir": "in" }
		]
	},
	
	{
		"devices": ["attiny828"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			{ "port": "B", "id": "4", "int": "12" },
			{ "port": "B", "id": "5", "int": "13" },
			{ "port": "B", "id": "6", "int": "14" },
			{ "port": "B", "id": "7", "int": "15" },
			
			{ "port": "C", "id": "0", "int": "16" },
			{ "port": "C", "id": "1", "int": "17" },
			{ "port": "C", "id": "2", "int": "18" },
			{ "port": "C", "id": "3", "int": "19" },
			{ "port": "C", "id": "4", "int": "20" },
			{ "port": "C", "id": "5", "int": "21" },
			{ "port": "C", "id": "6", "int": "22" },
			{ "port": "C", "id": "7", "int": "23" },
			
			{ "port": "D", "id": "0", "int": "24" },
			{ "port": "D", "id": "1", "int": "25" },
			{ "port": "D", "id": "2", "int": "26" },
			{ "port": "D", "id": "3", "int": "27" },
		],
		"extint":
		[
			{ "port": "C", "id": "1", "int": "0" },
			{ "port": "C", "id": "2", "int": "1" },
		],
		"spi":
		[
			{ "port": "D", "id": "0", "name": "miso", "dir": "in" },
			{ "port": "D", "id": "1", "name": "mosi", "dir": "out" },
			{ "port": "D", "id": "3", "name": "sck", "dir": "out" },
			{ "port": "C", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "C", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "C", "id": "3", "name": "txd", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "3", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "0", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["attiny1634"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "0" },
			{ "port": "A", "id": "1", "int": "1" },
			{ "port": "A", "id": "2", "int": "2" },
			{ "port": "A", "id": "3", "int": "3" },
			{ "port": "A", "id": "4", "int": "4" },
			{ "port": "A", "id": "5", "int": "5" },
			{ "port": "A", "id": "6", "int": "6" },
			{ "port": "A", "id": "7", "int": "7" },
			
			{ "port": "B", "id": "0", "int": "8" },
			{ "port": "B", "id": "1", "int": "9" },
			{ "port": "B", "id": "2", "int": "10" },
			{ "port": "B", "id": "3", "int": "11" },
			
			{ "port": "C", "id": "0", "int": "12" },
			{ "port": "C", "id": "1", "int": "13" },
		],
		"extint":
		[
			{ "port": "C", "id": "2", "int": "0" },
		],
		"uart0":
		[
			{ "port": "A", "id": "7", "name": "rxd", "dir": "in" },
			{ "port": "B", "id": "0", "name": "txd", "dir": "out" }
		],
		"usi":
		[
			{ "port": "B", "id": "1", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "2", "name": "do", "dir": "out" },
			{ "port": "C", "id": "1", "name": "di", "dir": "in" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["attiny2313a", "attiny2313",
					"attiny4313", "attiny4313a"],
		"pcint":
		[
			{ "port": "A", "id": "0", "int": "8" },
			{ "port": "A", "id": "1", "int": "9" },
			{ "port": "A", "id": "2", "int": "10" },
			
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "D", "id": "0", "int": "11" },
			{ "port": "D", "id": "1", "int": "12" },
			{ "port": "D", "id": "2", "int": "13" },
			{ "port": "D", "id": "3", "int": "14" },
			{ "port": "D", "id": "4", "int": "15" },
			{ "port": "D", "id": "5", "int": "16" },
			{ "port": "D", "id": "6", "int": "17" },
		],
		"extint":
		[
			{ "port": "D", "id": "2", "int": "0" },
			{ "port": "D", "id": "3", "int": "1" },
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "1", "name": "txd", "dir": "out" }
		],
		"usi":
		[
			{ "port": "B", "id": "7", "name": "usck", "dir": "out" },
			{ "port": "B", "id": "6", "name": "do", "dir": "out" },
			{ "port": "B", "id": "5", "name": "di", "dir": "in" }
		],
		"uartspi": []
	},
	
	# AT90 devices
	
	{
		"devices": ["at90can32", "at90can64", "at90can128"],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" }
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "B", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "dir": "in" },
			{ "port": "E", "id": "1", "name": "txd", "dir": "out" },
			{ "port": "E", "id": "2", "name": "xck", "dir": "out" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"can":
		[
			{ "port": "D", "id": "6", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "5", "name": "txd", "dir": "out" }
		]
	},
	
	{
		"devices": ["at90pwm1",
					"at90pwm2", "at90pwm2b", "at90pwm216"],
		"extint":
		[
			{ "port": "B", "id": "6", "int": "0" },
			{ "port": "D", "id": "2", "int": "1" },
			{ "port": "D", "id": "5", "int": "2" }
		],
		"spi":
		[
			{ "port": "B", "id": "1", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "0", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "D", "id": "3", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "4", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "0", "name": "xck", "dir": "out" }
		]
	},
	
	{
		"devices": ["at90pwm3", "at90pwm3b", "at90pwm316"],
		"extint":
		[
			{ "port": "B", "id": "6", "int": "0" },
			{ "port": "D", "id": "2", "int": "1" },
			{ "port": "D", "id": "5", "int": "2" },
			{ "port": "C", "id": "0", "int": "3" }
		],
		"spi":
		[
			{ "port": "B", "id": "1", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "0", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "7", "name": "sck", "dir": "out" },
			{ "port": "D", "id": "3", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "4", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "0", "name": "xck", "dir": "out" }
		]
	},
	
	{
		"devices": ["at90pwm81", "at90pwm161"],
		"extint":
		[
			{ "port": "B", "id": "2", "int": "0" },
			{ "port": "B", "id": "5", "int": "1" },
			{ "port": "E", "id": "0", "int": "2" }
		],
		"spi":
		[
			{ "port": "B", "id": "4", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "6", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "5", "name": "sck", "dir": "out" },
			{ "port": "D", "id": "0", "name": "ss", "dir": "out" }
		]
	},
	
	{
		"devices": ["at90usb82", "at90usb162"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" },
			
			{ "port": "C", "id": "6", "int": "8" },
			{ "port": "C", "id": "5", "int": "9" },
			{ "port": "C", "id": "4", "int": "10" },
			{ "port": "C", "id": "2", "int": "11" },
			{ "port": "D", "id": "5", "int": "12" }
		],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "C", "id": "7", "int": "4" },
			{ "port": "D", "id": "4", "int": "5" },
			{ "port": "D", "id": "6", "int": "6" },
			{ "port": "D", "id": "7", "int": "7" }
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "D", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"uartspi": []
	},
	
	{
		"devices": ["at90usb646", "at90usb1286",
					"at90usb647", "at90usb1287"],
		"pcint":
		[
			{ "port": "B", "id": "0", "int": "0" },
			{ "port": "B", "id": "1", "int": "1" },
			{ "port": "B", "id": "2", "int": "2" },
			{ "port": "B", "id": "3", "int": "3" },
			{ "port": "B", "id": "4", "int": "4" },
			{ "port": "B", "id": "5", "int": "5" },
			{ "port": "B", "id": "6", "int": "6" },
			{ "port": "B", "id": "7", "int": "7" }
		],
		"extint":
		[
			{ "port": "D", "id": "0", "int": "0" },
			{ "port": "D", "id": "1", "int": "1" },
			{ "port": "D", "id": "2", "int": "2" },
			{ "port": "D", "id": "3", "int": "3" },
			{ "port": "E", "id": "4", "int": "4" },
			{ "port": "E", "id": "5", "int": "5" },
			{ "port": "E", "id": "6", "int": "6" },
			{ "port": "E", "id": "7", "int": "7" }
		],
		"spi":
		[
			{ "port": "B", "id": "2", "name": "miso", "dir": "in" },
			{ "port": "B", "id": "3", "name": "mosi", "dir": "out" },
			{ "port": "B", "id": "1", "name": "sck", "dir": "out" },
			{ "port": "D", "id": "0", "name": "ss", "dir": "out" }
		],
		"uart0":
		[
			{ "port": "D", "id": "2", "name": "rxd", "dir": "in" },
			{ "port": "D", "id": "3", "name": "txd", "dir": "out" },
			{ "port": "D", "id": "5", "name": "xck", "dir": "out" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "dir": "out" },
			{ "port": "D", "id": "1", "name": "sda", "dir": "io" }
		],
		"uartspi": []
	}
]

