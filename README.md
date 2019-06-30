# 1 SimpleLink-CC3200 Overview
## 1.1 Basic Development Environment
* MCU : TI SimpleLink Wi-Fi CC3200(Arm Cortex-M4)
* SDK : CC3200 SDK v1.3.0
* OS  : FreeRTOS v8.0.1
* IDE(based on Windows) : IAR Embeded Workbench for Arm 8.3 (EWARM-CD-8301-17148.exe)
* Tools : UniFlash v3.4 | PinMux v4.0 | TeraTerm v4.1
## 1.2 Directory
* Doc
* Library
* Project
* Source
    * main.c
    * freertos
    * html
    * myapi
    * network
        * simplelink
        * simplelink_extlib
## 1.3 Workspace
* App
    * main.c
* Bsp
    * Wi-Fi
        * network.c
        * transport.c
    * adc_if.c
    * global.c
    * gpio_if.c
    * i2c_if.c
    * pinmux.c
    * pwm_if.c
    * sensor.c
    * spi_if.c
    * uart_if.c
* Doc
* FreeRTOS
* Network
    * SimpleLink
    * SimpleLink_extlib
# 2 CC3200 Design
## 2.1 Hardware Design
### 2.1.1 CC3200 LaunchPad Version
Please refer to [TI SimpleLink CC3200 LaunchPad](http://www.ti.com/tool/cc3200-launchxl) for hardware design & pcb layout. The software of project is based on this version.
### 2.1.2 Development Version (Failed)
My hardware design is failed. This is for your reference only.
Function|Pin Number|Pin Name|Signal Name|Signal Description
:--------:|:----------:|:--------:|:-----------:|:------------------:
PWM|1|GPIO10|GT_PWM06|GPTA3A_PWM

|I2C|2|GPIO11|I2C_SDA|I2C Data|
|I2C|3|GPIO12|I2C_SCL|I2C Clock|
|Button|4|GPIO13|GPIO13|Button SW1 Input|
|GSPI|5|GPIO14|GSPI_CLK|General SPI Clock|
|GSPI|6|GPIO15|GSPI_MISO|General SPI MISO|
|GSPI|7|GPIO16|GSPI_MOSI|General SPI MOSI|
|GSPI|8|GPIO17|GSPI_CS|General SPI Chip Select|
|Flash|11|FLASH_SPI_CLK|FLASH_SPI_CLK|Serial SPI Clock|
|Flash|12|FLASH_SPI_DOUT|FLASH_SPI_DOUT|Serial SPI Data Output|
|Flash|13|FLASH_SPI_DIN|FLASH_SPI_DIN|Serial SPI Data Input|
|Flash|14|FLASH_SPI_CS|FLASH_SPI_CS|Serial SPI Chip Select|
|Button|15|GPIO22|GPIO22|Button SW2 Input|
|JTAG|16|TDI|TDI|JTAG TDI|
|JTAG|17|TDO|TDO|JTAG TDO|
|JTAG/SWD|19|TCK|TCK|JTAG/SWD TCK|
|JTAG/SWD|20|TMS|TMS|JTAG/SWD TMS|
|SOP|21|SOP2|SOP2|Boot Option|
|Radio Frequency|31|RF_BG|RF_BG|Radio Frequency BG Band|
|Reset|32|nRESET|nRESET|Chip Reset|
|SOP|34|SOP1|SOP1|Boot Option|
|SOP|35|SOP0|SOP0|Boot Option|
|UART|55|GPIO1|UART0_TX|Serial Send|
|UART|57|GPIO2|UART0_RX|Serial Receive|
|ADC|58|GPIO3|ADC_CH1|ADC Channel 1|
|ADC|59|GPIO4|ADC_CH2|ADC Channel 2|
|ADC|60|GPIO5|ADC_CH3|ADC Channel 3|
|LED|61|GPIO6|GPIO6|LED1|
|LED|62|GPIO7|GPIO7|LED2|
|LED|63|GPIO8|GPIO8|LED3|
|PWM|64|GPIO9|GT_PWM05|GPTA2B_PWM|
## 2.2 Software Architecture
### 2.2.2 IoT Platform
* Cloud Server : [Tencent Cloud](https://cloud.tencent.com/) 
* Upper Monitor : [WeChat Interface Test](https://mp.weixin.qq.com/debug/cgi-bin/sandbox?t=sandbox/login)
# References
* [CC3100/CC3200 SimpleLink™ Wi-Fi ® Internet-on-a-Chip User's Guide](http://www.ti.com/lit/ug/swru368b/swru368b.pdf)
* [CC3100 and CC3200 SimpleLink™ Wi-Fi ® and Internet of Things Solution Layout Guidelines User's Guide](http://www.ti.com/lit/ug/swru370b/swru370b.pdf)
* [CC3200 SimpleLink Wi-Fi and Internet-of-Things Solution, a Single Chip Wireless MCU Technical Reference Manual](http://www.ti.com/lit/ug/swru367d/swru367d.pdf)
* [CC3200 SimpleLink™ Wi-Fi ® and IoT Solution with MCU LaunchPad Hardware User's Guide](http://www.ti.com/lit/ug/swru372b/swru372b.pdf)
* [CC3200 SimpleLink Wi-Fi and IoT Solution With MCU LaunchPad Getting Started Guide User's Guide](http://www.ti.com/lit/ug/swru376e/swru376e.pdf)
* [CC3200 SimpleLink Wi-Fi and Internet-of-Things Solution, a Single-Chip Wireless MCU datasheet](http://www.ti.com/lit/ds/swas032f/swas032f.pdf)
* 《ARM Cortex-M4+Wi-Fi MCU应用指南 CC3200 IAR基础篇》 郭书军
* 《CC3200Wi-Fi微控制器原理与实践：基于MiCO物联网操作系统》 沈建华
* 《智能家居控制系统的设计与开发：TI CC3200+物联网云平台+微信》 王立华
