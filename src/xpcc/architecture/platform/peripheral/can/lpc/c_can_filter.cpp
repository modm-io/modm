
#include "c_can_filter.hpp"

// ----------------------------------------------------------------------------
void
xpcc::lpc::CanFilter::setFilter(
					ExtendedIdentifier id,
					ExtendedFilterMask mask,
					uint8_t firstMob,
					uint8_t size)
{
	uint8_t lastMob = firstMob + size - 1;
	for (uint8_t mob = firstMob; mob <= lastMob; ++mob) {
		// What to write
		LPC_CAN->IF1_CMDMSK =
				CAN_IFn_CMDMSK_WR |
				CAN_IFn_CMDMSK_MASK |
				CAN_IFn_CMDMSK_ARB |
				CAN_IFn_CMDMSK_CTRL |
				CAN_IFn_CMDMSK_DATAA |
				CAN_IFn_CMDMSK_DATAB;

		// Mask
		LPC_CAN->IF1_MSK1 = mask & 0xffff; // ID_EXT_MASK & 0xFFFF;
		LPC_CAN->IF1_MSK2 = mask >> 16;    // CAN_IFn_MSK2_MXTD | (ID_EXT_MASK >> 16);

		// Arbitration
		LPC_CAN->IF1_ARB1 = id & 0xffff;  // (RX_EXT_MSG_ID) & 0xFFFF;
		LPC_CAN->IF1_ARB2 = id >> 16;     // CAN_IFn_ARB2_MSGVAL | CAN_IFn_ARB2_XTD | ((RX_EXT_MSG_ID) >> 16);

		if (mob == lastMob ) {
			// End of FIFO block
			LPC_CAN->IF1_MCTRL =
					CAN_IFn_MCTRL_UMASK |
					CAN_IFn_MCTRL_RXIE |
					CAN_IFn_MCTRL_EOB |
					CAN_IFn_MCTRL_DLC_MAX;
		}
		else {
			LPC_CAN->IF1_MCTRL =
					CAN_IFn_MCTRL_UMASK |
					CAN_IFn_MCTRL_RXIE |
					CAN_IFn_MCTRL_DLC_MAX;
		}

		LPC_CAN->IF1_DA1 = 0x0000;
		LPC_CAN->IF1_DA2 = 0x0000;
		LPC_CAN->IF1_DB1 = 0x0000;
		LPC_CAN->IF1_DB2 = 0x0000;

		// Transfer data to message RAM
		LPC_CAN->IF1_CMDREQ = mob + 1;
		while( LPC_CAN->IF1_CMDREQ & CAN_IFn_CMDREQ_BUSY );
	}
}
