
#ifndef INTERCONNECT_STRUCTURE_HPP
#define INTERCONNECT_STRUCTURE_HPP

struct MasterToSlave
{
	uint8_t ledCounter;
	uint8_t ledValue;
};

struct SlaveToMaster
{
	uint8_t ledResponse;
};


#endif	// INTERCONNECT_STRUCTURE_HPP
