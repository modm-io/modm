#ifndef THREAD_DISPLAY
#define THREAD_DISPLAY

#include <xpcc/processing/timer.hpp>
#include <xpcc/processing/protothread.hpp>

#include <xpcc/driver/display/ssd1306.hpp>

#include "hardware.hpp"

class DisplayThread: public xpcc::pt::Protothread
{
public:
	DisplayThread();

	bool
	update();

	bool
	setSeq(int32_t seq) {
		_dirty = true;
		_seq = seq;
		return true;
	}

	bool
	setTemp(int32_t temp) {
		_dirty = true;
		_temp = temp;
		return true;
	}

	bool
	setPres(int32_t pres) {
		_dirty = true;
		_pres = pres;
		return true;
	}

	bool
	setHumi(int32_t humi) {
		_dirty = true;
		_humi = humi;
		return true;
	}

protected:
	xpcc::Ssd1306<MyI2cMaster, 64> display;
	xpcc::ShortTimeout boot_timeout;
	bool _dirty;
	int32_t _seq;
	int32_t _temp;
	int32_t _pres;
	int32_t _humi;
};

#endif
