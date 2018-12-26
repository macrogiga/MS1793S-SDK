****MS1793S resources****

SPI2
CSN: GPB15
SCK: GPB14
MOSI:GPB13
MISO:GPB12


IRQ: GPA12

KEY: GPA0

UART1/I2C:
	PB6 - I2C_SCL/TX
	PB7 - I2C_SDA/RX

PWM(6)/ADC(7):
	GPA0 - ADC0/TIM2_CH1_ETR
	GPA1 - ADC1/TIM2_CH2
	PGA4 - ADC4/TIM14_CH1/SPI1_NSS
	GPA5 - ADC5/TIM2_CH1_ETR/SPI1_SCK
	GPA6 - ADC6/TIM3_CH1/TIM16_CH1/TIM1_BKIN/SPI1_MISO
	GPA7 - ADC7/TIM3_CH2/TIM14_CH1/TIM17_CH1/TIM1_CH1N/SPI1_MOSI
	GPB0 - ADC8/TIM3_CH3



****Sample applications****

1====UART数据透传应用==============
TX:GPB6 
RX:GPB7
模拟流控，用于STOP mode
    RTS:PA13
    CTS:PA14



2====i2c_master/slave=====
i2cmaster + OLED
  PB6 i2c_scl
  PB7 i2c_sda

i2cslave
  PB6 i2c_scl
  PB7 i2c_sda
  PA13 i2c send flag



3======LowPower=======
2分钟无连接进入低功耗状态（5uA）
PA0 按键唤醒



4====== PWM =========
GPA6   LED-R
GPA7   LED-G
GPB0   LED-B
GPA1   LED-Y
GPA4   LED-W