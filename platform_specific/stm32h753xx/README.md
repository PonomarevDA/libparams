# STM32H753xx flash driver

According to the reference manual for STM32H7x3, the STM32H753xx internal flash is:

- Total size: 2 MBytes
- Dual-bank: 1 MByte per bank
- Sector size: 128 KBytes
- Flash word program size: 256 bits (32 bytes)

So we have 8 sectors per bank (16 sectors total).

The main memory is:

| Area   | Name                                       | hex address                         | Size      |
| ------ | ------------------------------------------ | ----------------------------------- | --------- |
| Bank 2 | Sector 15 <br /> ... <br /> Sector 8       | 0x081E 0000 - 0x081F FFFF <br /> ... <br /> 0x0810 0000 - 0x0811 FFFF | 128 KByte <br /> ... <br /> 128 KByte |
| Bank 1 | Sector 7 <br /> ... <br /> Sector 0        | 0x080E 0000 - 0x080F FFFF <br /> ... <br /> 0x0800 0000 - 0x0801 FFFF | 128 KByte <br /> ... <br /> 128 KByte |
