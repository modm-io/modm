# Analog-to-Digital Converter (ADC)

Driver for the ADC of the STM32C5, STM32G5 and STM32H7P (ST IP `aditf6`) with
single software-triggered regular conversions. The ADC kernel clock must be
selected and divided in the RCC before calling `initialize()`, the ADC
sequencer, interrupts, DMA, oversampling and differential inputs are not
supported yet.
