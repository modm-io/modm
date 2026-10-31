# Direct Memory Access (DMA)

Driver for the linked-list DMA controllers of the newer STM32 devices: the
general purpose DMA (GPDMA), the high-performance DMA (HPDMA) and the
low-power DMA (LPDMA), for example, on the STM32H5, H7R/S, U3 and U5.

The channels implement the same interface as the other STM32 DMA drivers, so
that the peripheral drivers work with any of them:

```cpp
using Channel = Gpdma1::Channel0;
Gpdma1::enable();
Channel::configure(DmaBase::DataTransferDirection::MemoryToPeripheral,
                   DmaBase::MemoryDataSize::Byte, DmaBase::PeripheralDataSize::Byte,
                   DmaBase::MemoryIncrementMode::Increment,
                   DmaBase::PeripheralIncrementMode::Fixed);
Channel::setPeripheralRequest<Channel::RequestMapping<Peripheral::Spi1, DmaBase::Signal::Tx>::Request>();
Channel::setMemoryAddress(uintptr_t(buffer));
Channel::setPeripheralAddress(uintptr_t(&SPI1->TXDR));
Channel::setDataLength(sizeof(buffer));
Channel::start();
```

The request numbers are generated from the device data, every controller type
has its own request map. The data length counts the items of the source data
size, the hardware limits a transfer to 65535 bytes.

The circular mode is implemented with one linked-list item per channel, which
reloads the block size and the addresses after every transfer. The item is a
static variable and must be accessible by the DMA, like all transfer buffers.

Not supported: linked-list transfers with more than one item, 2D addressing,
bursts, triggers, the secure and privileged channel configuration.
