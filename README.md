# bldc-horseman

Accelerometer datasheet:
http://www.st.com/content/ccc/resource/technical/document/datasheet/a3/f5/4f/ae/8e/44/41/d7/DM00133076.pdf/files/DM00133076.pdf/jcr:content/translations/en.DM00133076.pdf

LSM6DS3 - accelerometer/gyro
STM32F103 - mcu
ACS758 - current sensor


# IDE requerments

Atollic TrueSTUDIO for ARM 7.0.1

STM32CubeMX

# pinout

analog:

pot - a0	- pa0
A	- a1	- pa1
B	- a11	- pc1
C	- a10	- pc0
N	- a4	- pa4
cur - a6	- pa6

digital:

AH - pa8
AL - pa7

BH - pa9
BL - pb0

CH - pa10
CL - pb1

SPI