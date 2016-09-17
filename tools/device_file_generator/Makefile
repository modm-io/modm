
all: generate-avr generate-stm32

generate-avr:
	./avr_generator.py AT90
	./avr_generator.py ATtiny
	./avr_generator.py ATmega
	./avr_generator.py ATxmega
	
generate-stm32:
	./stm_generator.py STM32F0
	./stm_generator.py STM32F1
	./stm_generator.py STM32F3
	./stm_generator.py STM32F4
	./stm_generator.py STM32F7
