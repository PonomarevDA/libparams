# STM32F103 flash driver

According to [the reference manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf) STM32F1 has different page size depends on flash size:

| Name | Series | Flash density, KB | Page size, KB |
| ---- | ------ | ----------------- | ------------- |
| Low-density | STM32F101xx, STM32F102xx, STM32F103xx | 16 - 32 | 1 |
| **Medium-density** | STM32F101xx, STM32F102xx, **STM32F103xx** | **64 - 128** | **1** |
| High-density | STM32F101xx, STM32F103xx | 256 - 512 | 2 |
| XL-density | STM32F101xx, STM32F103xx | 768 - 1024 | 2 |
| Connectivity line devices | STM32F105xx, STM32F107xx | any | 1 |

The memory table for stm32f103 with 128 KBytes might rbe represented as below:

| Name    | hex address               | Size        |
| ------- | ------------------------- | ----------- |
| Page 0  | 0x0800 0000 - 0x0800 03FF | 1 KByte     |
| Page 1  | 0x0800 0400 - 0x0800 07FF | 1 KByte     |
| Page 2  | 0x0800 0800 -             | 1 KByte     |
| Page 4  | 0x0800 1000 -             | 1 KByte     |
| Page 8  | 0x0800 2000 -             | 1 KByte     |
| Page 16 | 0x0800 4000 -             | 1 KByte     |
| Page 32 | 0x0800 8000 -             | 1 KByte     |
| Page 63 | 0x0800 FC00 - 0x0800 FFFF | 1 KByte     |
| Page 64 | 0x0801 0000 -             | 1 KByte     |
| Page 127| 0x0801 FC00 - 0x0801 FCFF | 1 KByte     |
| Page 128| 0x0802 0000 -             | 1 KByte     |
