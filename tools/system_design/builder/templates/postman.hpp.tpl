// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically from postman.hpp.tpl
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef	POSTMAN_HPP
#define	POSTMAN_HPP

#include <xpcc/communication/xpcc/postman/postman.hpp>

class Postman : public xpcc::Postman
{
public:
	xpcc::Postman::DeliverInfo
	deliverPacket(const xpcc::Header& header, const xpcc::SmartPointer& payload);
	
	bool
	isComponentAvaliable(uint8_t component) const;
};

#endif	// POSTMAN_HPP
