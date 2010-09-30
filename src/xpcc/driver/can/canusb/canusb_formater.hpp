#ifndef XPCC__CANUSB_FORMATER_HPP
#define XPCC__CANUSB_FORMATER_HPP

#include "../message.hpp"
namespace xpcc
{
	class CanUsbFormater
	{
	public:
		static bool
		convertToCanMessage(const char* in,can::Message& out);

		static bool
		convertToString(const can::Message& in, char* out);
	private:
		static inline uint8_t
		hexToByte(const char *s)
		{
			return (charToByte(s) << 4) | charToByte(s + 1);
		}

		static uint8_t
		charToByte(const char *s);

		static char
		byteToHex(uint8_t num);


	};
}
#endif /* XPCC__CANUSB_FORMATER_HPP */
