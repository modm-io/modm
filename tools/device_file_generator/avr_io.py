# -*- coding: utf-8 -*-
# 
# Copyright (c) 2013, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of the Roboterclub Aachen e.V. nor the
#    names of its contributors may be used to endorse or promote products
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
		{ "id": "4", "name": "ss", "type": "input" },
		{ "id": "5", "name": "mosi", "type": "output" },
		{ "id": "6", "name": "miso", "type": "input" },
		{ "id": "7", "name": "sck", "type": "output" }
	],
	"uart":
	[
		{ "id": "1", "name": "xck", "type": "output", "instance": "0" },
		{ "id": "2", "name": "rxd", "type": "input", "instance": "0" },
		{ "id": "3", "name": "txd", "type": "output", "instance": "0" },
		{ "id": "5", "name": "xck", "type": "output", "instance": "1" },
		{ "id": "6", "name": "rxd", "type": "input", "instance": "1" },
		{ "id": "7", "name": "txd", "type": "output", "instance": "1" }
	],
	"i2c":
	[
		{ "id": "0", "name": "sda", "type": "io" },
		{ "id": "1", "name": "scl", "type": "output" }
	],
	"timer":
	[
		{ "id": "0", "name": "A", "instance": "0" },
		{ "id": "1", "name": "B", "instance": "0" },
		{ "id": "2", "name": "C", "instance": "0" },
		{ "id": "3", "name": "D", "instance": "0" },
		{ "id": "4", "name": "A", "instance": "1" },
		{ "id": "5", "name": "B", "instance": "1" }
	],
	"awex":
	[
		{ "id": "0", "name": "ALS" },
		{ "id": "1", "name": "AHS" },
		{ "id": "2", "name": "BLS" },
		{ "id": "3", "name": "BHS" },
		{ "id": "4", "name": "CLS" },
		{ "id": "5", "name": "CHS" },
		{ "id": "6", "name": "DLS" },
		{ "id": "7", "name": "DHS" }
	],
	"usb":
	[
		{ "id": "0", "name": "DM" },
		{ "id": "1", "name": "DP" }
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
			{ "port": "B", "id": "3", "name": "miso", "type": "input" },
			{ "port": "B", "id": "4", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "5", "name": "sck", "type": "output" },
			{ "port": "B", "id": "2", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" }
		],
		"i2c":
		[
			{ "port": "C", "id": "5", "name": "scl", "type": "output" },
			{ "port": "C", "id": "4", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "5", "name": "miso", "type": "input" },
			{ "port": "B", "id": "6", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" }
		],
		"i2c":
		[
			{ "port": "C", "id": "0", "name": "scl", "type": "output" },
			{ "port": "C", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "E", "id": "1", "name": "txd", "type": "output" },
			{ "port": "E", "id": "2", "name": "xck", "type": "output" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "C", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "C", "id": "0", "name": "txd", "type": "output" }
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
			{ "port": "B", "id": "3", "name": "miso", "type": "input" },
			{ "port": "B", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "4", "name": "sck", "type": "output" },
			{ "port": "B", "id": "1", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "4", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" }
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
			{ "port": "B", "id": "6", "name": "miso", "type": "input" },
			{ "port": "B", "id": "7", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "5", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"i2c":
		[
			{ "port": "C", "id": "4", "name": "scl", "type": "output" },
			{ "port": "C", "id": "3", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "6", "name": "miso", "type": "input" },
			{ "port": "B", "id": "7", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "5", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "B", "id": "1", "name": "rxd", "type": "input" },
			{ "port": "B", "id": "3", "name": "txd", "type": "output" }
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
			{ "port": "B", "id": "3", "name": "miso", "type": "input" },
			{ "port": "B", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "3", "name": "miso", "type": "input" },
			{ "port": "B", "id": "4", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "5", "name": "sck", "type": "output" },
			{ "port": "B", "id": "2", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" },
			{ "port": "D", "id": "4", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "C", "id": "5", "name": "scl", "type": "output" },
			{ "port": "C", "id": "4", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "E", "id": "1", "name": "txd", "type": "output" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "5", "name": "miso", "type": "input" },
			{ "port": "B", "id": "6", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" },
			{ "port": "D", "id": "4", "name": "xck", "type": "output" }
		],
		"uart1":
		[
			{ "port": "B", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "B", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "2", "name": "xck", "type": "output" }
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
			{ "port": "B", "id": "5", "name": "miso", "type": "input" },
			{ "port": "B", "id": "6", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" },
			{ "port": "B", "id": "0", "name": "xck", "type": "output" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "4", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "C", "id": "0", "name": "scl", "type": "output" },
			{ "port": "C", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "E", "id": "1", "name": "txd", "type": "output" },
			{ "port": "E", "id": "2", "name": "xck", "type": "output" }
		],
		"usi":
		[
			{ "port": "E", "id": "4", "name": "usck", "type": "output" },
			{ "port": "E", "id": "6", "name": "do", "type": "output" },
			{ "port": "E", "id": "5", "name": "di", "type": "input" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "E", "id": "1", "name": "txd", "type": "output" },
			{ "port": "E", "id": "2", "name": "xck", "type": "output" }
		],
		"usi":
		[
			{ "port": "E", "id": "4", "name": "usck", "type": "output" },
			{ "port": "E", "id": "6", "name": "do", "type": "output" },
			{ "port": "E", "id": "5", "name": "di", "type": "input" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"uart2":
		[
			{ "port": "H", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "H", "id": "1", "name": "txd", "type": "output" },
			{ "port": "H", "id": "2", "name": "xck", "type": "output" }
		],
		"uart3":
		[
			{ "port": "J", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "J", "id": "1", "name": "txd", "type": "output" },
			{ "port": "J", "id": "2", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "5", "name": "miso", "type": "input" },
			{ "port": "B", "id": "6", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "01", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" },
			{ "port": "D", "id": "4", "name": "xck", "type": "output" }
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
			{ "port": "B", "id": "5", "name": "miso", "type": "input" },
			{ "port": "B", "id": "6", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "B", "id": "4", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" },
			{ "port": "B", "id": "0", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "C", "id": "0", "name": "scl", "type": "output" },
			{ "port": "C", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "3", "name": "miso", "type": "input" },
			{ "port": "B", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "E", "id": "1", "name": "txd", "type": "output" },
			{ "port": "E", "id": "2", "name": "xck", "type": "output" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
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
		"spi":
		[
			{ "port": "B", "id": "0", "name": "miso", "type": "input" },
			{ "port": "B", "id": "1", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "2", "name": "sck", "type": "output" }
		]
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
			{ "port": "A", "id": "1", "name": "miso", "type": "input" },
			{ "port": "A", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "B", "id": "6", "name": "ss", "type": "input" }
		],
		"i2c":
		[
			{ "port": "A", "id": "7", "name": "scl", "type": "output" },
			{ "port": "B", "id": "1", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "6", "name": "miso", "type": "input" },
			{ "port": "B", "id": "5", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "4", "name": "sck", "type": "output" }
		],
		"usi":
		[
			{ "port": "A", "id": "4", "name": "usck", "type": "output" },
			{ "port": "A", "id": "5", "name": "do", "type": "output" },
			{ "port": "A", "id": "6", "name": "di", "type": "input" }
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
		"spi":
		[
			{ "port": "B", "id": "1", "name": "miso", "type": "input" },
			{ "port": "B", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "3", "name": "sck", "type": "output" }
		],
		"usi":
		[
			{ "port": "B", "id": "2", "name": "usck", "type": "output" },
			{ "port": "B", "id": "1", "name": "do", "type": "output" },
			{ "port": "B", "id": "0", "name": "di", "type": "input" }
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
			{ "port": "B", "id": "2", "name": "usck", "type": "output" },
			{ "port": "B", "id": "1", "name": "do", "type": "output" },
			{ "port": "B", "id": "0", "name": "di", "type": "input" }
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
			{ "port": "C", "id": "4", "name": "miso", "type": "input" },
			{ "port": "C", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "C", "id": "1", "name": "sck", "type": "output" },
			{ "port": "C", "id": "0", "name": "ss", "type": "input" }
		],
		"i2c":
		[
			{ "port": "C", "id": "1", "name": "scl", "type": "output" },
			{ "port": "C", "id": "4", "name": "sda", "type": "io" }
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
			{ "port": "B", "id": "6", "name": "usck", "type": "output" },
			{ "port": "B", "id": "5", "name": "do", "type": "output" },
			{ "port": "B", "id": "4", "name": "di", "type": "input" }
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
			{ "port": "B", "id": "3", "name": "miso", "type": "input" },
			{ "port": "B", "id": "4", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "5", "name": "sck", "type": "output" },
			{ "port": "B", "id": "2", "name": "ss", "type": "input" }
		],
		"i2c":
		[
			{ "port": "C", "id": "5", "name": "scl", "type": "output" },
			{ "port": "C", "id": "4", "name": "sda", "type": "io" }
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
			{ "port": "A", "id": "4", "name": "miso", "type": "input" },
			{ "port": "A", "id": "2", "name": "mosi", "type": "output" },
			{ "port": "A", "id": "5", "name": "sck", "type": "output" },
			{ "port": "A", "id": "6", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "A", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "A", "id": "1", "name": "txd", "type": "output" }
		],
		"usi":
		[
			{ "port": "B", "id": "2", "name": "usck", "type": "output" },
			{ "port": "B", "id": "1", "name": "do", "type": "output" },
			{ "port": "B", "id": "0", "name": "di", "type": "input" }
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
			{ "port": "B", "id": "2", "name": "usck", "type": "output" },
			{ "port": "B", "id": "1", "name": "do", "type": "output" },
			{ "port": "B", "id": "0", "name": "di", "type": "input" }
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
			{ "port": "D", "id": "0", "name": "miso", "type": "input" },
			{ "port": "D", "id": "1", "name": "mosi", "type": "output" },
			{ "port": "D", "id": "3", "name": "sck", "type": "output" },
			{ "port": "C", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "C", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "C", "id": "3", "name": "txd", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "3", "name": "scl", "type": "output" },
			{ "port": "D", "id": "0", "name": "sda", "type": "io" }
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
			{ "port": "A", "id": "7", "name": "rxd", "type": "input" },
			{ "port": "B", "id": "0", "name": "txd", "type": "output" }
		],
		"usi":
		[
			{ "port": "B", "id": "1", "name": "usck", "type": "output" },
			{ "port": "B", "id": "2", "name": "do", "type": "output" },
			{ "port": "C", "id": "1", "name": "di", "type": "input" }
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
			{ "port": "D", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "1", "name": "txd", "type": "output" }
		],
		"usi":
		[
			{ "port": "B", "id": "7", "name": "usck", "type": "output" },
			{ "port": "B", "id": "6", "name": "do", "type": "output" },
			{ "port": "B", "id": "5", "name": "di", "type": "input" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "B", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "E", "id": "0", "name": "rxd", "type": "input" },
			{ "port": "E", "id": "1", "name": "txd", "type": "output" },
			{ "port": "E", "id": "2", "name": "xck", "type": "output" }
		],
		"uart1":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
		],
		"can":
		[
			{ "port": "D", "id": "6", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "5", "name": "txd", "type": "output" }
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
			{ "port": "B", "id": "1", "name": "miso", "type": "input" },
			{ "port": "B", "id": "0", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "D", "id": "3", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "4", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "0", "name": "xck", "type": "output" }
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
			{ "port": "B", "id": "1", "name": "miso", "type": "input" },
			{ "port": "B", "id": "0", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "7", "name": "sck", "type": "output" },
			{ "port": "D", "id": "3", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "4", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "0", "name": "xck", "type": "output" }
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
			{ "port": "B", "id": "4", "name": "miso", "type": "input" },
			{ "port": "B", "id": "6", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "5", "name": "sck", "type": "output" },
			{ "port": "D", "id": "0", "name": "ss", "type": "input" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "D", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
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
			{ "port": "B", "id": "2", "name": "miso", "type": "input" },
			{ "port": "B", "id": "3", "name": "mosi", "type": "output" },
			{ "port": "B", "id": "1", "name": "sck", "type": "output" },
			{ "port": "D", "id": "0", "name": "ss", "type": "input" }
		],
		"uart0":
		[
			{ "port": "D", "id": "2", "name": "rxd", "type": "input" },
			{ "port": "D", "id": "3", "name": "txd", "type": "output" },
			{ "port": "D", "id": "5", "name": "xck", "type": "output" }
		],
		"i2c":
		[
			{ "port": "D", "id": "0", "name": "scl", "type": "output" },
			{ "port": "D", "id": "1", "name": "sda", "type": "io" }
		],
		"uartspi": []
	}
]

