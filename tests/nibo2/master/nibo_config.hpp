
#ifndef NIBO_CONFIG_HPP
#define NIBO_CONFIG_HPP

#include <xpcc/architecture/driver/gpio.hpp>

namespace nibo
{
	GPIO__OUTPUT(FloorEnable, F, 6);
	GPIO__INPUT(Button, D, 4);
	
	GPIO__OUTPUT(LcdEn, G, 2);
	GPIO__OUTPUT(LcdRw, G, 4);
	GPIO__OUTPUT(LcdRs, G, 3);
	GPIO__OUTPUT(LcdCs1, G, 0);
	GPIO__OUTPUT(LcdCs2, G, 1);
	GPIO__PORT(LcdData, A);
	
	GPIO__OUTPUT(Audio, B, 4);	// OC0
	
	GPIO__OUTPUT(SlaveReset, D, 7);
	GPIO__IO(SlaveSelect, B, 0);
	GPIO__INPUT(Sck, B, 1);
	GPIO__INPUT(Mosi, B, 2);
	GPIO__OUTPUT(Miso, B, 3);
	
	typedef xpcc::Ks0108<LcdEn, LcdRw, LcdRs, LcdCs1, LcdCs2, LcdData> Display;
}

#endif // NIBO_CONFIG_HPP
