/*
 * Copyright (c) 2026, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/pio/pio_asm.hpp>

namespace modm::platform::pio {

	namespace implementation {

		template <typename T>
		static constexpr bool check(const T& t,uint16_t value) {
			struct Encoder {
				constexpr uint16_t encodeSideDelay(const OptionalValue& /*side*/,uint16_t /*delay*/) const {
					return 0;
				}
			};
			return t.encode(Encoder()) == value;
		}

	}

	// static checks instructions encoding
	// Jmp
	static_assert(implementation::check(Jmp,				0x0000),"jmp");
	static_assert(implementation::check(Jmp.not_x,			0x0020),"jmp !x");
	static_assert(implementation::check(Jmp.x_dec,			0x0040),"jmp x--");
	static_assert(implementation::check(Jmp.not_y,			0x0060),"jmp !y");
	static_assert(implementation::check(Jmp.y_dec,			0x0080),"jmp y--");
	static_assert(implementation::check(Jmp.x_ne_y, 		0x00a0),"jmp x!=y");
	static_assert(implementation::check(Jmp.pin,			0x00c0),"jmp pin");
	static_assert(implementation::check(Jmp.not_osre,		0x00e0),"jmp !osre");
	// Wait
	static_assert(implementation::check(Wait<0>.gpio<3>,	0x2003),"wait 0 gpio 3");
	static_assert(implementation::check(Wait<1>.gpio<3>,	0x2083),"wait 1 gpio 3");
	static_assert(implementation::check(Wait<0>.pin<3>,		0x2023),"wait 0 pin 3");
	static_assert(implementation::check(Wait<1>.pin<3>,		0x20a3),"wait 1 pin 3");
	static_assert(implementation::check(Wait<0>.irq<3>,		0x2043),"wait 0 irq 3");
	static_assert(implementation::check(Wait<1>.irq<3>,		0x20c3),"wait 1 irq 3");
	// In
	static_assert(implementation::check(In.pins<3>,			0x4003),"in pins, 3");
	static_assert(implementation::check(In.x<3>,			0x4023),"in x, 3");
	static_assert(implementation::check(In.y<3>,			0x4043),"in y, 3");
	static_assert(implementation::check(In.null<3>,			0x4063),"in null, 3");
	static_assert(implementation::check(In.isr<3>,			0x40c3),"in isr, 3");
	static_assert(implementation::check(In.osr<3>,			0x40e3),"in osr, 3");
	// Out
	static_assert(implementation::check(Out.pins<3>,		0x6003),"out pins, 3");
	static_assert(implementation::check(Out.x<3>,			0x6023),"out x, 3");
	static_assert(implementation::check(Out.y<3>,			0x6043),"out y, 3");
	static_assert(implementation::check(Out.null<3>,		0x6063),"out null, 3");
	static_assert(implementation::check(Out.pindirs<3>,		0x6083),"out pindirs, 3");
	static_assert(implementation::check(Out.pc<3>,			0x60a3),"out pc, 3");
	static_assert(implementation::check(Out.isr<3>,			0x60c3),"out isr, 3");
	static_assert(implementation::check(Out.exec<3>,		0x60e3),"out exec, 3");
	// Push
	static_assert(implementation::check(Push,				0x8020),"'push'");
	static_assert(implementation::check(Push.block,			0x8020),"'push block'");
	static_assert(implementation::check(Push.noblock,		0x8000),"'push noblock'");
	static_assert(implementation::check(Push.iffull,		0x8060),"'push iffull'");
	static_assert(implementation::check(Push.iffull.block,	0x8060),"'push iffull block'");
	static_assert(implementation::check(Push.iffull.noblock,0x8040),"'push iffull noblock'");
	// Pull
	static_assert(implementation::check(Pull,				0x80a0),"'pull'");
	static_assert(implementation::check(Pull.block,			0x80a0),"'pull block'");
	static_assert(implementation::check(Pull.noblock,		0x8080),"'pull noblock'");
	static_assert(implementation::check(Pull.ifempty,		0x80e0),"'pull ifempty'");
	static_assert(implementation::check(Pull.ifempty.block,	0x80e0),"'pull ifempty block'");
	static_assert(implementation::check(Pull.ifempty.noblock,0x80c0),"'pull ifempty noblock'");
	// Mov check only all encodings, not all variants
	static_assert(implementation::check(Mov.pins.pins,		0xa000),"mov pins,pins");
	static_assert(implementation::check(Mov.pins.invert.pins,0xa008),"mov pins,~pins");
	static_assert(implementation::check(Mov.pins.reverse.pins,0xa010),"mov pins,::pins");
	static_assert(implementation::check(Mov.x.pins,			0xa020),"mov x,pins");
	static_assert(implementation::check(Mov.x.invert.pins,	0xa028),"mov x,~pins");
	static_assert(implementation::check(Mov.x.reverse.pins,	0xa030),"mov x,::pins");
	static_assert(implementation::check(Mov.y.pins,			0xa040),"mov y,pins");
	static_assert(implementation::check(Mov.y.invert.pins,	0xa048),"mov y,~pins");
	static_assert(implementation::check(Mov.y.reverse.pins,	0xa050),"mov y,::pins");
	static_assert(implementation::check(Mov.exec.pins,		0xa080),"mov exec,pins");
	static_assert(implementation::check(Mov.exec.invert.pins,0xa088),"mov exec,~pins");
	static_assert(implementation::check(Mov.exec.reverse.pins,0xa090),"mov exec,::pins");
	static_assert(implementation::check(Mov.pc.pins,		0xa0a0),"mov pc,pins");
	static_assert(implementation::check(Mov.pc.invert.pins,	0xa0a8),"mov pc,~pins");
	static_assert(implementation::check(Mov.pc.reverse.pins,0xa0b0),"mov pc,::pins");
	static_assert(implementation::check(Mov.isr.pins,		0xa0c0),"mov isr,pins");
	static_assert(implementation::check(Mov.isr.invert.pins,0xa0c8),"mov isr,~pins");
	static_assert(implementation::check(Mov.isr.reverse.pins,0xa0d0),"mov isr,::pins");
	static_assert(implementation::check(Mov.osr.pins,		0xa0e0),"mov osr,pins");
	static_assert(implementation::check(Mov.osr.invert.pins,0xa0e8),"mov osr,~pins");
	static_assert(implementation::check(Mov.osr.reverse.pins,0xa0f0),"mov osr,::pins");

	static_assert(implementation::check(Mov.pins.x,			0xa001),"mov pins,x");
	static_assert(implementation::check(Mov.pins.invert.x,	0xa009),"mov pins,~x");
	static_assert(implementation::check(Mov.pins.reverse.x,	0xa011),"mov pins,::x");
	static_assert(implementation::check(Mov.pins.y,			0xa002),"mov pins,y");
	static_assert(implementation::check(Mov.pins.invert.y,	0xa00a),"mov pins,~y");
	static_assert(implementation::check(Mov.pins.reverse.y,	0xa012),"mov pins,::y");
	static_assert(implementation::check(Mov.pins.null,		0xa003),"mov pins,null");
	static_assert(implementation::check(Mov.pins.invert.null,	0xa00b),"mov pins,~null");
	static_assert(implementation::check(Mov.pins.reverse.null,	0xa013),"mov pins,::null");
	static_assert(implementation::check(Mov.pins.status,		0xa005),"mov pins,status");
	static_assert(implementation::check(Mov.pins.invert.status,	0xa00d),"mov pins,~status");
	static_assert(implementation::check(Mov.pins.reverse.status,0xa015),"mov pins,::status");
	static_assert(implementation::check(Mov.pins.isr,			0xa006),"mov pins,isr");
	static_assert(implementation::check(Mov.pins.invert.isr,	0xa00e),"mov pins,~isr");
	static_assert(implementation::check(Mov.pins.reverse.isr,	0xa016),"mov pins,::isr");
	static_assert(implementation::check(Mov.pins.osr,			0xa007),"mov pins,osr");
	static_assert(implementation::check(Mov.pins.invert.osr,	0xa00f),"mov pins,~osr");
	static_assert(implementation::check(Mov.pins.reverse.osr,	0xa017),"mov pins,::osr");
	// Irq
	//static_assert(implementation::check(Irq<3>,				0xc003),"irq 3"); // not supported, use 'irq set/irq nowait' equivalent
	static_assert(implementation::check(Irq.set<3>,			0xc003),"irq set 3");
	static_assert(implementation::check(Irq.nowait<3>,		0xc003),"irq nowait 3");
	static_assert(implementation::check(Irq.wait<3>,		0xc023),"irq wait 3");
	static_assert(implementation::check(Irq.clear<3>,		0xc043),"irq clear 3");
	// Set
	static_assert(implementation::check(Set.pins<3>,		0xe003),"set pins,3");
	static_assert(implementation::check(Set.x<3>,			0xe023),"set x,3");
	static_assert(implementation::check(Set.y<3>,			0xe043),"set y,3");
	static_assert(implementation::check(Set.pindirs<3>,		0xe083),"set pindirs,3");
}

