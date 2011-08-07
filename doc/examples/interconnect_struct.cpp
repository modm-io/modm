
struct MasterToSlave
{
	uint8_t commandCounter;
	Parameter commandParameter;
};

struct SlaveToMaster
{
	uint8_t commandResponse;
};
