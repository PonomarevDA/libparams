# STM32G0B1xx flash driver

According to [the reference manual](https://www.st.com/resource/en/reference_manual/rm0444-stm32g0x1-advanced-armbased-32bit-mcus-stmicroelectronics.pdf) STM32G0B1xx has different page size depends on flash size:

Up to 512 Kbytes of Flash memory (Main memory):
- up to 64 Kbytes for STM32G031xx and STM32G041xx / STM32G051xx and
STM32G061xx
- up to 128 Kbytes for STM32G071xx and STM32G081xx
- **up to 512 Kbytes for STM32G0B1xx** and STM32G0C1xx
- Page size: 2 Kbytes
- Subpage size: 512 bytes

So, we have 512 Kbytes dual-bank device.

The main memory is:

| Area    | Name          | hex address               | Size        |
| ------- | ------------- | ------------------------- | ----------- |
| Bank 2 | Page 383 <br /> Page 258-382 <br /> Page 257 <br /> Page 256 | 0x0807 F800 - 0x0807 FFFF <br /> ... <br /> 0x0804 0800 - 0x0804 0FFF <br /> 0x0804 0000 - 0x0804 07FF | 2 KByte <br /> ... <br /> 2 KByte <br /> 2 KByte |
| Bank 1 | Page 127 <br /> Page 2-126 <br /> Page 1 <br /> Page 0 | 0x0803 F800 - 0x0803 FFFF <br /> ... <br /> 0x0800 0800 - 0x0800 0FFF <br /> 0x0800 0000 - 0x0800 07FF | 2 KByte <br /> ... <br /> 2 KByte <br /> 2 KByte |
