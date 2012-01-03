
#include <xpcc_config.hpp>

#if STM32_AHB_FREQUENCY != F_CPU
#	error "STM32_AHB_FREQUENCY != F_CPU! Either define clock correctly or \
redefine STM32_AHB_FREQUENCY, STM32_APB1_FREQUENCY and \
STM32_APB2_FREQUENCY in your project.cfg"
#endif

