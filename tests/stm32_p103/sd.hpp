
#ifndef SD_HPP
#define SD_HPP

#include <xpcc/driver/storage/fat.hpp>

class SdCard : public xpcc::fat::Disk
{
public:
	SdCard();
	
	virtual xpcc::fat::Status
	initialize();
	
	virtual xpcc::fat::Status
	getStatus();
	
	virtual xpcc::fat::Result
	read(uint8_t *buffer, int32_t sectorNumber, uint8_t sectorCount);
	
	virtual xpcc::fat::Result
	write(const uint8_t *buffer, int32_t sectorNumber, uint8_t sectorCount);
	
	virtual xpcc::fat::Result
	ioctl(uint8_t command, uint32_t *buffer);
	
private:
	enum SpiSpeed
	{
		INTERFACE_SLOW,		///< Slow clock (100 - 400kHz)
		INTERFACE_FAST		///< Fast clock (>20 MHz)
	};
	
	void
	interface_speed(SpiSpeed speed);
	
	void
	socket_wp_init(void);
	
	uint32_t
	socket_is_write_protected(void);
	
	void
	socket_cp_init(void);
	
	uint32_t
	socket_is_empty(void);
	
	void
	card_power(bool on);
	
	bool
	chk_power(void);
	
	// Transmit/Receive a byte to MMC via SPI
	uint8_t
	stm32_spi_rw(uint8_t out);
	
	// Wait for card ready
	uint8_t
	wait_ready(void);
	
	// Deselect the card and release SPI bus
	void
	release_spi(void);
	
	void
	power_on(void);
	
	void
	power_off(void);
	
	// Receive a data packet from MMC
	bool
	rcvr_datablock(uint8_t *buff, UINT btr);
	
	// Send a data packet to MMC
	bool
	xmit_datablock(const uint8_t *buff, uint8_t token);
	
	// Send a command packet to MMC
	uint8_t
	send_cmd(uint8_t cmd, uint32_t arg);
	
public:
	// Device Timer Interrupt Procedure
	// This function must be called in period of 10ms
	void
	disk_timerproc(void);
	
private:
	uint8_t cardType;					///< Card type flags
	volatile DSTATUS globalDiskStatus;	///< Disk status
};

#endif // SD_HPP
